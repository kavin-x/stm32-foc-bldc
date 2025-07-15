#ifndef AIOLI_CAN_H
#define AIOLI_CAN_H

#include <Arduino.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

// CAN command IDs
enum CanCommands : uint8_t {
  CMD_NOP = 0x00,
  CMD_SET_ANGLE = 0x01,     // [0x01][float angle]
  CMD_ENABLE_MOTOR = 0x02,  // [0x02]
  CMD_DISABLE_MOTOR = 0x03  // [0x03]
};

void FDCAN_Start(uint16_t canID);
void FDCAN_SendMessage();
void FDCAN_ChangeID(uint16_t newID);
uint16_t FDCAN_FindUniqueID(void);

#endif
