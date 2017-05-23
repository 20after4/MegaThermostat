#ifndef globals_h
#define globals_h
#include <math.h>

float fixed_decimal(float val, int dp);

#define DEBUG 1

#if DEBUG
#define debug_print(label, value) \
    do { Serial.print(label); Serial.print(": "); Serial.println(value); } while (0)
#else
#define debug_print(...) /**/
#endif

#endif