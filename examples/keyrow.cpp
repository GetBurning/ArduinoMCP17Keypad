// -*- mode: arduino -*-
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <MCP17Keyrow.h>

#define SERIAL_BAUD 115200
#define MCP17_ADDR 0
#define MCP17_INT_MIRROR false
#define MCP17_INTA_PIN 34
#define MCP17_INTB_PIN 35

Adafruit_MCP23017 mcp17;
uint16_t kpd_state;
char keyPinMap[] = {1, 0, 3, 2};
MCP17Keyrow kpd(&mcp17, keyPinMap, sizeof(keyPinMap)/sizeof(char), &kpd_state);
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
void IRAM_ATTR kpd_callback(void) {
  portENTER_CRITICAL_ISR(&mux);
  kpd.wakeUp();
  portEXIT_CRITICAL_ISR(&mux);
}
typedef enum {DOWN = 1,	UP = 2,	LEFT = 4, RIGHT = 8} button_t;

void setup() {
  Serial.begin(SERIAL_BAUD);
  Wire.begin();
  mcp17.begin(MCP17_ADDR, &Wire);
  mcp17.setupInterrupts(MCP17_INT_MIRROR, false, LOW);
  kpd.begin(MCP17_INTA_PIN, kpd_callback);
}

void main() {
  if (kpd.check()) {
    switch (kpd_state) {
    case DOWN:
      Serial.println("Down key was pressed");
      break;
    case UP:
      Serial.println("Up key was pressed");
      break;
    case RIGHT:
      Serial.println("Right key was pressed");
      break;
    case LEFT:
      Serial.println("Left key was pressed");
      break;
    case 3:
      Serial.println("Up and down keys were pressed");
      break;
    }
  }
}
