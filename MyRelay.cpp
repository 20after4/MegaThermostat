#include "config.h"
#include "MyRelay.h"
/*
   Device class
*/



Relay::Relay(uint8_t pin, uint8_t sensor_num, String a_name, float setpoint_on, float setpoint_off)
  :IODevice(pin) {
  _pin = pin;
  digitalWrite(_pin, OFF);
  pinMode(_pin, OUTPUT);

  name = a_name;
  _setpoint_on = setpoint_on;
  _setpoint_off = setpoint_off;

  MyMessage my_message(sensor_num, V_STATUS);
  msgClass = my_message;
}

void Relay::onBefore(){
}

void Relay::onSetup(){
}

void Relay::onLoop(){
}

void Relay::onReceive(const MyMessage & message) {
}

void Relay::forceState(uint8_t forced_state) {
  if (state == forced_state) {
    return;
  }
  state = forced_state;
  send(msgClass.set(state == ON ? 1 : 0));
  debug_print(name, state);
  digitalWrite(_pin, state);
  lastChange = millis();
}

void Relay::refresh(float temperature) {
  _temperature = temperature;
  if (runMode == MODE_OFF) {
    forceState(OFF);
    return;
  }
  if (runMode == MODE_ON) {
    forceState(ON);
    return;
  }

  unsigned long ts = millis();
  if (holdUntil > ts) {
    return;
  }
  unsigned int secondsSinceLastChange = (ts - lastChange) / 1000;

  if (state == ON && secondsSinceLastChange > maxRunTime) {
    forceState(OFF);
    pendingState = -1;
    return;
  }

  if (pendingState == ON && secondsSinceLastChange < delayBeforeRestart) {
    return;
  } else if (pendingState == OFF && secondsSinceLastChange < minRunTime) {
    return;
  }

  if (pendingState > -1 && pendingState != state) {
    forceState(pendingState);
    pendingState = -1;
  } else if (_setpoint_on > 0 && _setpoint_on > _setpoint_off) {
    if (temperature > _setpoint_off && temperature >= _setpoint_on) {
      run();
    } else if (temperature <= _setpoint_off) {
      stop();
    }
  } else if (_setpoint_on > 0 && _setpoint_on < _setpoint_off) {
    if (temperature >= _setpoint_off) {
      stop();
    } else if (temperature <= _setpoint_on) {
      run();
    }
  } else if (_setpoint_on > 0) {
    stop();
  }
}