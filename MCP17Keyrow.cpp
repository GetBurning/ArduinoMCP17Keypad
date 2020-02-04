#include "MCP17Keyrow.h"

MCP17Keyrow::MCP17Keyrow(
  Adafruit_MCP23017 *mcp, char pinMap[], uint8_t width,
  uint16_t *state, uint16_t debounceDelay, uint16_t repeatDelay) {
  _mcp = mcp; keyPinMap = pinMap; _width = width;
  _state = state;
  // Set the keypad state to avoid any weird behavior.
  *_state = 0; last_state = 0; awake = true;
  debounce_time = 0; debounce_delay = debounceDelay;
#if KPD_REPEAT
  repeat_time = 0; repeat_delay = repeatDelay;
#endif
}
void MCP17Keyrow::begin() {
  // Key pins are waiting for an interrupt in a pull-up mode.
  for (char k=0; k < _width; k++) {
    char pin = keyPinMap[k];
    _mcp->pinMode(pin, INPUT);
    _mcp->pullUp(pin, HIGH);
    _mcp->setupInterruptPin(pin, FALLING);
  }
}
void MCP17Keyrow::begin(char intPin, void(*intCallback)(void)) {
  _int = digitalPinToInterrupt(intPin);
  // _callback = intCallback;
  pinMode(intPin, INPUT);
  begin();
  attachInterrupt(_int, intCallback, FALLING);
}
bool MCP17Keyrow::check() {
  if (!awake) return false;
  if (debounce_time == 0) {
    // Fist scan after wake up
    // detachInterrupt(_int);
    debounce_time = millis();

    uint8_t pin=_mcp->getLastInterruptPin();
    uint8_t val=_mcp->getLastInterruptPinValue();
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

uint16_t MCP17Keyrow::scanKeys() {
  uint16_t st = 0; // stores all the closed keys.
  for (char k=0; k < _width; k++) {
    // The closed key is low so we invert to high.
    bitWrite(st, k, !_mcp->digitalRead(keyPinMap[k]));
  }
  return st;
}

uint16_t MCP17Keyrow::scanKeys(char k) {
  uint16_t st = 0; // stores all the closed keys.
  bitWrite(st, k, !_mcp->digitalRead(keyPinMap[k]));
  return st;
}
