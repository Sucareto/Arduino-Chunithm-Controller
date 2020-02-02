#define SaveCfg //开启保存配置
#define SerialLed //开启串口灯

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#include "HID-Project.h"

#ifdef SaveCfg
#include "EEPROM.h"
#endif

#include "ShowLED.h"

#include "Adafruit_MPR121.h"
Adafruit_MPR121 mprA, mprB = Adafruit_MPR121();
uint8_t TOUCH = 3;//按下敏感度
uint8_t RELEASE = 1;//松开敏感度
uint8_t CONFIG1 = 53;//电流
uint8_t CONFIG2 = 2;//延迟
int lasttouchedA, currtouchedA, lasttouchedB, currtouchedB  = 0;

#define Keys 22
uint8_t KeyCode[Keys] = {
  '1', 'q', '2', 'w', '3', 'e', '4', 'r',
  '5', 't', '6',  'y', '7', 'u', '8', 'i',
  '/', '.', '\'', ';', ']', '[',
};
#define bts 6
uint8_t btn[bts] = {16, 14, 15, 20, 21, 9};
uint8_t lastStatus[bts] = {0};

void setup() {
  //定义灯条型号，数据 PIN，数据格式
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(115200);
  Serial.setTimeout(0);
  if (mprA.begin(0x5A, &Wire, TOUCH, RELEASE) && mprB.begin(0x5C, &Wire, TOUCH, RELEASE)) {
    mprA.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprA.writeRegister(MPR121_CONFIG2, CONFIG2);
    mprB.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprB.writeRegister(MPR121_CONFIG2, CONFIG2);
#ifdef SaveCfg
    for (uint8_t i = 0; i < Keys; i++) {
      uint8_t data = EEPROM.read(i);
      if (data == 255 || data == 0) {
        EEPROM.update(i, KeyCode[i]);
        showLED(Write);
      } else {
        KeyCode[i] = data;
        showLED(Load);
      }
    }
#endif
    for (uint8_t i = 0; i < bts; i++) {
      pinMode(btn[i], INPUT_PULLUP);
    }
    NKROKeyboard.begin();
    showLED(Start);
  } else {
    while (!Serial) {
      showLED(Error);
    }
    Serial.println("MPR121 not found.");
    while (1);
  }
}

void loop() {
  //mpr121A 处理
  currtouchedA = mprA.touched();
  if (currtouchedA != lasttouchedA) {
    for (uint8_t i = 0; i < 8; i++) {
      if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) ) {
        NKROKeyboard.press(KeyCode[i]);
#ifndef SerialLed
        leds[i] = CRGB(255, 0, 255);
#endif
      } else if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) ) {
        NKROKeyboard.release(KeyCode[i]);
#ifndef SerialLed
        leds[i] = CRGB(255, 130, 0);
#endif
      }
    }
    lasttouchedA = currtouchedA;
  }
  //mpr121B 处理
  currtouchedB = mprB.touched();
  if (currtouchedB != lasttouchedB) {
    for (uint8_t i = 0; i < 8; i++) {
      if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) ) {
        NKROKeyboard.press(KeyCode[i + 8]);
#ifndef SerialLed
        leds[i + 8] = CRGB(255, 0, 255);
#endif
      } else if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) ) {
        NKROKeyboard.release(KeyCode[i + 8]);
#ifndef SerialLed
        leds[i + 8] = CRGB(255, 130, 0);
#endif
      }
    }
    lasttouchedB = currtouchedB;
  }
  //灯效处理
#ifdef SerialLed
  uint8_t updateLeds = false;
  uint8_t serialData[200];
  if (Serial.available() && Serial.readBytes(serialData, 200)) {
    updateLeds = ReadLedData(serialData);
  }
  if (updateLeds) {//刷新灯
    for (uint8_t i = 0; i < 8; i++) {
      RGBLed temp1 = getKey(i);
      leds[i].setRGB( temp1.g, temp1.r, temp1.b);
      RGBLed temp2 = getKey(i + 8);
      leds[i + 8].setRGB( temp2.g, temp2.r, temp2.b);
    }
  }
#endif
  if (memcmp(leds, lastleds, 48)) {
    memcpy(lastleds, leds, 48);
    FastLED.show();
  }

  //Air 处理
  for (uint8_t i = 0; i < bts; i++) {
    uint8_t Status = digitalRead(btn[i]);
    if (Status != lastStatus[i] && Status) {
      NKROKeyboard.press(KeyCode[i + 16]);
    } else if (Status != lastStatus[i] && !Status) {
      NKROKeyboard.release(KeyCode[i + 16]);
    }
    lastStatus[i] = Status;
  }
  //串口配置
#ifndef SerialLed
  ReadSerial();
#endif
}

void ReadSerial() {
  if (Serial.available()) {
    uint8_t flag = Serial.read();
    switch (flag) {
      case 'k'://如果头部是k，后接 Keys 个字符（不带换行符）
        if (Serial.available() == Keys ) {
          for (uint8_t i = 0; i < Keys; i++) {
            uint8_t data = Serial.read();
            KeyCode[i] = data;
#ifdef SaveCfg
            EEPROM.update(i, data);
#endif
          }
          showLED(Write);
          Serial.println("KeyCode updated successfully!");
          showLED(Start);
        }
        break;
    }
  }
}
