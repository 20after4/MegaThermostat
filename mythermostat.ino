/**
 * The MySensors Arduino library handles the wireless radio link and protocol
 * between your home built sensors/actuators and HA controller of choice.
 * The sensors forms a self healing radio network with optional repeaters. Each
 * repeater and gateway builds a routing tables in EEPROM which keeps track of the
 * network topology allowing messages to be routed to nodes.
 *
 * Created by Henrik Ekblad <henrik.ekblad@mysensors.org>
 * Copyright (C) 2013-2015 Sensnology AB
 * Full contributor list: https://github.com/mysensors/Arduino/graphs/contributors
 *
 * Documentation: http://www.mysensors.org
 * Support Forum: http://forum.mysensors.org
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 *******************************
 *
 * REVISION HISTORY
 * Version 1.0 - Henrik Ekblad
 *
 * DESCRIPTION
 * Example sketch showing how to control physical relays.
 * This example will remember relay state after power failure.
 * http://www.mysensors.org/build/relay
 */


#include "config.h"
#include <MySensors.h>
#include "MyRelay.h"
#include "temperature.h"
#include "globals.h"

bool init_done = false;
uint8_t mode = MODE_AUTO;
String last_state = "Auto";
float setpoint_heat = 65;
float setpoint_cool = 79;

void dwell();
void do_request(uint8_t sensor, uint8_t value_type);
float get_temp();
void control_thermostat();
void initSensors();

Relay rFan(5, "FAN", 0.0, 200.0);
Relay rHeat(6, "HEAT", 0.0, 71.0);
Relay rCool(7, "COOL", 72, 200.0);

MyMessage msg(1, V_STATUS);
//MyMessage msg_setpoint_heat(2, V_HVAC_SETPOINT_HEAT);
//MyMessage msg_setpoint_cool(2, V_HVAC_SETPOINT_COOL);
//MyMessage msg_hvac_mode(2, V_HVAC_FLOW_MODE);
//MyMessage msg_hvac_flow_state(2, V_HVAC_FLOW_STATE);
//MyMessage msg_fan(5, V_STATUS);
//MyMessage msg_heat(6, V_STATUS);
//MyMessage msg_cool(7, V_STATUS);





void dwell() {
  wait(DWELL_TIME);
}

void before()
{
  rHeat.setInverseRelay(&rCool);
  rCool.setInverseRelay(&rHeat);
  rHeat.setFanRelay(&rFan);
  rCool.setFanRelay(&rFan);
}

void setup()
{
  do_request(2, V_HVAC_FLOW_STATE);
  do_request(2, V_HVAC_SETPOINT_COOL);
  do_request(2, V_HVAC_SETPOINT_HEAT);
}

void do_request(uint8_t sensor, uint8_t value_type) {
  request(sensor, value_type);
  dwell();
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Thermostat", "1.1");
  present(1, S_TEMP, "TEMPERATURE");
  present(2, S_HVAC, "HVAC");
  present(5, S_BINARY, "FAN");
  present(6, S_BINARY, "HEAT");
  present(7, S_BINARY, "COOL");
}



void loop()
{
  if (!init_done) {
    initSensors();
    init_done = true;
  }
  float temperature = send_temp();

  rHeat.refresh(temperature);
  dwell();
  rCool.refresh(temperature);
  dwell();
  rFan.refresh(temperature);
  //control_thermostat();
  wait(10000);
}


void initSensors() {
  send_temp();
  initHVAC(2);
  initBoolSensor(5);
  initBoolSensor(6);
  initBoolSensor(7);
}

void initHVAC(uint8_t sensorId) {

  msg = MyMessage(sensorId, V_HVAC_FLOW_STATE);
  send(msg.set(last_state));
  wait(100);
  msg = MyMessage(sensorId, V_HVAC_FLOW_MODE);
  send(msg.set(mode));
  wait(100);
  msg = MyMessage(sensorId, V_HVAC_SETPOINT_HEAT);
  send(msg.set(setpoint_heat, 1));
  wait(100);
  msg = MyMessage(sensorId, V_HVAC_SETPOINT_COOL);
  send(msg.set(setpoint_cool, 1));
  wait(100);
  send_temp();
  wait(100);
}

void initBoolSensor(uint8_t sensorId) {
  msg = MyMessage(sensorId, V_STATUS);
  send(msg.set(0));
  wait(1000);
  //wait(2000, C_SET, V_STATUS);
}

void receive(const MyMessage &message)
{
  if (message.isAck()) {
     return;
  }
  if (message.getCommand() != 1) {
    Serial.print("getCommand = ");
    Serial.println(message.getCommand());
    //only interested in set commands...
    return;
  }

	// Change relay state
  if (message.sensor == 2) {

  	if (message.type==V_HVAC_SETPOINT_HEAT) {
      setpoint_heat = message.getFloat();
      if (setpoint_heat + TEMP_TOLERANCE >= setpoint_cool) {
        setpoint_heat = setpoint_cool - TEMP_TOLERANCE;
      }
      debug_print("set heat", setpoint_heat);
      rHeat.setOn(setpoint_heat - TEMP_TOLERANCE_HEAT);
      rHeat.setOff(setpoint_heat + TEMP_TOLERANCE_HEAT);
      wait(100);
      msg = MyMessage(2, V_HVAC_SETPOINT_HEAT);
      send(msg.set(setpoint_heat, 1));
  	} else if (message.type==V_HVAC_SETPOINT_COOL) {
      setpoint_cool = message.getFloat();
      if (setpoint_cool - TEMP_TOLERANCE <= setpoint_heat) {
        setpoint_cool = setpoint_heat + TEMP_TOLERANCE;
      }
      wait(100);
      debug_print("set cool", setpoint_cool);
      rCool.setOn(setpoint_cool + TEMP_TOLERANCE_COOL);
      rCool.setOff(setpoint_cool - TEMP_TOLERANCE_COOL);
      msg = MyMessage(2, V_HVAC_SETPOINT_COOL);
      send(msg.set(setpoint_cool, 1));
    } else if (message.type == V_HVAC_FLOW_STATE) {
      String new_state = message.getString();

      if (last_state != new_state) {
        last_state = new_state;
        Serial.print("new state=");
        Serial.println(last_state);
        msg = MyMessage(2, V_STATUS);
        if (new_state[0] == 'C') {
          debug_print("mode", "cool");
          mode = MODE_COOL;
          rHeat.setMode(MODE_OFF);
          rHeat.stop();
          rFan.setMode(MODE_AUTO);
          rCool.setMode(MODE_AUTO);
          rCool.run();
          send(msg.set(true));
        } else if (new_state[0] == 'H') {
          debug_print("mode", "heat");
          mode = MODE_HEAT;
          rFan.setMode(MODE_AUTO);
          rCool.setMode(MODE_OFF);
          rCool.stop();
          rHeat.setMode(MODE_AUTO);
          rHeat.run();
          send(msg.set(true));
        } else if (new_state[0] == 'A') {
          debug_print("mode", "auto");
          mode = MODE_AUTO;
          rFan.setMode(mode);
          rHeat.setMode(mode);
          rCool.setMode(mode);
        } else if (new_state[0] == 'O') {
          debug_print("mode", "off");
          mode = MODE_OFF;
          rFan.setMode(mode);
          rHeat.setMode(mode);
          rCool.setMode(mode);
          wait(10);
          send(msg.set(false));
        }
        wait(10);
        debug_print("new mode", mode);
        msg = MyMessage(2, V_HVAC_FLOW_STATE);
        send(msg.set(new_state));
      }
    } else if (message.type == V_TEMP) {
      //float temperature = message.getFloat();
      //msg = MyMessage(1, V_TEMP);
      //send(msg.set(temperature, 2));
    } else if (message.type == V_HVAC_FLOW_MODE) {

    }
  }
}
