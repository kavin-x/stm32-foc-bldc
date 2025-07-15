#include <Arduino.h>
#include "can.h"

uint8_t configureCAN(void);

void setup()
{
	delay(5000);

	Serial.begin(115200);
	if (!configureCAN())
	{
		Serial.println("CAN init failed.");
	}
	else{
		Serial.println("CAN init successful");
	}
}

void loop()
{
	TxHeader.Identifier = 0x102;
	TxData[0] = 0xAB;
	TxData[1] = 0xCD;
	FDCAN_SendMessage();

	Serial.println("Board A sent to 0x102");
	delay(1000);
}

uint8_t configureCAN(void)
{
	FDCAN_Start(0x101); // This board uses ID 0x101
	return 1;
}
