
#ifndef myrelay_h
#define myrelay_h

#include <Arduino.h>
#include "config.h"
#include <MySensors.h>
//#include <core/MySensorsCore.h>
//#include <core/MyHwAVR.h>
#include "globals.h"

class IODevice {
  public:
    uint8_t _pin = 0;

    int getId() {
      return _pin;
    }

    IODevice(uint8_t pin) {
      _pin = pin;
    }
};

class InputDevice: public IODevice {
  public:
    InputDevice(uint8_t pin, bool pullup=true):IODevice(pin) {
      _pin = pin;
      pinMode(_pin, pullup ? INPUT_PULLUP : INPUT);
    }

    uint8_t read() {
      return digitalRead(_pin);
    }
};

class Relay: public IODevice {
  public:
    unsigned long holdUntil = 0;
    unsigned long lastChange = 0;
    int maxRunTime = MAX_RUNTIME;
    int minRunTime = MIN_RUNTIME;
    String name;
    uint8_t state = OFF;
    int8_t pendingState = -1;
    Relay *inverseRelay = NULL;
    Relay *fanRelay = NULL;
    float _setpoint_on = 0.0;
    float _setpoint_off = 0.0;
    MyMessage msgClass;
    float _temperature;
    uint8_t runMode = MODE_AUTO;

    Relay(uint8_t pin, uint8_t sensor_num, String a_name, float setpoint_on,
         float setpoint_off);
    void refresh(float temperature);
    void setState(uint8_t state) { pendingState = state; }
    void setOn(float setpoint_on) {
      _setpoint_on = setpoint_on;
      debug_print("setpoint_on", setpoint_on);
    }
    void setOff(float setpoint_off) {
      _setpoint_off = setpoint_off;
      debug_print("setpoint_off", setpoint_off);
    }
    void setMode(uint8_t new_mode) { runMode = new_mode; }
    void setInverseRelay(Relay* inverse) { inverseRelay = inverse; }
    void setFanRelay(Relay* fan) { fanRelay = fan; }
    void holdStateUntil(unsigned long until) { holdUntil = until; }
    void holdStateFor(uint16_t seconds) {
      holdUntil = millis() + (seconds * 1000);
    }
    void Relay::forceState(uint8_t);
    bool isRunning() { return state == ON; }
    bool isPending() { return pendingState > -1; }
    void onBefore();
    void onSetup();
    void onLoop();
    void onReceive(const MyMessage & message);
    void run() {
      if (runMode == MODE_OFF)
        return;
      if (fanRelay != NULL && fanRelay->runMode == MODE_OFF)
        return;

      setState(ON);
      if (fanRelay) {
        fanRelay->setState(ON);
        fanRelay->holdStateUntil(millis());
      }
      if (inverseRelay) {
        inverseRelay->forceState(OFF);
        inverseRelay->setMode(runMode == MODE_AUTO ? MODE_AUTO : MODE_OFF);
      }
    }

    void stop() {
      setState(OFF);
      if (fanRelay && fanRelay->runMode == MODE_AUTO) {
        fanRelay->holdStateFor(60);
        fanRelay->setState(OFF);
      }
    }

};

class TempControlledRelay: public Relay {
};

class DeviceManager {
  public:
    IODevice* _devices[255] = {0};
    // attach a built-in or custom device to this manager
    int registerDevice(IODevice* device) {
      // add the device to the array of registered devices
      _devices[device->getId()] = device;
      // return the child_id
      return device->getId();
    }

    // return a device given its index
    IODevice* get(int id) {
      // return a pointer to the device from the given child_id
      return _devices[id];
    }

};

#endif