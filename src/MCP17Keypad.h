#ifndef MCP17Keypad_H
#define MCP17Keypad_H
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#ifndef KPD_REPEAT
#define KPD_REPEAT true
#endif

typedef struct Keypad_t {
  char *rowPinMap;
  char rows;
  char *colPinMap;
  char cols;
} Keypad_t;

class MCP17Keypad {
public:
  // Row count should be lower until columns > rows * 6.
  MCP17Keypad(Adafruit_MCP23017 *mcp, Keypad_t *kpd, uint16_t *state,
	      uint16_t debounceDelay = 100, uint16_t repeatDelay = 500);
  void begin(void);
  void begin(char intPin, void(*intCallback)(void));
  inline void end(void) {detachInterrupt(_int);};
#if KPD_REPEAT
  inline void setRepeatDelay(uint16_t repeatDelay) {repeat_delay = repeatDelay;}
#endif
  inline void wakeUp(void) {awake = true;}
  bool check(void);
  uint16_t scanKeys(void);
  uint16_t scanKeys(char c);
private:
  void openRowPins(void);
  void closeRowPins(void);
  Adafruit_MCP23017 *_mcp;
  Keypad_t *_kpd;
  uint16_t *_state; // 4x4 keypad is a maximum for uint16_t;
  char _int;
  // void (*_callback)(void);
  volatile bool awake;
  uint16_t last_state;
  uint16_t debounce_delay;
  uint32_t debounce_time;
#if KPD_REPEAT
  uint16_t repeat_time;
  uint16_t repeat_delay;
#endif

};

#endif
