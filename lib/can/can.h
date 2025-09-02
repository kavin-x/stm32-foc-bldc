#ifndef INC_CAN_H_
#define INC_CAN_H_

#include <Arduino.h>
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_fdcan.h"

void FDCAN_Error_Handler(void);
void MX_FDCAN1_Init(void);
void Send_CAN_Message(void);
void Activate_CAN_Notifications(void);

#endif /* INC_CAN_H_ */
