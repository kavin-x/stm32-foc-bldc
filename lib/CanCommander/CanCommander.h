#ifndef CAN_COMMANDER_H
#define CAN_COMMANDER_H

#include <stdint.h>

extern uint32_t blink_interval_ms;

class CanCommander {
public:
    CanCommander();
    void process(uint8_t* data, uint8_t len);
};

#endif // CAN_COMMANDER_H
