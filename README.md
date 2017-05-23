# MegaThermostat

This arduino sketch implements a basic home thermostat controller which connects to the wireless MySensors network for remote control and monitoring.

## Hardware:

* Arduino Mega2560
* BMP180 berometric pressure + temperature sensor: https://www.adafruit.com/product/1603
* DHT22 Temperature + Humidity sensor
* NRF20L01+ radio for remote control
* Generic 4 channel relay module
* Generic 16x2 LCD + Keypad shield.

## Libraries:

* [MySensors](https://www.mysensors.org/)
* [Adafruit Unified Sensor Driver](https://github.com/adafruit/Adafruit_Sensor)
* [Adafruit_BMP085_U](https://github.com/adafruit/Adafruit_BMP085_Unified)
* [Adafruit_DHT_Unified](https://github.com/adafruit/Adafruit_DHT_Unified)

## Features:

Similar to the most basic general-purpose home thermostats, this can control heating, cooling, and fan operation.
The main features of the MySensors API for S_HVAC (Thermostat/HVAC device) are also implemented.

Standard HVAC control features:

 * Min and Max runtime for each relay
 * Delayed fan shutoff
 * Mutual exclusion between heat and cool relays
 * Enforced min and max temperature ranges
 * Prevents overlap between heat and cool ranges

Remote control:

 * Temperature range and operation mode can be controlled via the MySensors gateway.

Monitoring:

 * Transmits Temperature, Humidity and Barometric Pressure to the MySensors gateway.

Other stuff worth mentioning:

 * Averages reading from two temperature sensors
 * Modes: Off, Fan, Heat, Cool, Auto
 * Separate, configurable heat and cool tolerances control how far temperature is allowed to vary from the setpoint.
 * When set to auto there are two setpoints
  * low-temperature limit triggers heating
  * high-temperature limit triggers cooling.
