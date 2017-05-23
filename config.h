#ifndef config_h
#define config_h

#include <Arduino.h>

/**********************************
 * Sketch configuration
 */

#define SKETCH_NAME "mythermostat"
#define SKETCH_VERSION "1.1"

#define RELAY_1  5  // Arduino Digital I/O pin number for first relay (second on pin+1 etc)
#define LAST_RELAY 8 // Total number of attached relays

#define DWELL_TIME 10
#define COOLDOWN_TIME 360000

#define FAN_PIN 26
#define COOL_PIN 28
#define HEAT_PIN 30

#define ON 0
#define OFF 1
#define ISON on ? 1 : 0
#define ONOFF on ? ON : OFF

#define MIN_HEAT 40
#define MAX_HEAT 75
#define MIN_COOL 60
#define MAX_COOL 100
#define SLEEP_TIME 15000
#define MODE_OFF 0
#define MODE_AUTO 1
#define MODE_ON 2
#define MODE_COOL 3
#define MODE_HEAT 4


#define delayBeforeRestart 60
#define MIN_RUNTIME 45
#define MAX_RUNTIME 1200
#define TEMP_TOLERANCE 5
#define TEMP_TOLERANCE_HEAT 0.75
#define TEMP_TOLERANCE_COOL 0.5

#define METRIC false

#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

#define DISPLAY_SLEEP_TIME 60000

int adc_key_in  = 0;
float temperature = 0.0;
float setpoint  = 70.0;
float setpoint_saved = 70.0;
float setpoint_cool = 79.0;
float setpoint_heat = 60.0;
bool init_done = false;
int  last_key    = 0;
long last_key_time = 0;
long last_refresh_time = 0;



/**********************************
 * MySensors configuration
 */

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#else
#define MY_BAUD_RATE 115200
#endif


#define MY_RADIO_NRF24 true
#define MY_DEBUG true
#define MY_NODE_ID 2
#define MY_PARENT_NODE_ID 0
#define MY_PARMY_PARENT_NODE_IS_STATIC true
#define MY_RF24_PA_LEVEL RF24_PA_HIGH
#define MY_RF24_CE_PIN 49
#define MY_RF24_CS_PIN 53
#undef MY_RX_MESSAGE_BUFFER_FEATURE

#endif
