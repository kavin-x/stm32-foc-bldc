#include <SPI.h>
#include <SimpleFOC.h>

#define SELECT PA15
#define MOTOR_ENABLE PB10
#define MOTOR_FAULT PB1

BLDCMotor motor = BLDCMotor(4);
// BLDC driver instance
BLDCDriver6PWM driver = BLDCDriver6PWM(PA8, PB13, PA9, PB14, PA10, PB15);

SPIClass SPI_3(PB5, PB4, PB3);

void printBinary(uint16_t value)
{
  for (int i = 15; i >= 0; i--)
  {
    Serial.print((value >> i) & 1);
  }
  Serial.println();
}

void readRegister(uint16_t addr)
{
  SPI_3.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SELECT, LOW);
  // Read address 0x03
  uint16_t response = SPI_3.transfer16(addr);
  // take the SS pin high to de-select the chip:
  SPI_3.endTransaction();
  digitalWrite(SELECT, HIGH);
  delayMicroseconds(1);
  printBinary(response); // Print the binary response
}

void writeRegister(uint16_t addr)
{
  SPI_3.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SELECT, LOW);
  SPI_3.transfer16(addr);
  digitalWrite(SELECT, HIGH);
  delayMicroseconds(1);
  SPI_3.endTransaction();
}

void DRV8353_init()
{
  pinMode(SELECT, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);
  digitalWrite(MOTOR_ENABLE, HIGH);
  digitalWrite(SELECT, HIGH);
  SPI_3.begin();
  delayMicroseconds(100);
  // Set address 0x02h to (PWM_MODE = 01b, OCP_ACT = 1b, Others to Default)
  writeRegister(0b0001010000000001);
  // Set address 0x03h to (All values are set to Default)
  writeRegister(0b0001101100110011);
  // Set address 0x04h to (TDRIVE = 00b Others to Default)
  writeRegister(0b0010010000000000);
  // Set address 0x05h to (DEAD_TIME = 00b, OCP_MODE =00b, OCP_DEG=00b VDS_LVL=0000b)
  writeRegister(0b0010100000100000);
  // Set address 0x06h to (CSA_GAIN = 11b, Others to default)
  writeRegister(0b0011001011000011);
  // Set address 0x07h to (All values are set to Default)
  writeRegister(0b0011100000000000);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(PC14, OUTPUT);
  Serial.println("Starting Driver Initialization");
  DRV8353_init();

    // power supply voltage [V]
  // driver.voltage_power_supply = 12;
  // // Max DC voltage allowed - default voltage_power_supply
  // driver.voltage_limit = 12;

  // // driver init
  // if (!driver.init()){
  //   Serial.println("Driver init failed!");
  //   return;
  // }

  // // enable driver
  // driver.enable();
  
  // Serial.println("Driver ready!");
  delay(1000);
}

void loop()
{
  readRegister(0b1000000000000000);
  readRegister(0b1000100000000000);
  delay(500);
}
