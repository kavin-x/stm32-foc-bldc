#include <SimpleFOC.h>

#define SELECT PB11
#define MOTOR_ENABLE PB12
#define MOTOR_FAULT PC6

BLDCMotor motor = BLDCMotor(4);

BLDCDriver6PWM driver = BLDCDriver6PWM(PA8, PB13, PA9, PB14, PA10, PB15);

SPIClass SPI_3(PB5, PB4, PB3);

SPIClass SPI_1(PA7, PA6, PA5);

MagneticSensorSPI sensor = MagneticSensorSPI(PC4, 14, 0x3FFF);

// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.scalar(&motor.target, cmd); }
void doLimit(char *cmd) { command.scalar(&motor.voltage_limit, cmd); }

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
  // use monitoring with serial
  Serial.begin(115200);
  DRV8353_init();

  // // enable more verbose output for debugging
  // // comment out if not needed
  // SimpleFOCDebug::enable(&Serial);
  SPI_1.begin();
  // initialize magnetic sensor hardware
  sensor.init(&SPI_1);

  // motor.linkSensor(&sensor);

  // driver.voltage_power_supply = 12;
  // // limit the maximal dc voltage the driver can set
  // // as a protection measure for the low-resistance motors
  // // this value is fixed on startup
  // if (!driver.init())
  // {
  //   Serial.println("Driver init failed!");
  //   return;
  // }

  // // link the motor and the driver
  // motor.linkDriver(&driver);

  // // limiting motor movements
  // // limit the voltage to be set to the motor
  // // start very low for high resistance motors
  // // current = voltage / resistance, so try to be well under 1Amp
  // motor.voltage_limit = 1; // [V]

  // // open loop control config
  // motor.controller = MotionControlType::velocity;

  // // init motor hardware
  // if (!motor.init())
  // {
  //   Serial.println("Motor init failed!");
  //   return;
  // }
  // align sensor and start FOC
  // motor.initFOC();

  // add target command T
  command.add('T', doTarget, "target velocity");
  command.add('L', doLimit, "voltage limit");

  Serial.println("Motor ready!");
  Serial.println("Set target velocity [rad/s]");
  _delay(1000);
}

// velocity set point variable
float target_velocity = 10; // 2Rad/s ~ 20rpm

void loop()
{
  // iterative function updating the sensor internal variables
  // it is usually called in motor.loopFOC()
  // this function reads the sensor hardware and 
  // has to be called before getAngle nad getVelocity
  sensor.update();
  // display the angle and the angular velocity to the terminal
  Serial.print(sensor.getAngle());
  Serial.print("\t");
  Serial.println(sensor.getVelocity());
}
