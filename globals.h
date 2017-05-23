#ifndef globals_h
#define globals_h

#include "config.h"
#include <LiquidCrystal.h>
#include <core/MySensorsCore.h>

float fixed_decimal(float val, int dp);

void dwell() {
  wait(DWELL_TIME);
}

struct MySettings {
  uint8_t mode;
  float setpoint_heat;
  float setpoint_cool;

  String to_string() {
    String s = String('{');
    return s + mode + ", " + setpoint_heat + ", " + setpoint_cool + " };";
  }
};

inline bool operator==(const MySettings& lhs, const MySettings& rhs)
{
  return lhs.mode == rhs.mode
      && lhs.setpoint_heat == rhs.setpoint_heat
      && lhs.setpoint_cool == rhs.setpoint_cool;
}
bool operator!=(const MySettings& lhs, const MySettings& rhs)
{
    return ! (lhs == rhs);
}


#define DEBUG 1

#if DEBUG
#define debug_print(label, value) \
    do { Serial.print(label); Serial.print(": "); Serial.println(value); } while (0)
#else
#define debug_print(...) /**/
#endif

#endif