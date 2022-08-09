#include "servos.h"

void Servos::Servo::setState(uint8_t state) {
  finalPulseLength = TO_PULSE(ang[state]);
}

uint8_t Servos::Servo::getAng() {
  return TO_ANGLE(currentPulseLength);
}

void Servos::Servo::setAng(uint8_t ang) {
  finalPulseLength = TO_PULSE(ang);
}


void Servos::init() {
  initPWMs();
  initServos();
}

void Servos::update() {
  uint64_t now = millis();

  float acc = (now - lastTime);

  ///////
  if (acc > 1000) {
    SERIAL_PRINT(F("WARNING: servo last update was "));
    SERIAL_PRINT(acc);
    SERIAL_PRINTLN(F("ms ago"));
    now = millis();
    acc = 1000;
  }
  ///////

  if (acc > updateStep) {
    lastTime = now;

    while (acc > updateStep) {
      updateServos();
      acc -= updateStep;
    }
  }
}

void Servos::save() {
  for (uint16_t i = 0; i < MAX_SERVOS * 2; ++i) {
    EEPROM.write(i, servos[i / 2].ang[i % 2]);
  }
}


void Servos::initPWMs() {
  uint8_t errorResult;
  uint8_t i2c_address;
  uint8_t numDevices = 0;

  servoCount = 0;

  Wire.begin();

  for (i2c_address = START_ADDRESS; i2c_address < START_ADDRESS + MAX_PWMS; ++i2c_address) {
    Wire.beginTransmission(i2c_address);   // initiate communication at current address
    errorResult = Wire.endTransmission();  // if a device is present, it will send an ack and "0" will be returned from function

    if (errorResult == 0)  // "0" means a device at current address has acknowledged the serial communication
    {
      SERIAL_PRINT(F("I2C device found at address 0x"));
      SERIAL_PRINTLN(i2c_address, HEX);  // display the address on the serial monitor when a device is found

      pwm[numDevices] = new Adafruit_PWMServoDriver(i2c_address);
      servoCount += 16;

      pwm[numDevices]->begin();
      pwm[numDevices]->setOscillatorFrequency(27000000);
      pwm[numDevices]->setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

      ++numDevices;

    } else {
      SERIAL_PRINT(F("I2C device NOT found at address 0x"));
      SERIAL_PRINT(i2c_address, HEX);
      SERIAL_PRINTLN(F(", Terminate search"));
      break;  // do not connect more devices if one is disconnected
    }
  }

  SERIAL_PRINT(numDevices);
  SERIAL_PRINT(F(" of "));
  SERIAL_PRINT(MAX_PWMS);
  SERIAL_PRINTLN(F(" I2C devices connected"));
}

void Servos::initServos() {
  for (uint16_t i = 0; i < MAX_SERVOS * 2; ++i) {
    uint8_t data = EEPROM.read(i);
    servos[i / 2].ang[i % 2] = min(data, 180);
  }

  for (uint16_t i = 0; i < MAX_SERVOS; ++i) {
    servos[i].currentPulseLength = TO_PULSE(servos[i].ang[1]);
    servoPulseLength(i, servos[i].currentPulseLength);
    servos[i].finalPulseLength = TO_PULSE(servos[i].ang[0]);
  }

  delay(100);

  for (uint16_t i = 0; i < 100; ++i) {
    uint64_t lt = millis();
    updateServos();
    uint64_t dt = millis() - lt;
    if (dt < updateStep) delay(updateStep - dt);
  }

  lastTime = millis();
}

void Servos::servoPulseLength(uint16_t i, uint16_t pulseLength) {
  if (pwm[i / 16])
    pwm[i / 16]->setPWM(i % 16, 0, pulseLength);
}

void Servos::updateServos() {
  uint16_t i = 0;
  while (i < MAX_SERVOS) {
    if (servos[i].finalPulseLength != servos[i].currentPulseLength) {
      if (servos[i].finalPulseLength > servos[i].currentPulseLength)
        ++servos[i].currentPulseLength;
      if (servos[i].finalPulseLength < servos[i].currentPulseLength)
        --servos[i].currentPulseLength;

      servoPulseLength(i, servos[i].currentPulseLength);
    }

    ++i;
  }
}
