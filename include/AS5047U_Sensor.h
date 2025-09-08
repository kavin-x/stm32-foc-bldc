#ifndef AS5047U_SENSOR_H
#define AS5047U_SENSOR_H

#include <Arduino.h>
#include <SPI.h>

class AS5047U_Sensor {
public:
    AS5047U_Sensor(uint8_t csPin, SPIClass &spiPort = SPI);
    void begin();
    uint16_t readRawAngle();    // 0 - 16383
    float readAngleDeg();       // 0.0 - 360.0

private:
    uint16_t spiRead(uint16_t addr);

    uint8_t _csPin;
    SPIClass* _spi;
};

#endif
