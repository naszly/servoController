#ifndef SC_SERVOS_H
#define SC_SERVOS_H

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <EEPROM.h>

#include "serial.h"

#define MAX_PWMS 4
#define MAX_SERVOS 16 * MAX_PWMS
#define START_ADDRESS 0x40        //default address

#define SERVO_MIN 100  // This is the 'minimum' pulse length count (out of 4096)
#define SERVO_MAX 500  // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50  // Analog servos run at ~50 Hz updates

#define TO_PULSE(angle) map(angle, 0, 180, SERVO_MIN, SERVO_MAX)
#define TO_ANGLE(pulse) map(pulse, SERVO_MIN, SERVO_MAX, 0, 180)

class Servos {
  public:
    class Servo {
        friend class Servos;

      public:
        uint8_t ang[2]; // min, max angle

        void setState(uint8_t state); // set to min / max angle
        uint8_t getAng();
        void setAng(uint8_t ang);

      private:
        uint16_t currentPulseLength : 12;  // 0 - 4096
        uint16_t finalPulseLength : 12;
    };

    void init();
    void update();
    void save();

    uint16_t count() {
      return servoCount;
    }

    Servo& operator[](uint16_t index) {
      return servos[index];
    }

  private:
    Adafruit_PWMServoDriver* pwm[MAX_PWMS];
    Servo servos[MAX_SERVOS];
    uint16_t servoCount{0};

    uint64_t lastTime;
    const float updateStep{1000.0f / 120.0f};

    void initPWMs();
    void initServos();
    void servoPulseLength(uint16_t i, uint16_t pulseLength);
    void updateServos();
};

#endif
