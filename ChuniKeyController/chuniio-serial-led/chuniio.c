#include <windows.h>

#include <process.h>
#include <stdbool.h>
#include <stdint.h>

#include "chuniio/chuniio.h"
#include "chuniio/config.h"

// #include "util/dprintf.h"

static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx);

static bool chuni_io_coin;
static uint16_t chuni_io_coins;
// static uint8_t chuni_io_hand_pos;
static HANDLE chuni_io_slider_thread;
static bool chuni_io_slider_stop_flag;
static struct chuni_io_config chuni_io_cfg;

static const int chuni_io_default_irs[] = {
    VK_OEM_PERIOD, VK_OEM_2, VK_OEM_1, VK_OEM_7, VK_OEM_4, VK_OEM_6,
};
static HANDLE chuni_io_slider_led_port;

HRESULT chuni_io_jvs_init(void) {
  chuni_io_config_load(&chuni_io_cfg, L".\\segatools.ini");

  return S_OK;
}

void chuni_io_jvs_read_coin_counter(uint16_t *out) {
  if (out == NULL) {
    return;
  }

  if (GetAsyncKeyState(chuni_io_cfg.vk_coin)) {
    if (!chuni_io_coin) {
      chuni_io_coin = true;
      chuni_io_coins++;
    }
  } else {
    chuni_io_coin = false;
  }

  *out = chuni_io_coins;
}

void chuni_io_jvs_poll(uint8_t *opbtn, uint8_t *beams) {
  size_t i;

  if (GetAsyncKeyState(chuni_io_cfg.vk_test)) {
    *opbtn |= 0x01; /* Test */
  }

  if (GetAsyncKeyState(chuni_io_cfg.vk_service)) {
    *opbtn |= 0x02; /* Service */
  }

  for (i = 0; i < 6; i++) {
    if (GetAsyncKeyState(chuni_io_default_irs[i]) & 0x8000) {
      *beams |= (1 << i);
    } else {
      *beams &= ~(1 << i);
    }
  }
  // if (GetAsyncKeyState(chuni_io_cfg.vk_ir)) {
  //     if (chuni_io_hand_pos < 6) {
  //         chuni_io_hand_pos++;
  //     }
  // } else {
  //     if (chuni_io_hand_pos > 0) {
  //         chuni_io_hand_pos--;
  //     }
  // }

  // for (i = 0 ; i < 6 ; i++) {
  //     if (chuni_io_hand_pos > i) {
  //         *beams |= (1 << i);
  //     }
  // }
}

void chuni_io_jvs_set_coin_blocker(bool open) {}

HRESULT chuni_io_slider_init(void) { return S_OK; }

void chuni_io_slider_start(chuni_io_slider_callback_t callback) {
  if (chuni_io_slider_thread != NULL) {
    return;
  }

  chuni_io_slider_thread = (HANDLE)_beginthreadex(
      NULL, 0, chuni_io_slider_thread_proc, callback, 0, NULL);

  if (!chuni_io_cfg.led_port) {
    //初始化失败
    // dprintf("----------读取配置文件失败----------\n");
    return;
    // } else {
    // dprintf("----------读取配置文件成功----------\n");
    }
    char comname[8];
    snprintf(comname, sizeof(comname), "COM%d", chuni_io_cfg.led_port);
    chuni_io_slider_led_port = CreateFile(comname, GENERIC_READ | GENERIC_WRITE,
                                          0, NULL, OPEN_EXISTING, 0, NULL);

    // if (chuni_io_slider_led_port == INVALID_HANDLE_VALUE)
    // dprintf("----------串口 %s 打开失败----------\n", comname);
    // else
    // dprintf("----------串口 %s 打开成功----------\n", comname);

    DCB dcb_serial_params = {0};
    dcb_serial_params.DCBlength = sizeof(dcb_serial_params);
    // status = GetCommState(chuni_io_slider_led_port, &dcb_serial_params);

    dcb_serial_params.BaudRate = CBR_115200;  // Setting BaudRate = 115200
    dcb_serial_params.ByteSize = 8;           // Setting ByteSize = 8
    dcb_serial_params.StopBits = ONESTOPBIT;  // Setting StopBits = 1
    dcb_serial_params.Parity = NOPARITY;      // Setting Parity = None
    SetCommState(chuni_io_slider_led_port, &dcb_serial_params);

    COMMTIMEOUTS timeouts = {0};
    timeouts.ReadIntervalTimeout = 50;          // in milliseconds
    timeouts.ReadTotalTimeoutConstant = 50;     // in milliseconds
    timeouts.ReadTotalTimeoutMultiplier = 10;   // in milliseconds
    timeouts.WriteTotalTimeoutConstant = 50;    // in milliseconds
    timeouts.WriteTotalTimeoutMultiplier = 10;  // in milliseconds

    SetCommTimeouts(chuni_io_slider_led_port, &timeouts);
  }

  void chuni_io_slider_stop(void) {
    if (chuni_io_slider_thread == NULL) {
      return;
    }

    chuni_io_slider_stop_flag = true;

    WaitForSingleObject(chuni_io_slider_thread, INFINITE);
    CloseHandle(chuni_io_slider_thread);
    chuni_io_slider_thread = NULL;
    chuni_io_slider_stop_flag = false;
    CloseHandle(chuni_io_slider_led_port);
    // dprintf("----------串口已关闭----------\n");
  }

  void chuni_io_slider_set_leds(const uint8_t *rgb) {
    char led_buffer[52];
    DWORD bytes_to_write;     // No of bytes to write into the port
    DWORD bytes_written = 0;  // No of bytes written to the port
    bytes_to_write = sizeof(led_buffer);

    led_buffer[0] = 0xAA;
    led_buffer[1] = 0xAA;
    // Remap GBR to RGB
    for (int i = 0; i < 16; i++) {
      uint8_t led_c = i * 3;
      uint8_t rgb_c = (15 - i) * 6;
      led_buffer[led_c + 2] = rgb[rgb_c + 1];
      led_buffer[led_c + 3] = rgb[rgb_c + 2];
      led_buffer[led_c + 4] = rgb[rgb_c + 0];
    }
    led_buffer[50] = 0xDD;
    led_buffer[51] = 0xDD;
    // if(memcmp(led_buffer, last_led_buffer, 52)){
    WriteFile(chuni_io_slider_led_port,  // Handle to the Serial port
              led_buffer,                // Data to be written to the port
              bytes_to_write,            // No of bytes to write
              &bytes_written,            // Bytes written
              NULL);
  }

  static unsigned int __stdcall chuni_io_slider_thread_proc(void *ctx) {
    chuni_io_slider_callback_t callback;
    uint8_t pressure[32];
    size_t i;

    callback = ctx;

    while (!chuni_io_slider_stop_flag) {
      for (i = 0; i < _countof(pressure); i++) {
        if (GetAsyncKeyState(chuni_io_cfg.vk_cell[i]) & 0x8000) {
          pressure[i] = 128;
        } else {
          pressure[i] = 0;
        }
      }

      callback(pressure);
      Sleep(1);
    }

    return 0;
  }
