#include "AS5047U_Sensor.h"

#define REG_ANGLECOM 0x3FFF

AS5047U_Sensor::AS5047U_Sensor(uint8_t csPin, SPIClass &spiPort) {
    _csPin = csPin;
    _spi = &spiPort;
}

void AS5047U_Sensor::begin() {
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    _spi->begin();
}

// Low-level read (16-bit frame without CRC)
uint16_t AS5047U_Sensor::spiRead(uint16_t addr) {
    uint16_t command = 0x4000 | (addr & 0x3FFF); // bit14=1 for read

    digitalWrite(_csPin, LOW);
    _spi->transfer16(command);
    digitalWrite(_csPin, HIGH);

    delayMicroseconds(1);

    digitalWrite(_csPin, LOW);
    uint16_t response = _spi->transfer16(0x0000);
    digitalWrite(_csPin, HIGH);

    return response & 0x3FFF; // 14-bit data
}

uint16_t AS5047U_Sensor::readRawAngle() {
    return spiRead(REG_ANGLECOM);
}

float AS5047U_Sensor::readAngleDeg() {
    return (readRawAngle() * 360.0f) / 16384.0f;
}
