
/*
 * Have a six buttons peripheral.
 * @author https://github.com/ddmendes
 */

#include <Keyboard.h>

#define RELEASE 0x02
#define PRESS 0x01
#define NOOP 0x00

#define ROWS 2
#define COLS 3

#define EMERGENCY_RELEASE_PIN 6

struct Keebe {
  const unsigned short int rowLength = ROWS;
  const unsigned short int colLength = COLS;
  const byte rowPin[ROWS] = {8, 7};
  const byte colPin[COLS] = {15, 14, 16};
  const byte key[ROWS][COLS] = {
    {'c', KEY_LEFT_SHIFT, '1'},
    {'v', KEY_LEFT_CTRL, '2'}
  };
} kb;

struct KState {
  byte action[ROWS][COLS];
  byte logicalValue[ROWS][COLS];
} kstate;

void pinModeArray(const byte* pins, short int pinsLength, byte mode);
void digitalWriteArray(const byte* pins, short int pinsLength, byte value);
void setupKState(Keebe kb, KState &kstate);
void readKeyboard(Keebe kb, KState &kstate);
byte calculateAction(const byte now, const byte before);
void applyAction(Keebe kb, KState kstate);

void setup() {
  pinMode(EMERGENCY_RELEASE_PIN, INPUT_PULLUP);
  pinModeArray(kb.rowPin, kb.rowLength, OUTPUT);
  pinModeArray(kb.colPin, kb.colLength, INPUT_PULLUP);
  digitalWriteArray(kb.rowPin, kb.rowLength, HIGH);
  setupKState(kb, kstate);
  Keyboard.begin();
}

void loop() {
  readKeyboard(kb, kstate);
  applyAction(kb, kstate);
  byte emergency = digitalRead(EMERGENCY_RELEASE_PIN);
  if (emergency == LOW) {
    Keyboard.releaseAll();
  }
}

void pinModeArray(const byte* pins, short int pinsLength, byte mode) {
  for (int i = 0; i < pinsLength; i++) {
    pinMode(pins[i], mode);
  }
}

void digitalWriteArray(const byte* pins, short int pinsLength, byte value) {
  for (int i = 0; i < pinsLength; i++) {
    digitalWrite(pins[i], value);
  }
}

void setupKState(Keebe kb, KState &kstate) {
  for (int i = 0; i < kb.rowLength; i++) {
    for (int j = 0; j < kb.colLength; j++) {
      kstate.logicalValue[i][j] = HIGH;
      kstate.action[i][j] = NOOP;
    }
  }
}

void readKeyboard(Keebe kb, KState &kstate) {
  for (int i = 0; i < kb.rowLength; i++) {
    digitalWrite(kb.rowPin[i], LOW);
    for (int j = 0; j < kb.colLength; j++) {
      byte colValue = (byte) digitalRead(kb.colPin[j]);
      kstate.action[i][j] = calculateAction(colValue, kstate.logicalValue[i][j]);
      kstate.logicalValue[i][j] = colValue;
    }
    digitalWrite(kb.rowPin[i], HIGH);
  }
}

byte calculateAction(const byte now, const byte before) {
  if (now == before) {
    return NOOP;
  } else if (now == LOW && before == HIGH) {
    return PRESS;
  } else if (now == HIGH && before == LOW) {
    return RELEASE;
  }
}

void applyAction(Keebe kb, KState kstate) {
  for (int i = 0; i < kb.rowLength; i++) {
    for (int j = 0; j < kb.colLength; j++) {
      switch (kstate.action[i][j]) {
        case PRESS:
          Keyboard.press(kb.key[i][j]);
          break;
        case RELEASE:
          Keyboard.release(kb.key[i][j]);
          break;
      }
    }
  }
}
