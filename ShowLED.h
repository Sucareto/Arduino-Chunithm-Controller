#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 6

CRGB leds[NUM_LEDS], lastleds[NUM_LEDS];

uint8_t data[93];
uint8_t lastdata[93];

typedef struct {
  uint8_t b;
  uint8_t r;
  uint8_t g;
} RGBLed;

union {
  RGBLed rgb[31];
  uint8_t raw[93];
} ledData;

enum
{
  Start,
  Write,
  Load,
  Error,
  Debug,
};

void showLED(uint8_t code) {
  switch (code) {
    case Start://启动，顺序橘色
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(255, 130, 0);
        FastLED.show();
        delay(20);
      }
      break;
    case Error://错误，全部红色
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(255, 0, 0);
      }
      FastLED.show();
      delay(200);
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
      delay(200);
      break;
    case Write://写入，中间散开绿色
      for (uint8_t i = (NUM_LEDS / 2) - 1; i > -1; i--) {
        leds[i] = CRGB(0, 255, 0);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 255, 0);
        FastLED.show();
        delay(20);
      }
      break;
    case Load://加载，两侧进入蓝色
      for (uint8_t i = 0; i < NUM_LEDS / 2; i++) {
        leds[i] = CRGB(0, 0, 255);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 0, 255);
        FastLED.show();
        delay(20);
      }
      break;
    case Debug://调试用，间隔红蓝
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        ++i;
        leds[i] = CRGB(0, 0, 255);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 0, 255);
        FastLED.show();
        delay(20);
      }
      for (uint8_t i = (NUM_LEDS - 1); i > -1; i--) {
        --i;
        leds[i] = CRGB(255, 0, 0);
        leds[NUM_LEDS - 1 - i] = CRGB(255, 0, 0);
        FastLED.show();
        delay(20);
      }
      break;
  }
}

uint8_t ReadLedData(byte *buf) {//传入读到数据的对象指针
  uint8_t refresh = false;

  for (int i = 0; i < 100; i++) {
    if (buf[i] == 0xAA && buf[i + 1] == 0xAA) {//寻找数据头部
      memcpy(data, buf + i + 2, 93);//复制数据
      if (memcmp(data, lastdata, 93)) {//对比数据是否改变
        memcpy(ledData.raw, data, 93);
        refresh = true;
      }
      memcpy(lastdata, data, 93);
      return refresh;
    }
  }
  return refresh;
}

RGBLed getKey(uint8_t key) {
  return ledData.rgb[30 - (key * 2)];
}
