#include <SPI.h>
#include <SimpleFOC.h>
#include <DRV8353FS.h>

#define MOTOR_ENABLE PB10
#define MOTOR_FAULT PB1

SPIClass SPI_3(PB5, PB4, PB3); // MOSI, MISO, SCK
DRV8353FS drv(PA15, &SPI_3);   // CS pin on D10

// BLDCDriver6PWM(pwmAh, pwmAl, pwmBh, pwmBl, pwmCh, pwmCl, (en optional))
BLDCDriver6PWM driver = BLDCDriver6PWM(PA8, PB13, PA9, PB14, PA10, PB15);

void DRV8353_init()
{
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);
  digitalWrite(MOTOR_ENABLE, HIGH);
}

void setup()
{
  Serial.begin(115200);
  delay(5000);
  DRV8353_init();
  drv.begin();
  Serial.println("Initializing DRV8353FS...");

  drv.clearFaults();
  // drv.configureGateDrive(0x0F, 0x0F);     // Max gate strength
  // drv.setOCPLevel(8);
  // Mid overcurrent level
  // drv.configureCSA(0x01, true);          // 10V/V gain, bidirectional
  drv.attachStatusLed(PC14);
  drv.readAllRegisters();
  drv.printStatus();

  // power supply voltage [V]
  driver.voltage_power_supply = 12;
  // Max DC voltage allowed - default voltage_power_supply
  driver.voltage_limit = 12;
  // daad_zone [0,1] - default 0.02f - 2%
  driver.dead_zone = 0.05f;

  // driver init
  if (!driver.init())
  {
    Serial.println("Driver init failed!");
    return;
  }

  // enable driver
  driver.enable();
  Serial.println("Driver ready!");
  _delay(1000);
}

void loop()
{
  // Your motor control logic here
  drv.readAllRegisters();
  delay(5000);
  // driver.setPwm(3,6,5);
}
