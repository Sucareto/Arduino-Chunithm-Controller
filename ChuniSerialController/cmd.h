enum {
  SLIDER_CMD_AUTO_SCAN = 0x01,
  SLIDER_CMD_SET_LED = 0x02,
  SLIDER_CMD_AUTO_SCAN_START = 0x03,
  SLIDER_CMD_AUTO_SCAN_STOP = 0x04,
  SLIDER_CMD_DIVA_UNK_09 = 0x09,
  SLIDER_CMD_DIVA_UNK_0A = 0x0A,
  SLIDER_CMD_RESET = 0x10,
  SLIDER_CMD_GET_BOARD_INFO = 0xF0
};

typedef union slider_packet {
  struct {
    uint8_t sync;
    uint8_t cmd;
    uint8_t size;
    union {
      struct {
        uint8_t led_unk;
        uint8_t leds[96];
      };
      char version[32];
      uint8_t pressure[32];
    };
  };
  uint8_t data[128];
} slider_packet_t;

extern int slider_tx_pending;
static slider_packet_t req, res;
int slider_tx_pending = 0;
static uint8_t auto_scan;

static uint8_t req_len, req_r, req_sum, req_esc;
uint8_t slider_read() {
  while (SerialDevice.available()) {
    req_r = SerialDevice.read();
    if (req_r == 0xFF) {
      req_len = 0;
      req.sync = req_r;
      req.cmd = 0;
      req.size = req_r;
      req_sum = req_r;
      req_esc = false;
      continue;
    }
    if (req_r == 0xFD) {
      req_esc = true;
      continue;
    }
    if (req_esc) {
      req_esc = false;
      req_r++;
    }
    req_sum += req_r;
    if (!req.cmd) {
      req.cmd = req_r;
      continue;
    }
    if (req.size == 0xFF) {
      req.size = req_r;
      req_len = 3;
      continue;
    }
    if (req_len >= 128) {
      continue;
    }
    req.data[req_len++] = req_r;
    if (req_len == req.size + 4) {
      if (!req_sum) {
        return req.cmd;
      }
    }
  }
  return 0;
}

void slider_write() {
  static short ptr = 0;
  static uint8_t checksum;
  if (!slider_tx_pending) {
    if (res.cmd) {
      slider_tx_pending = res.size + 4;
      ptr = 0;
      checksum = 0;
    } else {
      return;
    }
  }
  while (slider_tx_pending) {
    uint8_t w;
    if (slider_tx_pending == 1) {
      w = -checksum;
    } else {
      w = res.data[ptr];
    }
    checksum += w;
    if ((ptr != 0 && w == 0xFF) || w == 0xFD) {
      if (SerialDevice.availableForWrite() < 2)
        return;
      SerialDevice.write(0xFD);
      w--;
    } else {
      if (SerialDevice.availableForWrite() < 1)
        return;
    }
    SerialDevice.write(w);
    ptr++;
    slider_tx_pending--;
  }
  res.cmd = 0;
}

static void slider_res_init() {//通用
  res.sync = 0xFF;
  res.cmd = req.cmd;
  res.size = 0;
  req.cmd = 0;
}

static void slider_reset() {//重置slider，重新初始化触摸
  res.sync = 0xFF;
  res.cmd = SLIDER_CMD_RESET;
  res.size = 0;
  req.cmd = 0;
}
static void slider_get_board_info() {//返回版本信息
  res.sync = 0xFF;
  res.cmd = SLIDER_CMD_GET_BOARD_INFO;
  res.size = sizeof(res.version);
  strcpy(res.version, "15330   \xA0""06712\xFF""\x90");
  req.cmd = 0;
}

static void slider_scan() {//触摸扫描
  if (!auto_scan || slider_tx_pending) {
    return;
  }
  res.sync = 0xFF;
  res.cmd = SLIDER_CMD_AUTO_SCAN;
  res.size = sizeof(res.pressure);
  memset(res.pressure, 0, sizeof(res.pressure));
}

static void slider_set_led() {//串口读取led数据
  if (slider_tx_pending) {
    return;
  }
}

#ifdef Enable_HID
void KeySetup() {
  Keyboard.begin();
#if defined(__AVR_ATmega32U4__)
  DDRB  &= B10000001;
  PORTB |= B01111110;
  DDRF  &= B00001111;
  PORTF |= B11110000;
#elif defined(ARDUINO_SAMD_ZERO)
  for (uint8_t i : airs) {
    pinMode(i, INPUT_PULLUP);
  }
  for (uint8_t i : btns) {
    pinMode(i, INPUT_PULLUP);
  }
#endif
}

void KeyCheck() {
#if defined(__AVR_ATmega32U4__)
  uint8_t NowAir = (PINB & B01111110) >> 1;
  if (NowAir != LastAir) {
    for (uint8_t i = 0; i < 6; i++) {
      NowAir & _BV(i) ? Keyboard.press(KeyCode[i]) : Keyboard.release(KeyCode[i]);
    }
    LastAir = NowAir;
  }

  uint8_t NowBtn = PINF & B11110000 >> 4;
  if (NowBtn != LastBtn) {
    for (uint8_t i = 0; i < 4; i++) {
      NowBtn & _BV(i) ? Keyboard.press(KeyCode[i + 6]) : Keyboard.release(KeyCode[i + 6]);
    }
    LastBtn = NowBtn;
  }

#elif defined(ARDUINO_SAMD_ZERO)
  for (uint8_t i = 0; i < 6; i++) {
    uint8_t NowAir = !digitalRead(airs[i]);
    if (NowAir != LastAir[i]) {
      NowAir ? Keyboard.press(KeyCode[i]) : Keyboard.release(KeyCode[i]);
      LastAir[i] = NowAir;
    }
  }
  for (uint8_t i = 0; i < 4; i++) {
    uint8_t NowBtn = !digitalRead(btns[i]);
    if (NowBtn != LastBtn[i]) {
      NowBtn ? Keyboard.press(KeyCode[i + 6]) : Keyboard.release(KeyCode[i + 6]);
      LastBtn[i] = NowBtn;
    }
  }
#endif
}
#endif
