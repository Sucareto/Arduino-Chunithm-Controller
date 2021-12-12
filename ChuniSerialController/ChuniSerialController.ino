#if defined(__AVR_ATmega32U4__)
#pragma message "当前的开发板是 ATmega32U4"
#define SerialDevice SerialUSB
#define LED_PIN 6
#define Enable_LED
#define Enable_HID
uint8_t LastAir, LastBtn;

#elif defined(ARDUINO_SAMD_ZERO)
#pragma message "当前的开发板是 SAMD_ZERO"
#define SerialDevice SerialUSB
#define LED_PIN 5
#define Enable_LED
#define Enable_HID
uint8_t airs[6] = {A1, A2, A3, A4, 8, 9}; //AIR针脚
uint8_t btns[4] = {10, 12, 6, 7};
uint8_t LastAir[6], LastBtn[4];

#elif defined(ARDUINO_ESP8266_NODEMCU_ESP12E)
#pragma message "当前的开发板是 NODEMCU_ESP12E"
#define SerialDevice Serial
#define LED_PIN D5
#define Enable_LED

#else
#error "未经测试的开发板，请检查串口和阵脚定义"
#endif

#ifdef Enable_LED
#include "FastLED.h"
#define LED_NUM 16
CRGB leds[LED_NUM];
#endif

#ifdef Enable_HID
#include "Keyboard.h"
uint8_t KeyCode[10] = {//键值列表
  //  KEY_SLASH, KEY_PERIOD, KEY_QUOTE, KEY_SEMICOLON, KEY_RIGHT_BRACE, KEY_LEFT_BRACE,
  '/', '.', '\'', ';', ']', '[',
  KEY_F1, KEY_F2, KEY_F3, KEY_RETURN,
};
#endif

#include "cmd.h"
void setup() {
  SerialDevice.begin(115200);
  Serial.setTimeout(0);
#ifdef Enable_LED
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_NUM);
#endif
#ifdef Enable_HID
  KeySetup();
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

}
