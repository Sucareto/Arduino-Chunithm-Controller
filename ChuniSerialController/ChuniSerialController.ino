#if defined(__AVR_ATmega32U4__)
#pragma message "当前的开发板是 ATmega32U4"
#define SerialDevice SerialUSB
#define Enable_LED
#define LED_PIN 6
#define Enable_HID
uint8_t LastAir, LastBtn;

#elif defined(ARDUINO_SAMD_ZERO)
#pragma message "当前的开发板是 SAMD_ZERO"
#define SerialDevice SerialUSB
#define Enable_LED
#define LED_PIN 5
#define Enable_HID
uint8_t airs[6] = {A1, A2, A3, A4, 8, 9}; //AIR针脚
uint8_t btns[4] = {10, 12, 6, 7};
uint8_t LastAir[6], LastBtn[4];

#elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
#pragma message "当前的开发板是 NODEMCU_ESP12E"
#define SerialDevice Serial
#define Enable_LED
#define LED_PIN D5

#else
#error "未经测试的开发板，请检查串口和阵脚定义"
#endif

#ifdef Enable_LED
#include "FastLED.h"
#define LED_NUM 16 //LED数量
CRGB leds[LED_NUM];
#endif

#include "Adafruit_MPR121.h"
Adafruit_MPR121 capL, capR;
#define Threshold 10 //触摸触发阈值
#include "slider_cmd.h"


void setup() {
  SerialDevice.begin(115200);
  SerialDevice.setTimeout(0);
#ifdef Enable_LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_NUM);
  FastLED.setBrightness(50);//设置亮度
  FastLED.clear();
#endif

#ifdef Enable_HID
  KeySetup();
#endif

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

void loop() {
  switch (slider_read()) {
    case SLIDER_CMD_SET_LED:
      slider_res_init();
      slider_set_led();
      break;
    case SLIDER_CMD_AUTO_SCAN_START:
      auto_scan = true;
      slider_res_init();
      slider_scan();
      break;
    case SLIDER_CMD_AUTO_SCAN_STOP:
      auto_scan = false;
      slider_res_init();
      break;
    case SLIDER_CMD_RESET:
      slider_reset();
      break;
    case SLIDER_CMD_GET_BOARD_INFO:
      slider_get_board_info();
      break;
    default:
      slider_scan();
  }
  slider_write();

#ifdef Enable_HID
  KeyCheck();
#endif
}
