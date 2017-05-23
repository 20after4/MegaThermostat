#ifndef temperature_h
#define temperature_h

#include <Adafruit_BMP085.h>
#include "config.h"
#include "globals.h"
#include <core/MySensorsCore.h>
#include <core/MyHwAVR.h>

float send_temp(float temp=0.0);
float get_temp();

#endif