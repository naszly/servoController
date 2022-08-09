#include <StaticSerialCommands.h>

#include "servos.h"
#include "events.h"
#include "serial.h"

#define MAX_EVENTS 48
// digital 2-11, analog 0-3
#define PINS 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, 16, 17

void cmd_servo_list(SerialCommands& sender, Args &args);
void cmd_servo_ang(SerialCommands& sender, Args &args);
void cmd_servo_set(SerialCommands& sender, Args &args);
void cmd_servo_get(SerialCommands& sender, Args &args);
void cmd_servo_min(SerialCommands& sender, Args &args);
void cmd_servo_max(SerialCommands& sender, Args &args);
void cmd_servo_set_all(SerialCommands& sender, Args &args);
void cmd_servo_min_all(SerialCommands& sender, Args &args);
void cmd_servo_max_all(SerialCommands& sender, Args &args);
void cmd_servo_swap(SerialCommands& sender, Args &args);
void cmd_servo_save(SerialCommands& sender, Args &args);
void cmd_servo_reload(SerialCommands& sender, Args &args);
void cmd_servo_help(SerialCommands& sender, Args &args);
void cmd_mm(SerialCommands& sender, Args &args);
void cmd_event_list(SerialCommands& sender, Args &args);
void cmd_event_enable(SerialCommands& sender, Args &args);
void cmd_event_enable_from_to(SerialCommands& sender, Args &args);
void cmd_event_disable(SerialCommands& sender, Args &args);
void cmd_event_disable_from_to(SerialCommands& sender, Args &args);
void cmd_event_disable_all(SerialCommands& sender, Args &args);
void cmd_event_set(SerialCommands& sender, Args &args);
void cmd_event_get(SerialCommands& sender, Args &args);
void cmd_event_save(SerialCommands& sender, Args &args);
void cmd_event_help(SerialCommands& sender, Args &args);
void cmd_help(SerialCommands& sender, Args &args);
void cmd_export(SerialCommands& sender, Args &args);
void cmd_save(SerialCommands& sender, Args &args);
void cmd_set_debug(SerialCommands& sender, Args &args);
void cmd_send(SerialCommands& sender, Args &args);

constexpr auto boolArg = ARG(ArgType::Int, 0, 1, "bool");

constexpr auto servoIDArg = ARG(ArgType::Int, 0, MAX_SERVOS - 1, "servoID");
constexpr auto servoAngleArg = ARG(ArgType::Int, 0, 180, "ang");
constexpr auto servoStateArg = ARG(ArgType::Int, 0, 1, "servoState");

constexpr auto eventIDArg = ARG(ArgType::Int, 0, MAX_EVENTS - 1, "eventID");
constexpr auto eventPinArg = ARG(ArgType::Int, "pin");
constexpr auto pinStateArg = ARG(ArgType::Int, 0, 1, "pinState");

Command servoCommands[] {
  COMMAND(cmd_servo_list,    "list",                            nullptr, "list servos min, max"),
  COMMAND(cmd_servo_ang,     "ang",  servoIDArg, servoAngleArg, nullptr, "set servo angle"),
  COMMAND(cmd_servo_set,     "set",  servoIDArg, servoStateArg, nullptr, "set servo to min/max"),
  COMMAND(cmd_servo_get,     "get",  servoIDArg,                nullptr, "get servo angle, min, max"),
  COMMAND(cmd_servo_min,     "min",  servoIDArg, servoAngleArg, nullptr, "set min angle"),
  COMMAND(cmd_servo_max,     "max",  servoIDArg, servoAngleArg, nullptr, "set max angle"),
  COMMAND(cmd_servo_set_all, "aset", servoStateArg,             nullptr, "set all state"),
  COMMAND(cmd_servo_min_all, "amin", servoAngleArg,             nullptr, "set all min"),
  COMMAND(cmd_servo_max_all, "amax", servoAngleArg,             nullptr, "set all max"),
  COMMAND(cmd_servo_swap,    "swap", servoIDArg,                nullptr, "swap min and max"),
  COMMAND(cmd_servo_save,    "save",                            nullptr, "save servo min/max to ROM"),
  COMMAND(cmd_servo_reload, "reload",                          nullptr, ""),
  COMMAND(cmd_servo_help,    "help",                            nullptr, ""),
};

Command eventCommands[] {
  COMMAND(cmd_event_list,         "list",       nullptr, "list enabled events"),
  COMMAND(cmd_event_enable,       "enable",     eventIDArg, nullptr, ""),
  COMMAND(cmd_event_disable,      "disable",    eventIDArg, nullptr, ""),
  COMMAND(cmd_event_disable_all,  "dall",       nullptr, "disable all events"),
  COMMAND(cmd_event_set,          "set",        eventIDArg, eventPinArg, pinStateArg, servoIDArg, servoStateArg, nullptr, ""),
  COMMAND(cmd_event_get,          "get",        eventIDArg, nullptr, ""),
  COMMAND(cmd_event_save,         "save",       nullptr, "save events to ROM"),
  COMMAND(cmd_event_help,         "help",       nullptr, ""),
};

Command commands[] {
  COMMAND(cmd_servo_help, "servo",  servoCommands, ""),
  COMMAND(cmd_event_help, "event",  eventCommands, ""),
  COMMAND(cmd_help,       "help",   nullptr,       ""),
  COMMAND(cmd_export,     "export", nullptr,       "export configuration to commands"),
  COMMAND(cmd_save,       "save",   nullptr,       "save configuration to ROM"),

  COMMAND(cmd_servo_set,  "v",      servoIDArg, servoStateArg, nullptr, "set servo state"),
  COMMAND(cmd_set_debug,  "d",      boolArg,                   nullptr, "enable/disable debug info"),
  COMMAND(cmd_send,       ">",      ArgType::String,           nullptr, "send a string to serial and bluetooth"),

  COMMAND(cmd_mm, "mm", servoIDArg, servoAngleArg, servoAngleArg, nullptr, "servo min max"),
  COMMAND(cmd_event_set, "es", eventIDArg, eventPinArg, pinStateArg, servoIDArg, servoStateArg, nullptr, "event set"),
  COMMAND(cmd_event_enable_from_to, "ee", eventIDArg, eventIDArg, nullptr, "event enable"),
  COMMAND(cmd_event_disable_from_to, "ed", eventIDArg, eventIDArg, nullptr, "event disable"),
  COMMAND(cmd_event_disable_all, "eda", nullptr, "event disable all"),
};

char serialBuffer[48];
char bluetoothBuffer[48];

SerialCommands serialCommands(Serial, commands, sizeof(commands) / sizeof(Command), serialBuffer, sizeof(serialBuffer));
SerialCommands bluetoothCommands(BLUETOOTH, commands, sizeof(commands) / sizeof(Command), bluetoothBuffer, sizeof(bluetoothBuffer));

Servos servos;
Events<servos, MAX_EVENTS, PINS> events;

void setup() {
  SERIAL_BEGIN(9600);

  servos.init();

  events.init();

  SERIAL_PRINTLN(F("Ready!"));
}

void loop() {

  serialCommands.readSerial();
  bluetoothCommands.readSerial();

  events.update();

  servos.update();
}

void cmd_servo_list(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_SERVOS; ++i) {
    sender.getSerial().print(i);
    sender.getSerial().print(F(": "));
    sender.getSerial().print(servos[i].ang[0]);
    sender.getSerial().print(' ');
    sender.getSerial().print(servos[i].ang[1]);
    sender.getSerial().println();
  }
}

void cmd_servo_ang(SerialCommands& sender, Args &args) {
  servos[args[0].getInt()].setAng(args[1].getInt());
  sender.getSerial().println(F("OK"));
}

void cmd_servo_set(SerialCommands& sender, Args &args) {
  servos[args[0].getInt()].setState(args[1].getInt());
  sender.getSerial().println(F("OK"));
}

void cmd_servo_get(SerialCommands& sender, Args &args) {
  sender.getSerial().print(F("ang: "));
  sender.getSerial().print(servos[args[0].getInt()].getAng());
  sender.getSerial().print(F(" min: "));
  sender.getSerial().print(servos[args[0].getInt()].ang[0]);
  sender.getSerial().print(F(" max: "));
  sender.getSerial().println(servos[args[0].getInt()].ang[1]);
}

void cmd_servo_min(SerialCommands& sender, Args &args) {
  servos[args[0].getInt()].ang[0] = args[1].getInt();
  sender.getSerial().println(F("OK"));
}

void cmd_servo_max(SerialCommands& sender, Args &args) {
  servos[args[0].getInt()].ang[1] = args[1].getInt();
  sender.getSerial().println(F("OK"));
}

void cmd_servo_set_all(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_SERVOS; ++i)
    servos[i].setState(args[0].getInt());
  sender.getSerial().println(F("OK"));
}

void cmd_servo_min_all(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_SERVOS; ++i)
    servos[i].ang[0] = args[0].getInt();
  sender.getSerial().println(F("OK"));
}

void cmd_servo_max_all(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_SERVOS; ++i)
    servos[i].ang[1] = args[0].getInt();
  sender.getSerial().println(F("OK"));
}

void cmd_servo_swap(SerialCommands& sender, Args &args) {
  auto id = args[0].getInt();
  auto tmp = servos[id].ang[0];
  servos[id].ang[0] = servos[id].ang[1];
  servos[id].ang[1] = tmp;

  sender.getSerial().println(F("OK"));
}

void cmd_servo_save(SerialCommands& sender, Args &args) {
  servos.save();
  sender.getSerial().println(F("SAVED"));
}

void cmd_servo_reload(SerialCommands& sender, Args &args) {
  servos.init();
  sender.getSerial().println(F("OK"));
}

void cmd_servo_help(SerialCommands& sender, Args &args) {
  sender.listAllCommands(servoCommands, sizeof(servoCommands) / sizeof(Command));
}

void cmd_mm(SerialCommands& sender, Args &args) {
    servos[args[0].getInt()].ang[0] = args[1].getInt();
    servos[args[0].getInt()].ang[1] = args[2].getInt();
    sender.getSerial().println(F("OK"));
}

void cmd_event_list(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_EVENTS; ++i) {
    if (events[i].enabled) {
      sender.getSerial().print(i);
      if (i < 10) sender.getSerial().print(' ');
      sender.getSerial().print(F(": pin:"));
      sender.getSerial().print(events[i].pinID);
      if (events[i].pinID < 10) sender.getSerial().print(' ');
      sender.getSerial().print(F(" state:"));
      sender.getSerial().print(events[i].pinState);
      sender.getSerial().print(F(" servo:"));
      sender.getSerial().print(events[i].servoID);
      if (events[i].servoID < 10) sender.getSerial().print(' ');
      sender.getSerial().print(F(" state:"));
      sender.getSerial().println(events[i].servoState);
    }
  }
  sender.getSerial().print(F("max events: "));
  sender.getSerial().println(MAX_EVENTS);
}

void cmd_event_enable(SerialCommands& sender, Args &args) {
  events[args[0].getInt()].enabled = 1;
  sender.getSerial().println(F("OK"));
}

void cmd_event_enable_from_to(SerialCommands& sender, Args &args) {
  auto e1 = args[0].getInt();
  auto e2 = args[1].getInt();

  if (e1 > e2) {
    auto t = e1;
    e1 = e2;
    e2 = t;
  }

  for (uint16_t i = e1; i <= e2; ++i)
    events[i].enabled = 1;

  sender.getSerial().println(F("OK"));
}

void cmd_event_disable(SerialCommands& sender, Args &args) {
  events[args[0].getInt()].enabled = 0;
  sender.getSerial().println(F("OK"));
}

void cmd_event_disable_from_to(SerialCommands& sender, Args &args) {
  auto e1 = args[0].getInt();
  auto e2 = args[1].getInt();

  if (e1 > e2) {
    auto t = e1;
    e1 = e2;
    e2 = t;
  }

  for (uint16_t i = e1; i <= e2; ++i)
    events[i].enabled = 0;

  sender.getSerial().println(F("OK"));
}

void cmd_event_disable_all(SerialCommands& sender, Args &args) {
  for (uint16_t i = 0; i < MAX_EVENTS; ++i)
    events[i].enabled = 0;
  sender.getSerial().println(F("OK"));
}

void cmd_event_set(SerialCommands& sender, Args &args) {
  uint16_t eventID = args[0].getInt();
  uint16_t pinID = args[1].getInt();
  uint16_t pinState = args[2].getInt();
  uint16_t servoID = args[3].getInt();
  uint16_t servoState = args[4].getInt();

  if (!events.isInputPin(pinID)) {
    sender.getSerial().print(F("ERROR: Invalid pin\nvalid pins: "));
    events.printPins(sender.getSerial());
    sender.getSerial().println();
    return;
  }

  events[eventID].pinID = pinID;
  events[eventID].pinState = pinState;
  events[eventID].servoID = servoID;
  events[eventID].servoState = servoState;
  sender.getSerial().println(F("OK"));
}

void cmd_event_get(SerialCommands& sender, Args &args) {
  uint16_t eventID = args[0].getInt();
  sender.getSerial().print(F("pin: "));
  sender.getSerial().print(events[eventID].pinID);
  sender.getSerial().print(F(" state: "));
  sender.getSerial().print(events[eventID].pinState);
  sender.getSerial().print(F(" servo: "));
  sender.getSerial().print(events[eventID].servoID);
  sender.getSerial().print(F(" state: "));
  sender.getSerial().print(events[eventID].servoState);
}

void cmd_event_save(SerialCommands& sender, Args &args) {
  events.save();
  sender.getSerial().println(F("SAVED"));
}

void cmd_event_help(SerialCommands& sender, Args &args) {
  sender.listAllCommands(eventCommands, sizeof(eventCommands) / sizeof(Command));
}

void cmd_set_debug(SerialCommands& sender, Args &args) {
  events.setDebug(args[0].getInt());
  sender.getSerial().println(F("OK"));
}

void cmd_send(SerialCommands& sender, Args &args) {
  SERIAL_PRINTLN(args[0].getString());
}

void cmd_help(SerialCommands& sender, Args &args) {
  sender.listAllCommands();
}

void cmd_export(SerialCommands& sender, Args &args) {
  sender.getSerial().println();
  sender.getSerial().println(F("---- BEGIN ----"));
  sender.getSerial().println();

  for (uint16_t i = 0; i < MAX_SERVOS; ++i) {
    sender.getSerial().print(F("mm "));
    sender.getSerial().print(i);
    sender.getSerial().print(' ');
    sender.getSerial().print(servos[i].ang[0]);
    sender.getSerial().print(' ');
    sender.getSerial().println(servos[i].ang[1]);
  }

  sender.getSerial().println(F("eda"));

  uint16_t start = 0;
  uint16_t eventCount = 0;
  for (uint16_t i = 0; i < MAX_EVENTS; ++i) {
    if (events[i].enabled) {
      sender.getSerial().print(F("es "));
      sender.getSerial().print(i);
      sender.getSerial().print(' ');
      sender.getSerial().print(events[i].pinID);
      sender.getSerial().print(' ');
      sender.getSerial().print(events[i].pinState);
      sender.getSerial().print(' ');
      sender.getSerial().print(events[i].servoID);
      sender.getSerial().print(' ');
      sender.getSerial().println(events[i].servoState);
      ++eventCount;
    } else {
      if (eventCount > 0) {
        sender.getSerial().print(F("ee "));
        sender.getSerial().print(start);
        sender.getSerial().print(' ');
        sender.getSerial().println(start + eventCount - 1);
      }
      start = i + 1;
      eventCount = 0;
    }
  }

  sender.getSerial().println();
  sender.getSerial().println(F("----- END -----"));
  sender.getSerial().println();
}

void cmd_save(SerialCommands& sender, Args &args) {
  servos.save();
  events.save();
  sender.getSerial().println(F("SAVED"));
}
