#ifndef SC_SERIAL_H
#define SC_SERIAL_H

#include <SoftwareSerial.h>

/*
  SoftwareSerial Bluetooth(12, 13); // RX, TX
  #define SERIAL_BEGIN(...) Serial.begin(__VA_ARGS__); Bluetooth.begin(__VA_ARGS__);
  #define SERIAL_PRINT(...) Serial.print(__VA_ARGS__); Bluetooth.print(__VA_ARGS__);
  #define SERIAL_PRINTLN(...) Serial.println(__VA_ARGS__); Bluetooth.println(__VA_ARGS__);
*/

#define SERIAL_BEGIN(...) DualSerial::begin(__VA_ARGS__)
#define SERIAL_PRINT(...) DualSerial::print(__VA_ARGS__)
#define SERIAL_PRINTLN(...) DualSerial::println(__VA_ARGS__)

#define BLUETOOTH DualSerial::getBluetooth()

class DualSerial {
  public:
    template<typename... Args>
    static void begin(Args... args) {
      Serial.begin(args...);
      getBluetooth().begin(args...);
    }

    template<typename... Args>
    static void print(Args... args) {
      Serial.print(args...);
      getBluetooth().print(args...);
    }

    template<typename... Args>
    static void println(Args... args) {
      Serial.println(args...);
      getBluetooth().println(args...);
    }

    static SoftwareSerial& getBluetooth() {
      static SoftwareSerial bluetooth(12, 13); // RX, TX
      return bluetooth;
    }
};


#endif
