#define COMLed //开启串口灯效

#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

#include "Adafruit_MPR121.h"//mpr121定义
Adafruit_MPR121 mprA, mprB;
int LastTA, CurrTA, LastTB, CurrTB, LastA, CurrA, LastB, CurrB;

#include "HID-Project.h"//键盘定义
#define Keys 26
KeyboardKeycode KeyCode[Keys] = {
  KEY_1, KEY_Q, KEY_2, KEY_W, KEY_3, KEY_E, KEY_4, KEY_R,
  KEY_5, KEY_T, KEY_6,  KEY_Y, KEY_7, KEY_U, KEY_8, KEY_I,
  KEY_SLASH, KEY_PERIOD, KEY_QUOTE, KEY_SEMICOLON, KEY_RIGHT_BRACE, KEY_LEFT_BRACE,
  KEY_F1, KEY_F2, KEY_F3, KEY_ENTER,
};

#include <FastLED.h>//灯效定义
#define NUM_LEDS 16
#define DATA_PIN 6
#define purple 0xFF00FF
#define orange 0xFF8200
CRGB leds[NUM_LEDS], Tleds[NUM_LEDS], Sleds[NUM_LEDS], lastleds[NUM_LEDS];

unsigned long lastime = 0;


void setup() {//初始化函数
  LedSetup();
#ifdef COMLed//串口灯效
  Serial.begin(115200);
  Serial.setTimeout(0);
#endif

  if (TouchSetup()) {
    AirSetup();
    BtnSetup();
    NKROKeyboard.begin();//正式开始
    fill_solid(leds, NUM_LEDS, orange);
    FastLED.show();
    memcpy(Tleds, leds, 48);

  } else {//错误
    while (!Serial) {
      LEDS.showColor(CRGB::Red);
      delay(200);
      FastLED.clear();
      delay(200);
    }
    Serial.println("MPR121 not found.");
    while (1);
  }
}

void loop() {
  MprCheck();

#ifdef COMLed//串口灯效
  GetLed();
#endif

  SetLed();
  AirCheck();
  BtnCheck();
}


void LedSetup() {//LED初始化
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(90);//设置 LED 亮度
  for (uint8_t i = 0; i < NUM_LEDS / 2; i++) {
    leds[i] = CRGB::Blue;
    leds[NUM_LEDS - 1 - i] = CRGB::Blue;
    FastLED.show();
    delay(30);
  }
}

bool TouchSetup() {//触摸初始化
  uint8_t TOUCH = 2;//按下敏感度
  uint8_t RELEASE = 1;//松开敏感度
  uint8_t CONFIG1 = 0x35;//电流
  uint8_t CONFIG2 = 0x02;//延迟
  if (mprA.begin(0x5A, &Wire, TOUCH, RELEASE) && mprB.begin(0x5C, &Wire, TOUCH, RELEASE)) {
    mprA.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprA.writeRegister(MPR121_CONFIG2, CONFIG2);
    mprB.writeRegister(MPR121_CONFIG1, CONFIG1);
    mprB.writeRegister(MPR121_CONFIG2, CONFIG2);
    return true;
  } else {
    return false;
  }
}

void AirSetup() {//AIR初始化
  DDRB  &= B11000001;
  PORTB |= B00111110;
}

void BtnSetup() {//按钮初始化
  DDRF  &= B00001111;
  PORTF |= B11110000;
}

void MprCheck() {
  CurrTA = mprA.touched();//mprA 检测部分
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

  CurrTB = mprB.touched();//mprB 检测部分
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
}
void AirCheck() {//Air 检测部分
  CurrA = PINB;
  for (int i = 1; i < 6; i++) {
    if ((CurrA & _BV(i)) && !(LastA & _BV(i))) {
      NKROKeyboard.press(KeyCode[i + 15]);
    } else if (!(CurrA & _BV(i)) && (LastA & _BV(i))) {
      NKROKeyboard.release(KeyCode[i + 15]);
    }
  }
  LastA = CurrA;
}

void BtnCheck() {//按钮检测部分
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

void GetLed() {//获取串口灯效
  uint8_t LedData[104];
  if (Serial.available() && Serial.readBytes(LedData, 104)) {
    lastime = millis();
    for (int i = 0; i < 52; i++) {
      if (LedData[i] == 0xAA && LedData[i + 1] == 0xAA) {
        memcpy(Sleds, LedData + i + 2, 48);
        return;
      }
    }
  }
}

void SetLed() {//设置灯效
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
}
