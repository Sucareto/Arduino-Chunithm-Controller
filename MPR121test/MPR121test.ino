#if defined(__AVR_ATmega32U4__)
#pragma message "当前的开发板是 ATmega32U4"
#define SerialDevice SerialUSB
uint8_t RstBtn = A3;
#define Enable_LED
#define LED_PIN 6

#elif defined(ARDUINO_SAMD_ZERO)
#pragma message "当前的开发板是 SAMD_ZERO"
#define SerialDevice SerialUSB
uint8_t RstBtn = 7;
#define Enable_HID
#define LED_PIN 5

#elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
#pragma message "当前的开发板是 NODEMCU_ESP12E"
#define SerialDevice Serial
uint8_t RstBtn = D6;
#define Enable_LED
#define LED_PIN D5

#else
#error "未经测试的开发板，请检查串口和针脚定义"
#endif

#ifdef Enable_LED
#include "FastLED.h"
#define LED_NUM 16 //LED数量
CRGB leds[LED_NUM];
#endif

#include "Adafruit_MPR121.h"
Adafruit_MPR121 capL, capR;
#define Threshold 10 //触摸触发阈值

#define CLAMP(val) (val < 0 ? 0 : (val > 255 ? 255 : val))
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

void setup() {
  SerialDevice.begin(115200);
  SerialDevice.setTimeout(0);
#ifdef Enable_LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(50);//设置亮度
  FastLED.clear();
#endif
  pinMode(RstBtn, INPUT_PULLUP);//重启mpr121用按钮
  while (!(capL.begin(0x5A) & capR.begin(0x5C))) {
    delay(500);
  }
  MprSetup(capL);
  MprSetup(capR);
  Wire.setClock(800000);
#ifdef Enable_LED
  fill_solid(leds, LED_NUM, CRGB::Purple);
  FastLED.show();
#endif
}

unsigned long ReqTime;
void loop() {
  ReqTime = micros();
  int16_t bv, fd, pressure;
  for (int i = 0; i < 8; i++) {
    bv = capL.baselineData(i);
    fd = capL.filteredData(i);
    pressure = bv - fd - Threshold + 20;
    leds[i] = pressure > 20 ? 0xFF00FF : 0xFF8200;
    SerialDevice.print(CLAMP(pressure));
    SerialDevice.print("\t");
  }
  for (int i = 0; i < 8; i++) {
    bv = capR.baselineData(i);
    fd = capR.filteredData(i);
    pressure = bv - fd - Threshold + 20;
    leds[i + 8] = pressure > 20 ? 0xFF00FF : 0xFF8200;
    SerialDevice.print(CLAMP(pressure));
    SerialDevice.print("\t");
  }
  FastLED.show();
  SerialDevice.println(micros() - ReqTime);

  if (!digitalRead(RstBtn)) {
    MprSetup(capL);
    MprSetup(capR);
  }
}

void MprSetup(Adafruit_MPR121 cap) {//mpr121自定义初始化
  cap.writeRegister(MPR121_SOFTRESET, 0x63);//MprReset
  delay(1);
  cap.writeRegister(MPR121_ECR, 0x0);//MprStop
  cap.writeRegister(MPR121_MHDR, 1);// 上升最大变化值
  cap.writeRegister(MPR121_NHDR, 8);//上升幅度
  cap.writeRegister(MPR121_NCLR, 16);//上升修正样本个数
  cap.writeRegister(MPR121_FDLR, 0);//修正前等待样本个数
  cap.writeRegister(MPR121_MHDF, 1);//下降最大变化值
  cap.writeRegister(MPR121_NHDF, 1);//下降幅度
  cap.writeRegister(MPR121_NCLF, 8);//下降修正样本个数
  cap.writeRegister(MPR121_FDLF, 8);//修正前等待样本个数
  cap.writeRegister(MPR121_NHDT, 0);
  cap.writeRegister(MPR121_NCLT, 0);
  cap.writeRegister(MPR121_FDLT, 0);
  cap.setThresholds(Threshold, Threshold); //设置触发阈值和充放电流时间
  cap.writeRegister(MPR121_DEBOUNCE, (4 << 4) | 2); //设置采样数,0
  cap.writeRegister(MPR121_CONFIG1, 16);//0x10
  cap.writeRegister(MPR121_CONFIG2, 1 << 5);
  cap.writeRegister(MPR121_AUTOCONFIG0, 0x0B);
  cap.writeRegister(MPR121_AUTOCONFIG1, (1 << 7));
  cap.writeRegister(MPR121_UPLIMIT, 202);//上限，((Vdd - 0.7)/Vdd) * 256
  cap.writeRegister(MPR121_TARGETLIMIT, 182);//目标，UPLIMIT * 0.9
  cap.writeRegister(MPR121_LOWLIMIT, 131);//下限，UPLIMIT * 0.65
  cap.writeRegister(MPR121_ECR, B10000000 + 8);//MprRun
}
