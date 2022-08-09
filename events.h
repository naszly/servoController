#ifndef SC_EVENTS_H
#define SC_EVENTS_H

#include <EEPROM.h>

#include "servos.h"
#include "serial.h"

#define EVENT_ROM_OFFSET 256  // 0-255 reserved for servo data

struct Event {
  uint8_t enabled : 1;
  uint8_t pinID : 7;
  uint8_t pinState : 1; // LOW / HIGH
  uint8_t servoState : 1;
  uint16_t servoID : 10; // max 64 PWMs * 16 servos
};

template<Servos& servos, uint16_t count, uint8_t... pins>
class Events {
  public:
    void init() {
      for (uint16_t i = 0; i < count; ++i)
        EEPROM.get(i * sizeof(Event) + EVENT_ROM_OFFSET, events[i]);

      for (uint16_t i = 0; i < sizeof...(pins); ++i)
        pinMode(inputPins[i], INPUT_PULLUP);
    }

    void update() {
      uint8_t val;
      for (uint16_t i = 0; i < count; ++i) {
        if (events[i].enabled) {
          val = digitalRead(events[i].pinID);
          if (val == events[i].pinState) {
            servos[events[i].servoID].setState(events[i].servoState);
            if (debug) {
              SERIAL_PRINT('d');
              SERIAL_PRINTLN(events[i].pinID);
              delay(20);
            }
          }
        }
      }
    }

    void save() {
      if (debug) {
        SERIAL_PRINT(sizeof(Event) * count);
        SERIAL_PRINTLN(" Byte");
      }
      for (uint16_t i = 0; i < count; ++i) {
        EEPROM.put(i * sizeof(Event) + EVENT_ROM_OFFSET, events[i]);
      }
    }

    bool isInputPin(uint8_t pin) {
      for (uint16_t i = 0; i < sizeof...(pins); ++i)
        if (inputPins[i] == pin) return true;
      return false;
    }

    void printPins(Stream& serial) {
      for (uint16_t i = 0; i < sizeof...(pins); ++i) {
        serial.print(inputPins[i]);
        serial.print(' ');
      }
    }

    void setDebug(bool debug) {
      this->debug = debug;
    }

    Event& operator[](uint16_t index) {
      return events[index];
    }

  private:
    Event events[count];
    bool debug{0};
    const uint8_t inputPins[sizeof...(pins)] {
      pins...
    };
};
#endif
