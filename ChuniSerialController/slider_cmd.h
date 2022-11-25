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
        uint8_t leds[96];//触摸点和分隔的灯光数据，从右到左，BRG
      };
      char version[32];
      uint8_t pressure[32];//从右到左的触摸数据
    };
  };
  uint8_t data[128];
} slider_packet_t;

static slider_packet_t req, res;
static uint8_t auto_scan, req_len = 0, req_r, req_sum = 0, req_esc, slider_tx_pending = 0;

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
  if (res.cmd) {
    slider_tx_pending = res.size + 4;
    ptr = 0;
    checksum = 0;
  } else {
    return;
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
      SerialDevice.write(0xFD);
      w--;
    }
    SerialDevice.write(w);
    ptr++;
    slider_tx_pending--;
  }
  res.cmd = 0;
}

static void slider_res_init() {//通用回复
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
  MprSetup(capL);
  MprSetup(capR);
}

static void slider_get_board_info() {//返回版本信息
  res.sync = 0xFF;
  res.cmd = SLIDER_CMD_GET_BOARD_INFO;
  res.size = sizeof(res.version);
  strcpy(res.version, "15330   \xA0""06712\xFF""\x90");
  req.cmd = 0;
}

#define CLAMP(val) (val < 0 ? 0 : (val > 255 ? 255 : val))
static void slider_scan() {//触摸扫描
  if (!auto_scan) {
    return;
  }
  res.sync = 0xFF;
  res.cmd = SLIDER_CMD_AUTO_SCAN;
  res.size = sizeof(res.pressure);
  memset(res.pressure, 0, sizeof(res.pressure));
  int16_t bv, fd, pressure;
  for (int i = 0; i < 8; i++) {
    bv = capL.baselineData(i);
    fd = capL.filteredData(i);
    pressure = bv - fd - Threshold + 20;
    res.pressure[31 - (i * 2)] = CLAMP(pressure);
    //res.pressure[31 - (i * 2 + 1)] = 0;
    bv = capR.baselineData(i);
    fd = capR.filteredData(i);
    pressure = bv - fd - Threshold + 20;
    res.pressure[31 - (16 + i * 2)] = CLAMP(pressure);
    //res.pressure[31 - (16 + i * 2 + 1)] = 0;
  }
}

static void slider_set_led() {//串口读取led数据
  uint8_t t1, t2;
  for (int i = 0; i < 16; i++) {//BRG
    t1 = 15 - i;
    t2 = i * 6;
    leds[t1].r = req.leds[t2 + 1];
    leds[t1].g = req.leds[t2 + 2];
    leds[t1].b = req.leds[t2 + 0];
  }
  FastLED.show();
  req.cmd = 0;
}

#ifdef Enable_HID

#include "Keyboard.h"
uint8_t KeyCode[10] = {//键值列表
  '/', '.', '\'', ';', ']', '[',
  KEY_F1, KEY_F2, KEY_F3, KEY_RETURN,
};

void KeySetup() {//按钮和AIR初始化
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

void KeyCheck() {//按钮和AIR检查
#if defined(__AVR_ATmega32U4__)
  uint8_t NowAir = (PINB & B01111110) >> 1;
  if (NowAir != LastAir) {
    for (uint8_t i = 0; i < 6; i++) {
      NowAir & _BV(i) ? Keyboard.release(KeyCode[i]) : Keyboard.press(KeyCode[i]);
    }
    LastAir = NowAir;
  }

  uint8_t NowBtn = (PINF & B11110000) >> 4;
  if (NowBtn != LastBtn) {
    for (uint8_t i = 0; i < 3; i++) {
      NowBtn & _BV(i) ? Keyboard.release(KeyCode[i + 6]) : Keyboard.press(KeyCode[i + 6]);
    }
    if (NowBtn & _BV(3)) {//刷卡键，以及重置触摸
      Keyboard.release(KeyCode[9]);
    } else {
      Keyboard.press(KeyCode[9]);
      MprSetup(capL);
      MprSetup(capR);
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
  for (uint8_t i = 0; i < 3; i++) {
    uint8_t NowBtn = !digitalRead(btns[i]);
    if (NowBtn != LastBtn[i]) {
      NowBtn ? Keyboard.press(KeyCode[i + 6]) : Keyboard.release(KeyCode[i + 6]);
      LastBtn[i] = NowBtn;
    }
  }
  uint8_t NowBtn = !digitalRead(btns[3]);
  if (NowBtn != LastBtn[3]) {//刷卡键，以及重置触摸
    Keyboard.press(KeyCode[9]);
    MprSetup(capL);
    MprSetup(capR);
  } else {
    Keyboard.release(KeyCode[9]);
  }
#endif
}

#endif
