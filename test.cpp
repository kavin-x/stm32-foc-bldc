#define SELECT PB12
#define MOTOR_SLEEP PA15
#define MOTOR_ENABLE PB9
#define MOTOR_FAULT PB10

SPIClass SPI_2(PB5, PB4, PB3);

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

void DRV8316_init()
{
  pinMode(SELECT, OUTPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_FAULT, INPUT_PULLUP);
  digitalWrite(MOTOR_ENABLE, HIGH);
  digitalWrite(SELECT, HIGH);

  Serial.println("Initialzing....SPI");
  SPI_2.begin();
  Serial.println("InitialziED SPI");
  delayMicroseconds(100);

  readRegister(0b1000000000000000);
  readRegister(0b1000001100000000);
  readRegister(0b1000010100000000);
  readRegister(0b1000011100000000);
  readRegister(0b1000100100000000);
  readRegister(0b1000101100000000);
  readRegister(0b1000110100000000);
  readRegister(0b1000111100000000);
  readRegister(0b1001000100000000);
  readRegister(0b1001001100000000);

  Serial.println("UNLOCKING REGISTERS");
  readRegister(0b100001100000000);
  writeRegister(0b0000011000000011);
  readRegister(0b100001100000000);
  Serial.println("Registers unlocked");

  Serial.println("Clearing Fault");
  readRegister(0b1000100100000000);
  writeRegister(0b0000100001100001);
  readRegister(0b1000100100000000);
  Serial.println("Cleared Fault");

  Serial.println("Before setting Buck");
  readRegister(0b1001000100000000);
  writeRegister(0b0001000000000010);
  Serial.println("After setting Buck");
  readRegister(0b1001000100000000);
}

void setup() { 
  pinMode(PC13, OUTPUT);
  Serial.begin(115200);
  DRV8316_init();
}

void loop() {
  // Read the state of PB10
  int buttonState = digitalRead(MOTOR_FAULT);

  // If PB10 is LOW, turn on the LED on PC13
  if (buttonState == LOW) {
    digitalWrite(PC13, HIGH); // LED on
  } else {
    digitalWrite(PC13, LOW); // LED off
  }
  // Small delay for debounce (optional)
  delay(50);
}