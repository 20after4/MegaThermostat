
#include "config.h"
#include <MyConfig.h>
#include <MySensors.h>
#include "MyRelay.h"
#include <EEPROM.h>
#include "globals.h"
#include "multisensor.h"


MySettings settings_saved = {
  MODE_AUTO,
  65.0,
  75.0
};

MySettings settings =  {
  MODE_AUTO,
  65.0,
  75.0
};

Relay rFan(FAN_PIN, 5,  "FAN",  0.0, 200.0);
Relay rHeat(HEAT_PIN, 6, "HEAT", settings.setpoint_heat,
            settings.setpoint_heat + TEMP_TOLERANCE_HEAT);
Relay rCool(COOL_PIN, 7, "COOL", settings.setpoint_cool,
            settings.setpoint_cool - TEMP_TOLERANCE_COOL);

MyMessage msg(1, V_STATUS);

String mode_txt = "AUTO";
String last_state = "Auto";
long int awake_time = 0;
uint8_t tick=0;
long int tick_time = 0;

#include "display.h"

void setHeat(float setpoint) {
  if (setpoint < MIN_HEAT || setpoint > MAX_HEAT) {
    debug_print("setpoint_heat out of range: ", setpoint);
    return;
  }
  if (setpoint > settings.setpoint_cool - TEMP_TOLERANCE) {
    debug_print("setpoint_cool must be warmer than setpoint_heat + ", TEMP_TOLERANCE);
    setpoint = settings.setpoint_cool
             - TEMP_TOLERANCE
             - TEMP_TOLERANCE_HEAT
             - TEMP_TOLERANCE_COOL;
  }
  float on = setpoint - TEMP_TOLERANCE_HEAT;
  float off = setpoint + TEMP_TOLERANCE_HEAT;
  rHeat.setOn(on);
  rHeat.setOff(off);
  settings.setpoint_heat = setpoint;
  msg = MyMessage(2, V_HVAC_SETPOINT_HEAT);
  send(msg.set(settings.setpoint_heat, 1));
}

void setCool(float setpoint) {
  if (setpoint < MIN_COOL || setpoint > MAX_COOL) {
    debug_print("setpoint_cool out of range: ", setpoint);
    return;
  }
  if (setpoint < settings.setpoint_heat + TEMP_TOLERANCE) {
    debug_print("setpoint_cool must be warmer than setpoint_heat + ", TEMP_TOLERANCE);
    setpoint = settings.setpoint_heat
             + TEMP_TOLERANCE
             + TEMP_TOLERANCE_COOL
             + TEMP_TOLERANCE_HEAT;
  }
  float on = setpoint + TEMP_TOLERANCE_COOL;
  float off = setpoint - TEMP_TOLERANCE_COOL;
  rCool.setOn(on);
  rCool.setOff(off);
  settings.setpoint_cool = setpoint;
  msg = MyMessage(2, V_HVAC_SETPOINT_COOL);
  send(msg.set(settings.setpoint_cool, 1));
}

void setMode(uint8_t newmode) {
  uint8_t mode = newmode;


  if (mode > 4) {
    mode = 0;
  }
  msg = MyMessage(2, V_HVAC_FLOW_STATE);
  MyMessage msg2 = MyMessage(2, V_HVAC_FLOW_MODE);
  msg2.set("PeriodicOn");
  settings.mode = mode;
  switch (mode)               // depending on which button was pushed, we perform an action
  {
    case MODE_OFF:
    {
      rFan.setMode(settings.mode);
      rHeat.setMode(settings.mode);
      rCool.setMode(settings.mode);
      rCool.stop();
      rHeat.stop();
      rFan.stop();
      mode_txt = "OFF";
      send(msg.set("Off"));
      send(msg2.set("Auto"));
      break;
    }
    case MODE_AUTO:
    {
      rFan.setMode(settings.mode);
      rHeat.setMode(settings.mode);
      rCool.setMode(settings.mode);
      mode_txt = "AUTO";
      send(msg.set("AutoChangeOver"));
      send(msg2.set("Auto"));
      break;
    }
    case MODE_ON:
    {
      rFan.setMode(MODE_ON);
      rFan.run();
      rHeat.setMode(MODE_AUTO);
      rCool.setMode(MODE_AUTO);
      mode_txt = "FAN";
      send(msg.set("AutoChangeOver"));
      send(msg2.set("ContinuousOn"));
      break;
    }
    case MODE_HEAT:
    {
      rFan.setMode(MODE_AUTO);
      rCool.setMode(MODE_OFF);
      rCool.stop();
      rHeat.setMode(MODE_AUTO);
      rHeat.run();
      mode_txt = "HEAT";
      send(msg.set("HeatOn"));
      send(msg2);
      break;
    }
    case MODE_COOL:
    {
      rHeat.setMode(MODE_OFF);
      rHeat.stop();
      rFan.setMode(MODE_AUTO);
      rCool.setMode(MODE_AUTO);
      rCool.run();
      mode_txt = "COOL";
      send(msg.set("CoolOn"));
      send(msg2);
      break;
    }
  }
  debug_print("new mode", mode_txt);
}

void before()
{
  rHeat.setInverseRelay(&rCool);
  rCool.setInverseRelay(&rHeat);
  rHeat.setFanRelay(&rFan);
  rCool.setFanRelay(&rFan);
}

void do_request(uint8_t sensor, uint8_t value_type) {
  request(sensor, value_type);
  dwell();
}

void presentation()
{
	// Send the sketch version information to the gateway and Controller
	sendSketchInfo("Thermostat", "1.2");
  present(1, S_TEMP, "Temperature");
  present(2, S_HVAC, "HVAC");
  present(3, S_HUM, "Humidity");
  present(5, S_BINARY, "Fan");
  present(6, S_BINARY, "Heat");
  present(7, S_BINARY, "Cool");
}

void setup() {
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.print("Loading...");

  EEPROM.get(0, settings);
  if (isnan(settings.setpoint_cool)
   || isnan(settings.setpoint_heat)
   || isnan(settings.mode)
   || settings.mode < 0
   || settings.mode > 4) {
     settings = settings_saved;
     EEPROM.put(0, settings);
   } else {
    settings_saved = settings;
   }
   setMode(settings.mode);
   setHeat(settings.setpoint_heat);
   setCool(settings.setpoint_cool);

   do_request(2, V_HVAC_FLOW_STATE);
   do_request(2, V_HVAC_FLOW_MODE);
   do_request(2, V_HVAC_SETPOINT_COOL);
   do_request(2, V_HVAC_SETPOINT_HEAT);

   sensor.begin();
   sensor.refresh(false);
   last_key_time = millis();
   awake_time = millis();
}

float send_temp(float temp=0.0) {
    if (temp == 0.0) {
        temp = sensor.temperature();
    }
    if (temp != temperature) {
        temperature = temp;
        Serial.print("Temperature: ");
        Serial.println(temperature);
        MyMessage msg_temp(1, V_TEMP);
        send(msg_temp.set(temp, 1));
        msg_temp.setSensor(2);
        dwell();
        send(msg_temp);
    }
    return temp;
}

float send_humidity() {
  static float last_humidity = 0;
  float humidity = sensor.humidity();
  if (humidity != last_humidity) {
    msg = MyMessage(3, V_HUM);
    send(msg.set(humidity,1));
    last_humidity = humidity;
  }
}


void initHVAC(uint8_t sensorId) {
  setMode(settings.mode);
  msg = MyMessage(sensorId, V_HVAC_SETPOINT_HEAT);
  send(msg.set(settings.setpoint_heat, 1));
  wait(10);
  msg = MyMessage(sensorId, V_HVAC_SETPOINT_COOL);
  send(msg.set(settings.setpoint_cool, 1));
  wait(10);
  send_temp();
  wait(10);
}

void initBoolSensor(uint8_t sensorId) {
  msg = MyMessage(sensorId, V_STATUS);
  send(msg.set(0));
  wait(100);
  //wait(2000, C_SET, V_STATUS);
}


void initSensors() {
  initHVAC(2);
  initBoolSensor(5);
  initBoolSensor(6);
  initBoolSensor(7);
}

int read_LCD_buttons();

int getkey() {
  int key = read_LCD_buttons();
  if (key == btnNONE) {
    last_key = btnNONE;
    return btnNONE;
  }
  if ((last_key != btnNONE && key != btnNONE)||last_key == key) {
    if (millis() - last_key_time < 500) {
      return btnNONE;
    } else if (key == btnSELECT) {
      return btnNONE;
    }
  }

  // if (millis() - awake_time > DISPLAY_SLEEP_TIME) {
  //   lcd.display();
  //   key = btnNONE;
  // }

  last_key_time = millis();
  last_key = key;
  awake_time = last_key_time;
  return key;
}

int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;
 return btnNONE;  // when all others fail, return this...
}

void cls(bool right=false) {
  uint8_t i = 0;
  for (i = 0; i < 16; i++) {
    if (right) {
      lcd.scrollDisplayRight();
    } else {
      lcd.scrollDisplayLeft();
    }
    wait(5);
  }
  lcd.clear();
}

uint8_t page_index = 0;
uint8_t loopcounter = 0;
uint8_t refresh_counter = 0;

void loop() {
  if (millis() - tick_time >= 1000) {
    tick_time = millis();
    tick++;
  } else {
    return;
  }
  if (tick > 60) {
    tick = 1;
  }

  if (!init_done) {
    initSensors();
    init_done = true;
  }
  int lcd_key =  getkey();
  switch (lcd_key)               // depending on which button was pushed, we perform an action
  {
    case btnRIGHT:
      {
        if (page_index < 2) {
          page_index++;
          loopcounter = 1;
          cls(false);
        }
      break;
      }
    case btnLEFT:
      {
        if (page_index > 0) {
          page_index--;
          loopcounter = 1;
          cls(true);
        }
      break;
      }
    case btnUP:
      {
        settings.setpoint_cool += 0.5;
        lcd.blink();
        lcd.setCursor(0,1);
        lcd.print(settings.setpoint_cool);
        lcd.print("F");
        loopcounter = 2;
        break;
      }
    case btnDOWN:
      {
        lcd.blink();
        settings.setpoint_cool -= 0.5;
        lcd.setCursor(0,1);
        lcd.print(settings.setpoint_cool);
        lcd.print("F");
        loopcounter = 2;
        break;
      }
    case btnSELECT:
      {
        lcd.blink();
        lcd.cursor();
        setMode(settings.mode + 1);
        page_index = 0;
        lcd.setCursor(7,0);
        lcd.print("      ");
        lcd.setCursor(7,0);
        lcd.print(mode_txt);
        loopcounter = 2;
        wait(100);
        lcd.noBlink();
        break;
      }
      case btnNONE:
      {
      break;
      }
  }
  if (page_index > 2) {
    page_index = 2;
  }

  if (tick == 59 || (last_key_time > 0 && millis() - last_key_time > 5000)) {
    last_key_time = 0;
    page_index = 0;
    loopcounter = 1;
    if (settings_saved != settings) {
      debug_print("save settings", settings.to_string());
      EEPROM.put(0, settings);
      settings_saved = settings;
      msg = MyMessage(2, V_HVAC_SETPOINT_COOL);
      setCool(settings.setpoint_cool);
      setHeat(settings.setpoint_heat);
      send(msg.set(settings.setpoint_cool, 1));
      last_refresh_time -= 20000;
    }
  }

  if (loopcounter > 200) {
    loopcounter = 0;
  }
  if (tick % 10 == 1) {
    debug_print("refresh", last_refresh_time);
    last_refresh_time = millis();
    sensor.refresh(true);
    temperature = sensor.temperature();
    rHeat.refresh(temperature);
    rCool.refresh(temperature);
    rFan.refresh(temperature);
    updateDisplay(page_index);
  }
  if (tick == 2) {
    send_temp();
    send_humidity();
    updateDisplay(page_index);
  } else if (tick % 5 == 1 || loopcounter == 1) {
    updateDisplay(page_index);
  }
  loopcounter++;
  wait(100);
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
      setHeat(setpoint_heat);
  	} else if (message.type==V_HVAC_SETPOINT_COOL) {
      setpoint_cool = message.getFloat();
      setCool(setpoint_cool);
    } else if (message.type == V_HVAC_FLOW_STATE) {
      String new_state = message.getString();

      if (last_state != new_state) {
        last_state = new_state;
        Serial.print("new state=");
        Serial.println(last_state);

        if (new_state[0] == 'C') {
          setMode(MODE_COOL);
        } else if (new_state[0] == 'H') {
          setMode(MODE_HEAT);
        } else if (new_state[0] == 'A') {
          setMode(MODE_AUTO);
        } else if (new_state[0] == 'O') {
          setMode(MODE_OFF);
        }
      }
    } else if (message.type == V_HVAC_FLOW_MODE) {
      // String new_mode = message.getString();
      // if (new_mode[0] == 'C') {
      //   setMode(MODE_ON);
      // }
    }
  }
}