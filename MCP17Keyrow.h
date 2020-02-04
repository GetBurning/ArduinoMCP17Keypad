#ifndef MCP17Keyrow_H
#define MCP17Keyrow_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#ifndef KPD_REPEAT
#define KPD_REPEAT true
#endif

class MCP17Keyrow {
public:
  // No rows, no columns, only keys.
  MCP17Keyrow(Adafruit_MCP23017 *mcp,
	      char pinMap[], uint8_t width, uint16_t *state,
	      uint16_t debounceDelay = 100, uint16_t repeatDelay = 500);
  void begin(void);
  void begin(char intPin, void(*intCallback)(void));
  void end(void);
#if KPD_REPEAT
  inline void setRepeatDelay(uint16_t repeatDelay) {repeat_delay = repeatDelay;}
#endif
  inline void wakeUp(void) {awake = true;}
  bool check(void);
  uint16_t scanKeys(void);
  uint16_t scanKeys(char k);
private:
  Adafruit_MCP23017 *_mcp;
  char *keyPinMap;
  uint8_t _width;
  uint16_t *_state; // 16 key row is a maximum for uint16_t;
  char _int;
  // portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
  // void (*_callback)(void);
  volatile bool awake;
  uint16_t last_state;
  uint32_t debounce_time;
  uint16_t debounce_delay;
#if KPD_REPEAT
  uint16_t repeat_time;
  uint16_t repeat_delay;
#endif
};

#endif
