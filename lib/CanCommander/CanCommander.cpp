#include "CanCommander.h"
#include <cstring>

uint32_t blink_interval_ms = 500;  // Default value

CanCommander::CanCommander() {}

void CanCommander::process(uint8_t* data, uint8_t len) {
  if (len < 6) return;

  uint8_t id = data[0];
  uint8_t cmd = data[1];
  float value;

  memcpy(&value, &data[2], sizeof(float));  

  if (cmd == 0x01) {
    if (value >= 50.0f && value <= 5000.0f) {
      blink_interval_ms = static_cast<uint32_t>(value);
    }
  }
}
