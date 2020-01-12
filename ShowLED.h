#include <FastLED.h>
#define Start 0
#define Write 1
#define Load 2
#define Error 3
#define Setting 4
#define NUM_LEDS 16
#define DATA_PIN 6

CRGB leds[NUM_LEDS];//GRB

void showLED(char code) {
  switch (code) {
    case Start://启动，顺序橘色
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(130, 255, 0);
        FastLED.show();
        delay(20);
      }
      break;
    case Error://错误，全部红色
      for (char i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 255, 0);
      }
      FastLED.show();
      delay(200);
      for (char i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(0, 0, 0);
      }
      FastLED.show();
      delay(200);
      break;
    case Write://写入，中间散开绿色
      for (int i = (NUM_LEDS / 2) - 1; i > -1; i--) {
        leds[i] = CRGB(255, 0, 0);
        leds[NUM_LEDS - 1 - i] = CRGB(255, 0, 0);
        FastLED.show();
        delay(20);
      }
      break;
    case Load://加载，两侧进入蓝色
      for (int i = 0; i < NUM_LEDS / 2; i++) {
        leds[i] = CRGB(0, 0, 255);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 0, 255);
        FastLED.show();
        delay(20);
      }
      break;
    case Setting://设置,间隔红蓝
      for (int i = 0; i < NUM_LEDS; i++) {
        ++i;
        leds[i] = CRGB(0, 0, 255);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 0, 255);
        FastLED.show();
        delay(20);
      }
      for (int i = (NUM_LEDS - 1); i > -1; i--) {
        --i;
        leds[i] = CRGB(0, 0, 255);
        leds[NUM_LEDS - 1 - i] = CRGB(0, 255, 0);
        FastLED.show();
        delay(20);
      }
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB(130, 255, 0);
        FastLED.show();
        delay(20);
      }
      break;
  }
}
