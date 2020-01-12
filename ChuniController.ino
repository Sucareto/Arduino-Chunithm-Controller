#include "Adafruit_MPR121.h"
#include "HID-Project.h"
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif

Adafruit_MPR121 cap1, cap2 = Adafruit_MPR121();

int lasttouchedA, currtouchedA, lasttouchedB, currtouchedB  = 0;
int lastStatus[7] = {0};

char KeyCodeA[8] = {'r', '4', 'e', '3', 'w', '2', 'q', '1',};
char KeyCodeB[8] = {'i', '8', 'u',  '7', 'y', '6', 't', '5',};

typedef struct {
  char pin;
  uint8_t keycode;
} LIST;
LIST SWs[] =
{
  {21, ']'}, {20, ';'}, {15, '\''}, {14, '.'}, {16, '/'}, {9, KEY_F1}, {10, KEY_F2},
};

void setup() {
  if (cap1.begin(0x5A) && cap2.begin(0x5B) ) {
    for (int i = 0; i < 7; i++) {
      pinMode(SWs[i].pin, INPUT_PULLUP);
    }
    NKROKeyboard.begin();
  } else {
    Serial.begin(9600);
    while (!Serial);
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
}

void loop() {
  currtouchedA = cap1.touched();
  currtouchedB = cap2.touched();

  for (int i = 0; i < 8; i++) {
    if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) ) {
      NKROKeyboard.press(KeyCodeA[i]);
    } else if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) ) {
      NKROKeyboard.release(KeyCodeA[i]);
    }
    if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) ) {
      NKROKeyboard.press(KeyCodeB[i]);
    } else if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) ) {
      NKROKeyboard.release(KeyCodeB[i]);
    }
  }
  for (char i = 0; i < 7; i++) {
    boolean Status = !digitalRead(SWs[i].pin);
    if (Status != lastStatus[i] && Status) {
      NKROKeyboard.press(SWs[i].keycode);
    } else if (Status != lastStatus[i] && !Status) {
      NKROKeyboard.release(SWs[i].keycode);
    }
    lastStatus[i] = Status;
  }
  lasttouchedA = currtouchedA;
  lasttouchedB = currtouchedB;
}
