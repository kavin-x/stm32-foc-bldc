#include <Arduino.h>
#include "can.h"

uint8_t configureCAN(void);

void setup()
{
	uint8_t ret;
	ret = configureCAN();
	if (!ret)
	{
		SIMPLEFOC_DEBUG("CAN init failed.");
	}
}

void loop()
{
	// Send CAN message to Board B (0x102)
	TxHeader.Identifier = 0x102; // Set receiver ID
	TxData[0] = 0xAB;
	TxData[1] = 0xCD;
	FDCAN_SendMessage();

	Serial.println("Sent CAN message to 0x102");
	delay(1000);
}

uint8_t configureCAN(void)
{
	FDCAN_Start(0x7CC);
	return 1;
}
