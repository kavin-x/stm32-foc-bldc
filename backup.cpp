#include <SPI.h>
#include <SimpleFOC.h>

#define SELECT PB10
#define MOTOR_ENABLE PB11
#define MOTOR_FAULT PB12

LowsideCurrentSense current_sense = LowsideCurrentSense(0.003, 40, PA0, PA1, PA2);

MagneticSensorSPI sensor = MagneticSensorSPI(PA4, 14, 0x3FFF);
// BLDC motor & driver instance
// BLDCMotor motor = BLDCMotor(pole pair number, phase resistance (optional) );
BLDCMotor motor = BLDCMotor(4);
// BLDC driver instance
BLDCDriver6PWM driver = BLDCDriver6PWM(PA8, PC13, PA9, PB0, PA10, PB1);

SPIClass SPI_2(PB15, PB14, PB13);

//target variable
float target_velocity = 60;

// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char* cmd) { command.scalar(&target_velocity, cmd); }
void doLimit(char* cmd) { command.scalar(&motor.voltage_limit, cmd); }

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
  SPI_2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SELECT, LOW);
  // Read address 0x03
  uint16_t response = SPI_2.transfer16(addr);
  // take the SS pin high to de-select the chip:
  SPI_2.endTransaction();
  digitalWrite(SELECT, HIGH);
  delayMicroseconds(1);
  printBinary(response); // Print the binary response
}

void writeRegister(uint16_t addr)
{
  SPI_2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE1));
  digitalWrite(SELECT, LOW);
  SPI_2.transfer16(addr);
  digitalWrite(SELECT, HIGH);
  delayMicroseconds(1);
  SPI_2.endTransaction();
}

void DRV8353_init()
{
  pinMode(SELECT, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);
  digitalWrite(MOTOR_ENABLE, HIGH);
  digitalWrite(SELECT, HIGH);
  SPI_2.begin();
  delayMicroseconds(100);
  // Set address 0x02h to (PWM_MODE = 01b, OCP_ACT = 1b, Others to Default)
  writeRegister(0b0 00101 0000000000);
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

void setup() { 
  DRV8353_init();
  sensor.init();
  // link the motor to the sensor
  // motor.linkSensor(&sensor);

  driver.voltage_power_supply = 24;
  driver.init();
  
    // link the driver to the current sense
  // current_sense.linkDriver(&driver);
  // link driver
  motor.linkDriver(&driver);

  motor.voltage_limit = 2;
  // set motion control loop to be used
  motor.controller = MotionControlType::angle_openloop;

  // use monitoring with serial 
  Serial.begin(115200);
  // comment out if not needed
  motor.useMonitoring(Serial);

  // initialize motor
  motor.init();
  // init current sense
  current_sense.init();
  // link the current sense to the motor
  motor.linkCurrentSense(&current_sense);
  
  // align sensor and start FOC
  motor.initFOC();
  command.add('M', doTarget, "motor");
  _delay(1000);
}

void loop() {

  // main FOC algorithm function
  motor.loopFOC();
  // open loop velocity movement
  // using motor.voltage_limit and motor.velocity_limit
  // to turn the motor "backwards", just set a negative target_velocity
  motor.move(target_velocity);
  // user communication
  command.run();
}
