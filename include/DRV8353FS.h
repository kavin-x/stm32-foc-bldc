// DRV8353FS.h
#ifndef DRV8353FS_H
#define DRV8353FS_H

#include <Arduino.h>
#include <SPI.h>

// Register addresses
#define REG_FAULT_STATUS_1         0x00
#define REG_FAULT_STATUS_2         0x01
#define REG_GATE_DRIVER_CONTROL_1  0x02
#define REG_GATE_DRIVER_CONTROL_2  0x03
#define REG_OCP_CONTROL            0x04
#define REG_CSA_CONTROL            0x05
#define REG_CSA_OCP_CONTROL        0x06
#define REG_VDS_CONTROL            0x07
#define REG_TEMPERATURE_CONTROL    0x08
#define REG_GATE_DRIVER_CONTROL_3  0x09
#define REG_CSA_CONTROL_2          0x0A

class DRV8353FS {
public:
    DRV8353FS(uint8_t csPin, SPIClass* spi = &SPI);

    void begin();

    uint16_t readRegister(uint8_t address);
    void writeRegister(uint8_t address, uint16_t data);

    void configureGateDrive(uint8_t hs_idrive, uint8_t ls_idrive);
    void setOCPLevel(uint8_t level);
    void configureCSA(uint8_t gain, bool bidirectional);
    void clearFaults();

    void printStatus();
    void readAllRegisters();
    bool hasFault();
    void attachStatusLed(uint8_t pin);
    void updateStatusLed();

private:
    uint8_t _csPin;
    SPIClass* _spi;

    uint16_t spiTransfer(uint16_t data);
    uint8_t _ledPin = 255; // 255 means LED not used
};

#endif // DRV8353FS_H