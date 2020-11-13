#include "MCP17Keypad.h"

MCP17Keypad::MCP17Keypad(Adafruit_MCP23017 *mcp, Keypad_t *kpd, uint16_t *state,
			 uint16_t debounceDelay, uint16_t repeatDelay) {
  _mcp = mcp; _kpd = kpd; _state = state;
  // Set the keypad state to avoid any weird behavior.
  *_state = 0; last_state = 0; awake = true;
  debounce_time = 0; debounce_delay = debounceDelay;
#if KPD_REPEAT
  repeat_time = 0; repeat_delay = repeatDelay;
#endif
}
void MCP17Keypad::begin() {
  // Row pins are standing by in an open drain output mode.
  openRowPins();
  // Column pins are waiting for an interrupt in a pull-up mode.
  for (char c=0; c < _kpd->cols; c++) {
    char pin = _kpd->colPinMap[c];
    _mcp->pinMode(pin, INPUT);
    _mcp->pullUp(pin, HIGH);
    _mcp->setupInterruptPin(pin, FALLING);
  }
}
void MCP17Keypad::begin(char intPin, void(*intCallback)(void)) {
  _int = digitalPinToInterrupt(intPin);
  // _callback = intCallback;
  pinMode(intPin, INPUT);
  begin();
  attachInterrupt(_int, intCallback, FALLING);
}
void MCP17Keypad::openRowPins() {
  for (char r=0; r < _kpd->rows; r++) {
    char pin = _kpd->rowPinMap[r];
    _mcp->pinMode(pin, OUTPUT);
    _mcp->pullUp(pin, LOW);
  }
}
void MCP17Keypad::closeRowPins() {
  for (char r=0; r < _kpd->rows; r++) {
    char pin = _kpd->rowPinMap[r];
    _mcp->pinMode(pin, INPUT);
    _mcp->pullUp(pin, HIGH);
  }
}
bool MCP17Keypad::check() {
  if (!awake) return false;
  if (debounce_time == 0) {
    // Fist scan after wake up
    detachInterrupt(_int);
    debounce_time = millis();
    closeRowPins();

    uint8_t pin=_mcp->getLastInterruptPin();
    last_state = scanKeys();
    return false; }

  uint32_t cur = millis();
  if ((cur - debounce_time) < debounce_delay)
    return false;

  uint16_t st = scanKeys();
  if (st != last_state) {
    // Set debounce time on state change, remeber the state and return.
    debounce_time = cur;
    last_state = st;
    return false; }

  // This state is old enough to trust it.
  if (st == 0) {
    // Going back to sleep.
    openRowPins();
    awake = false;
    debounce_time = 0;
    // attachInterrupt(_int, _callback, FALLING);
  } else
    debounce_time = cur;
#if KPD_REPEAT
  if (*_state != st) {
    *_state = st;
    repeat_time = cur;
    return true;
  } else if ((cur - repeat_time) > repeat_delay)
    return true;
  else return false;
#else
  if (*_state != st) {
    *_state = st;
    return true;
  } else return false;
#endif
}

uint16_t MCP17Keypad::scanKeys() {
  uint16_t st = 0; // stores all the closed keys.
  for (char r=0; r < _kpd->rows; r++) {
    char rpin = _kpd->rowPinMap[r];
    _mcp->pinMode(rpin, OUTPUT);
    _mcp->digitalWrite(rpin, LOW); // Begin the row pulse.
    for (char c=0; c < _kpd->cols; c++) {
      // The closed key is low so we invert to high.
      bitWrite(st, r * _kpd->cols + c, !_mcp->digitalRead(_kpd->colPinMap[c]));
    }
    // Set the row pin to a pull-up input mode. This effectively ends the row pulse.
    _mcp->pinMode(rpin, INPUT);
    _mcp->pullUp(rpin, HIGH);
  }
  return st;
}

uint16_t MCP17Keypad::scanKeys(char c) {
  uint16_t st = 0; // stores all the closed keys.
  for (char r=0; r < _kpd->rows; r++) {
    char rpin = _kpd->rowPinMap[r];
    _mcp->pinMode(rpin, OUTPUT);
    _mcp->digitalWrite(rpin, LOW); // Begin the row pulse.
    // The closed key is low so we invert to high.
    bitWrite(st, r * _kpd->cols + c, !_mcp->digitalRead(_kpd->colPinMap[c]));
    // Set the row pin to a pull-up input mode. This effectively ends the row pulse.
    _mcp->pinMode(rpin, INPUT);
    _mcp->pullUp(rpin, HIGH);
  }
  return st;
}
