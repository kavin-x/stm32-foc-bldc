#include "DRV8353FS.h"

DRV8353FS::DRV8353FS(uint8_t csPin, SPIClass *spi, uint8_t motorEnablePin, uint8_t motorFaultPin)
    : _csPin(csPin), _spi(spi), _motorEnablePin(motorEnablePin), _motorFaultPin(motorFaultPin) {}

void DRV8353FS::begin()
{
    pinMode(_csPin, OUTPUT);
    digitalWrite(_csPin, HIGH);
    if (_ledPin != 255)
    {
        pinMode(_ledPin, OUTPUT);
        digitalWrite(_ledPin, LOW);
    }
    if (_motorEnablePin != 255)
    {
        pinMode(_motorEnablePin, OUTPUT);
        digitalWrite(_motorEnablePin, HIGH); // Enable motor driver
    }
    if (_motorFaultPin != 255)
    {
        pinMode(_motorFaultPin, INPUT_PULLUP); // Driver Fault Pin
    }
    _spi->begin();
}

void DRV8353FS::attachStatusLed(uint8_t pin)
{
    _ledPin = pin;
    pinMode(_ledPin, OUTPUT);
    digitalWrite(_ledPin, LOW);
}

void DRV8353FS::updateStatusLed()
{
    if (_ledPin != 255)
    {
        digitalWrite(_ledPin, hasFault() ? LOW : HIGH);
    }
}

uint16_t DRV8353FS::spiTransfer(uint16_t data)
{
    digitalWrite(_csPin, LOW);
    _spi->beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
    uint16_t response = _spi->transfer16(data);
    _spi->endTransaction();
    digitalWrite(_csPin, HIGH);
    delayMicroseconds(1);
    return response;
}

uint16_t DRV8353FS::readRegister(uint8_t address)
{
    uint16_t command = (address & 0x7F) << 11;
    return spiTransfer(command);
}

void DRV8353FS::writeRegister(uint8_t address, uint16_t data)
{
    uint16_t command = (0 << 15) | ((address & 0x0F) << 11) | (data & 0x07FF);
    spiTransfer(command);
}

void DRV8353FS::configureGateDrive(uint8_t hs_idrive, uint8_t ls_idrive)
{
    uint16_t reg = readRegister(REG_GATE_DRIVER_CONTROL_1);
    reg &= ~0x0F0F;
    reg |= ((hs_idrive & 0x0F) << 8) | (ls_idrive & 0x0F);
    writeRegister(REG_GATE_DRIVER_CONTROL_1, reg);
}

void DRV8353FS::setOCPLevel(uint8_t level)
{
    uint16_t reg = readRegister(REG_OCP_CONTROL);
    reg &= ~0x000F;
    reg |= (level & 0x0F);
    writeRegister(REG_OCP_CONTROL, reg);
}

void DRV8353FS::configureCSA(uint8_t gain, bool bidirectional)
{
    uint16_t reg = readRegister(REG_CSA_CONTROL);
    reg &= ~0x000F;
    reg |= (gain & 0x03);
    if (bidirectional)
        reg |= (1 << 3);
    writeRegister(REG_CSA_CONTROL, reg);
}

void DRV8353FS::clearFaults()
{
    writeRegister(REG_FAULT_STATUS_1, 0x07FF);
    writeRegister(REG_FAULT_STATUS_2, 0x07FF);
}

bool DRV8353FS::hasFault()
{
    uint16_t f1 = readRegister(REG_FAULT_STATUS_1);
    uint16_t f2 = readRegister(REG_FAULT_STATUS_2);
    return (f1 != 0 || f2 != 0); // Fault exists if either is non-zero
}

void DRV8353FS::initializeRegisters()
{
    writeRegister(REG_GATE_DRIVER_CONTROL_1, 0b10000000000); // 0x02
    writeRegister(REG_GATE_DRIVER_CONTROL_2, 0b01100000000); // 0x03
    writeRegister(REG_OCP_CONTROL, 0b10000000000);           // 0x04
    writeRegister(REG_CSA_CONTROL, 0b00000100000);           // 0x05
    writeRegister(REG_CSA_OCP_CONTROL, 0b01011000011);       // 0x06
    writeRegister(REG_VDS_CONTROL, 0b00000000000);           // 0x07
}

void DRV8353FS::printStatus()
{
    uint16_t f1 = readRegister(REG_FAULT_STATUS_1);
    uint16_t f2 = readRegister(REG_FAULT_STATUS_2);

    Serial.println("DRV8353FS Fault Status Report");

    Serial.print("FAULT_STATUS_1 [0x00]: 0b");
    for (int8_t i = 10; i >= 0; i--)
        Serial.print((f1 >> i) & 0x01);
    Serial.println();

    Serial.print("FAULT_STATUS_2 [0x01]: 0b");
    for (int8_t i = 10; i >= 0; i--)
        Serial.print((f2 >> i) & 0x01);
    Serial.println();

    if (f1 == 0)
        Serial.println("No faults reported in FAULT_STATUS_1.");
    else
    {
        Serial.println("Decoded FAULT_STATUS_1:");
        if (f1 & (1 << 10))
            Serial.println(" - FAULT: OR of all faults (mirrors nFAULT pin).");
        if (f1 & (1 << 9))
            Serial.println(" - VDS_OCP: VDS monitor overcurrent fault.");
        if (f1 & (1 << 8))
            Serial.println(" - GDF: Gate drive fault condition.");
        if (f1 & (1 << 7))
            Serial.println(" - UVLO: Undervoltage lockout condition.");
        if (f1 & (1 << 6))
            Serial.println(" - OTSD: Overtemperature shutdown.");
        if (f1 & (1 << 5))
            Serial.println(" - VDS_HA: Overcurrent on A low-side MOSFET.");
        if (f1 & (1 << 4))
            Serial.println(" - VDS_LA: Overcurrent on A high-side MOSFET.");
        if (f1 & (1 << 3))
            Serial.println(" - VDS_HB: Overcurrent on B low-side MOSFET.");
        if (f1 & (1 << 2))
            Serial.println(" - VDS_LB: Overcurrent on B high-side MOSFET.");
        if (f1 & (1 << 1))
            Serial.println(" - VDS_HC: Overcurrent on C low-side MOSFET.");
        if (f1 & (1 << 0))
            Serial.println(" - VDS_LC: Overcurrent on C high-side MOSFET.");
    }

    if (f2 == 0)
        Serial.println("No faults reported in FAULT_STATUS_2.");
    else
    {
        Serial.println("Decoded FAULT_STATUS_2:");
        if (f2 & (1 << 10))
            Serial.println(" - SA_OC: Overcurrent on phase A sense amplifier.");
        if (f2 & (1 << 9))
            Serial.println(" - SB_OC: Overcurrent on phase B sense amplifier.");
        if (f2 & (1 << 8))
            Serial.println(" - SC_OC: Overcurrent on phase C sense amplifier.");
        if (f2 & (1 << 7))
            Serial.println(" - OTW: Overtemperature warning.");
        if (f2 & (1 << 6))
            Serial.println(" - GDUV: Charge pump or VGLS undervoltage.");
        if (f2 & (1 << 5))
            Serial.println(" - VGS_HA: Gate fault on A high-side MOSFET.");
        if (f2 & (1 << 4))
            Serial.println(" - VGS_LA: Gate fault on A low-side MOSFET.");
        if (f2 & (1 << 3))
            Serial.println(" - VGS_HB: Gate fault on B high-side MOSFET.");
        if (f2 & (1 << 2))
            Serial.println(" - VGS_LB: Gate fault on B low-side MOSFET.");
        if (f2 & (1 << 1))
            Serial.println(" - VGS_HC: Gate fault on C high-side MOSFET.");
        if (f2 & (1 << 0))
            Serial.println(" - VGS_LC: Gate fault on C low-side MOSFET.");
    }

    Serial.println("Use drv.clearFaults() after resolving the issues.");
}

void DRV8353FS::readAllRegisters()
{
    Serial.println("DRV8353FS Register Dump (Binary):");
    for (uint8_t i = 0x00; i <= 0x0A; i++)
    {
        uint16_t val = readRegister(i);
        Serial.print("Reg 0x");
        if (i < 0x10)
            Serial.print("0");
        Serial.print(i, HEX);
        Serial.print(": 0b");
        for (int8_t b = 15; b >= 0; b--)
        {
            Serial.print((val >> b) & 0x01);
        }
        Serial.println();
    }
}
