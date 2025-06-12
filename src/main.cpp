#include <SPI.h>
#include <SimpleFOC.h>

#define SELECT PA15
#define MOTOR_ENABLE PB10
#define MOTOR_FAULT PB1

BLDCMotor motor = BLDCMotor(7);

BLDCDriver6PWM driver = BLDCDriver6PWM(PA8, PB13, PA9, PB14, PA10, PB15);

SPIClass SPI_3(PB5, PB4, PB3);

// velocity set point variable
float target_velocity = 0;
// commander interface
Commander command = Commander(Serial);
void doTarget(char* cmd){ command.scalar(&target_velocity, cmd); }

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
  // Set address 0x02h to (PWM_MODE = 00b, OCP_ACT = 1b, Others to Default)
  writeRegister(0b0001010000000000);
  // Set address 0x03h to (All values are set to Default)
  writeRegister(0b0001101100000000);
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
  pinMode(PC14, OUTPUT);
  Serial.println("Starting Driver Initialization");
  DRV8353_init();
  SimpleFOCDebug::enable(&Serial);

  // driver config
  // power supply voltage [V]
  driver.voltage_power_supply = 24;
  driver.init();
  // link the motor and the driver
  motor.linkDriver(&driver);

  // limiting motor current (provided resistance)
  motor.voltage_limit = 2.5; // [Amps]

  // open loop control config
  motor.controller = MotionControlType::velocity_openloop;

  // init motor hardware
  motor.init();
  motor.initFOC();

  // add target command T
  command.add('T', doTarget, "target velocity");

  Serial.begin(115200);
  Serial.println("Motor ready!");
  Serial.println("Set target velocity [rad/s]");
  _delay(1000);
}

void loop()
{
    motor.move(target_velocity);
}
