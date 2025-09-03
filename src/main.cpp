#include "DRV8353FS.h"
#include <SimpleFOC.h>

#define DRV_CS_PIN PB11
#define MOTOR_ENABLE PB12
#define MOTOR_FAULT PC6

SPIClass SPI_3(PB5, PB4, PB3); 
DRV8353FS drv(DRV_CS_PIN, &SPI_3, MOTOR_ENABLE, MOTOR_FAULT);

SPIClass SPI_1(PA7, PA6, PA5);
MagneticSensorSPI sensor = MagneticSensorSPI(PC4, 14, 0x3FFF);

void setup()
{
  Serial.begin(115200);
  drv.begin();
  drv.clearFaults();
  drv.initializeRegisters(); // Configure all registers

  sensor.init(&SPI_1);
}

void loop()
{
  sensor.update();
  Serial.print(sensor.getAngle());
  Serial.print("\t");
  Serial.println(sensor.getVelocity());
  delay(1000);
}