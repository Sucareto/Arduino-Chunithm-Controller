#include "Adafruit_MPR121.h"
#include "HID-Project.h"
#include <EEPROM.h>
#ifndef _BV
#define _BV(bit) (1 << (bit))
#endif
Adafruit_MPR121 mprA, mprB = Adafruit_MPR121();
int lasttouchedA, currtouchedA, lasttouchedB, currtouchedB  = 0;

#define Keys 22
int KeyCode[Keys] = {
  '1', 'q', '2', 'w', '3', 'e', '4', 'r',
  '5', 't', '6',  'y', '7', 'u', '8', 'i',
  '/', '.', '\'', ';', ']', '[',
};
#define bts 6
char btn[bts] = {16, 14, 15, 20, 21, 9,};
char lastStatus[bts] = {0};

void setup() {
  if (mprA.begin(0x5A) && mprB.begin(0x5C) ) {
    Serial.begin(9600);
    for (char i = 0; i < Keys; i++) {
      if (EEPROM.read(i) == 255 || EEPROM.read(i) == 0) {
        EEPROM.update(i, KeyCode[i]);
      } else {
        KeyCode[i] = EEPROM.read(i);
      }
    }
    for (char i = 0; i < bts; i++) {
      pinMode(btn[i], INPUT_PULLUP);
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
  currtouchedA = mprA.touched();
  currtouchedB = mprB.touched();
  for (char i = 0; i < 8; i++) {
    if ((currtouchedA & _BV(i)) && !(lasttouchedA & _BV(i)) ) {
      NKROKeyboard.press(KeyCode[i]);
    } else if (!(currtouchedA & _BV(i)) && (lasttouchedA & _BV(i)) ) {
      NKROKeyboard.release(KeyCode[i]);
    }
    if ((currtouchedB & _BV(i)) && !(lasttouchedB & _BV(i)) ) {
      NKROKeyboard.press(KeyCode[i + 8]);
    } else if (!(currtouchedB & _BV(i)) && (lasttouchedB & _BV(i)) ) {
      NKROKeyboard.release(KeyCode[i + 8]);
    }
  }
  lasttouchedA = currtouchedA;
  lasttouchedB = currtouchedB;
  for (char i = 0; i < bts; i++) {
    boolean Status = digitalRead(btn[i]);
    if (Status != lastStatus[i] && Status) {
      NKROKeyboard.press(KeyCode[i + 16]);
    } else if (Status != lastStatus[i] && !Status) {
      NKROKeyboard.release(KeyCode[i + 16]);
    }
    lastStatus[i] = Status;
  }

  if (Serial && Serial.available() == Keys ) {
    for (char i = 0; i < Keys; i++) {
      char data = Serial.read();
      if (data >= 0 && data <= 255) {
        KeyCode[i] = data;
        EEPROM.update(i, data);
      }
    }
    Serial.println("KeyCode updated successfully!");
    //    Serial.println(KeyCode);
  } else if (Serial.available()) {
    while (Serial.available()) {
      Serial.read();
    }
    Serial.println("Error,clear.");
  }
}
