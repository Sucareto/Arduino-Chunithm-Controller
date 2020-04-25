#define COMLed //开启串口灯效

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#include "ShowLED.h"

//传感器初始化部分
#include "Adafruit_MPR121.h"
Adafruit_MPR121 mprA, mprB;
uint8_t TOUCH = 2;//按下敏感度
uint8_t RELEASE = 1;//松开敏感度
uint8_t CONFIG1 = 0x39;//电流
uint8_t CONFIG2 = 1;//延迟
int LastTA, CurrTA, LastTB, CurrTB, LastA, CurrA, LastB, CurrB;

#include "HID-Project.h"
#define Keys 26
KeyboardKeycode KeyCode[Keys] = {
  KEY_1, KEY_Q, KEY_2, KEY_W, KEY_3, KEY_E, KEY_4, KEY_R,
  KEY_5, KEY_T, KEY_6,  KEY_Y, KEY_7, KEY_U, KEY_8, KEY_I,
  KEY_SLASH, KEY_PERIOD, KEY_QUOTE, KEY_SEMICOLON, KEY_RIGHT_BRACE, KEY_LEFT_BRACE,
  KEY_F1, KEY_F2, KEY_F3, KEY_ENTER,
};

unsigned long lastime = 0;

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(25);//设置 LED 亮度
  showLED(Load);
  SerialUSB.begin(115200);
  SerialUSB.setTimeout(0);

  if (mprA.begin(0x5A, &Wire, TOUCH, RELEASE) && mprB.begin(0x5C, &Wire, TOUCH, RELEASE)) {
    mprA.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprA.writeRegister(MPR121_CONFIG2, CONFIG2);
    mprB.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprB.writeRegister(MPR121_CONFIG2, CONFIG2);

    //Air和按钮初始化部分
    DDRB = B0000001;
    PORTB = B1111110;
    DDRF =  B00000000;
    PORTF = B11110000;

    //正式开始
    NKROKeyboard.begin();
    showLED(Start);

  } else {//错误
    while (!SerialUSB) {
      showLED(Error);
    }
    SerialUSB.println("MPR121 not found.");
    while (1);
  }
}

void loop() {
  //mprA 检测部分
  CurrTA = mprA.touched();
  if (CurrTA != LastTA) {
    for (uint8_t i = 0; i < 8; i++) {
      if ((CurrTA & _BV(i)) && !(LastTA & _BV(i))) {
        NKROKeyboard.press(KeyCode[i]);
        Tleds[i] = purple;
      } else if (!(CurrTA & _BV(i)) && (LastTA & _BV(i))) {
        NKROKeyboard.release(KeyCode[i]);
        Tleds[i] = orange;
      }
    }
    LastTA = CurrTA;
  }

  //mprB 检测部分
  CurrTB = mprB.touched();
  if (CurrTB != LastTB) {
    for (uint8_t i = 0; i < 8; i++) {
      if ((CurrTB & _BV(i)) && !(LastTB & _BV(i))) {
        NKROKeyboard.press(KeyCode[i + 8]);
        Tleds[i + 8] = purple;
      } else if (!(CurrTB & _BV(i)) && (LastTB & _BV(i))) {
        NKROKeyboard.release(KeyCode[i + 8]);
        Tleds[i + 8] = orange;
      }
    }
    LastTB = CurrTB;
  }

#ifdef COMLed//串口灯效
  //  uint8_t updateLeds = false;
  uint8_t LedData[200];
  if (SerialUSB.available() && SerialUSB.readBytes(LedData, 200)) {
    lastime = millis();
    if (ReadLedData(LedData)) {
      getLED(Sleds);
    }
  }
#endif

  //串口未使用时，恢复触发式灯效
  if ((millis() - lastime) < 5000) {
    memcpy(leds, Sleds, 48);
  } else {
    memcpy(leds, Tleds, 48);
  }
  if (memcmp(leds, lastleds, 48)) {
    memcpy(lastleds, leds, 48);
    FastLED.show();
  }

  Air 检测部分
  CurrA = PINB;
  for (int i = 1; i < 7; i++) {
    if ((CurrA & _BV(i)) && !(LastA & _BV(i))) {
      NKROKeyboard.press(KeyCode[i + 15]);
    } else if (!(CurrA & _BV(i)) && (LastA & _BV(i))) {
      NKROKeyboard.release(KeyCode[i + 15]);
    }
  }
  LastA = CurrA;

  //按钮检测部分
  CurrB = PINF;
  for (uint8_t i = 4; i < 8; i++) {
    if (!(CurrB & _BV(i)) && (LastB & _BV(i))) {
      NKROKeyboard.press(KeyCode[i + 18]);
    } else if ((CurrB & _BV(i)) && !(LastB & _BV(i))) {
      NKROKeyboard.release(KeyCode[i + 18]);
    }
  }
  LastB = CurrB;
}
