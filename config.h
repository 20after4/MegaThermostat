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

#define FAN_PIN 5
#define COOL_PIN 6
#define HEAT_PIN 7

#define ON 0
#define OFF 1
#define ISON on ? 1 : 0
#define ONOFF on ? ON : OFF

#define MIN_HEAT 40
#define MAX_HEAT 75
#define MIN_COOL 70
#define MAX_COOL 80
#define SLEEP_TIME 15000
#define MODE_OFF 0
#define MODE_AUTO 1
#define MODE_ON 5
#define MODE_COOL 6
#define MODE_HEAT 7




#define delayBeforeRestart 60
#define minRunTime 45
#define TEMP_TOLERANCE 3.5
#define TEMP_TOLERANCE_HEAT 1
#define TEMP_TOLERANCE_COOL 0.75

#define METRIC false

/**********************************
 * MySensors configuration
 */

// Define a lower baud rate for Arduino's running on 8 MHz (Arduino Pro Mini 3.3V & SenseBender)
#if F_CPU == 8000000L
#define MY_BAUD_RATE 38400
#else
#define MY_BAUD_RATE 115200
#endif

#define MY_DEBUG 0
#define MY_NODE_ID 2
#define MY_RADIO_NRF24
//#define MY_RF24_ENABLE_ENCRYPTION
//#define MY_RF24_CHANNEL 76



#define MY_RF24_PA_LEVEL RF24_PA_HIGH



/***********************************
 * NodeManager configuration
 */

// if enabled, enable debug messages on serial port
#define DEBUG 0

// if enabled, will load the sleep manager library. Sleep mode and sleep interval have to be configured to make the board sleeping/waiting
#define SLEEP_MANAGER 0
// if enabled, enable the capability to power on sensors with the arduino's pins to save battery while sleeping
#define POWER_MANAGER 0
// if enabled, will load the battery manager library to allow the battery level to be reported automatically or on demand
#define BATTERY_MANAGER 0
// if enabled, allow modifying the configuration remotely by interacting with the configuration child id
#define REMOTE_CONFIGURATION 0
// if enabled, persist the remote configuration settings on EEPROM
#define PERSIST 0

// if enabled, send a SLEEPING and AWAKE service messages just before entering and just after leaving a sleep cycle and STARTED when starting/rebooting
#define SERVICE_MESSAGES 0
// if enabled, a battery sensor will be created at BATTERY_CHILD_ID and will report vcc voltage together with the battery level percentage
#define BATTERY_SENSOR 0

// Enable this module to use one of the following sensors: SENSOR_ANALOG_INPUT, SENSOR_LDR, SENSOR_THERMISTOR, SENSOR_MQ, SENSOR_ML8511
#define MODULE_ANALOG_INPUT 0
// Enable this module to use one of the following sensors: SENSOR_DIGITAL_INPUT
#define MODULE_DIGITAL_INPUT 0
// Enable this module to use one of the following sensors: SENSOR_DIGITAL_OUTPUT, SENSOR_RELAY, SENSOR_LATCHING_RELAY
#define MODULE_DIGITAL_OUTPUT 0
// Enable this module to use one of the following sensors: SENSOR_DHT11, SENSOR_DHT22
#define MODULE_DHT 0
// Enable this module to use one of the following sensors: SENSOR_SHT21
#define MODULE_SHT21 0
// Enable this module to use one of the following sensors: SENSOR_SWITCH, SENSOR_DOOR, SENSOR_MOTION
#define MODULE_SWITCH 0
// Enable this module to use one of the following sensors: SENSOR_DS18B20
#define MODULE_DS18B20 0
// Enable this module to use one of the following sensors: SENSOR_BH1750
#define MODULE_BH1750 0
// Enable this module to use one of the following sensors: SENSOR_MLX90614
#define MODULE_MLX90614 0
// Enable this module to use one of the following sensors: SENSOR_BME280
#define MODULE_BME280 0

#endif
