#include <FastLED.h>

#define NUM_LEDS 16
#define DATA_PIN 6

CRGB leds[NUM_LEDS], Tleds[NUM_LEDS], Sleds[NUM_LEDS], lastleds[NUM_LEDS];

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

enum {
  Start,
  Write,
  Load,
  Error,
};

#define purple 0xFF00FF
#define orange 0xFF8200

void showLED(uint8_t code) {
  switch (code) {
    case Start:  //启动，顺序橘色
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        leds[i] = orange;
        FastLED.show();
        delay(20);
      }
      FastLED.show();
      memcpy(Tleds, leds, 48);
      break;
    case Error:  //错误，全部红色
      LEDS.showColor(CRGB::Red);
      delay(200);
      LEDS.showColor(CRGB::Black);
      delay(200);
      break;
    case Write:  //写入，中间散开绿色
      for (uint8_t i = (NUM_LEDS / 2) - 1; i > -1; i--) {
        leds[i] = CRGB::Green;
        leds[NUM_LEDS - 1 - i] = CRGB::Green;
        FastLED.show();
        delay(20);
      }
      break;
    case Load:  //加载，两侧进入蓝色
      for (uint8_t i = 0; i < NUM_LEDS / 2; i++) {
        leds[i] = CRGB::Blue;
        leds[NUM_LEDS - 1 - i] = CRGB::Blue;
        FastLED.show();
        delay(20);
      }
      break;
  }
}

uint8_t ReadLedData(byte *buf) {//从串口数据中读取灯效指令
  uint8_t refresh = false;
  for (int i = 0; i < 100; i++) {
    if (buf[i] == 0xAA && buf[i + 1] == 0xAA) {
      memcpy(data, buf + i + 2, 93);
      if (memcmp(data, lastdata, 93)) {
        memcpy(ledData.raw, data, 93);
        refresh = true;
      }
      memcpy(lastdata, data, 93);
      return refresh;
    }
  }
  return refresh;
}

RGBLed getKey(uint8_t key) {//仅返回 16 key 灯效
  return ledData.rgb[30 - (key * 2)];
}

void getLED(CRGB *leds) {
  for (uint8_t i = 0; i < 16; i++) {
    RGBLed led = ledData.rgb[30 - (i * 2)];
    leds[i].setRGB(led.r, led.g, led.b);
  }
}
