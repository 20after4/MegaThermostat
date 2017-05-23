#include "temperature.h"

Adafruit_BMP085 mybmp = Adafruit_BMP085();      // Digital Pressure Sensor

int8_t have_temp = -1;
float temperature = 80.0;

float get_temp() {
  if (have_temp == -1) {
    if (!mybmp.begin()) {
      have_temp = 0;
    } else {
      have_temp = 1;
    }
  }
  if (have_temp == 0) {
    return 0.0;
  }

  float temp = mybmp.readTemperature();

  if (isnan(temp)) {
    Serial.println("Failed reading temperature from bmp");
    return temperature;
  }

  if (!METRIC)
  {
    // Convert to fahrenheit
    temp = (temp * 1.8) + 32.0;
  }

  temp = fixed_decimal(temp, 2);
  return temp;
}

float send_temp(float temp=0.0) {
    if (temp == 0.0) {
        temp = get_temp();
    }
    if (temp != temperature) {
        temperature = temp;
        Serial.print("Temperature: ");
        Serial.println(temperature);
        MyMessage msg_temp(1, V_TEMP);
        send(msg_temp.set(temp, 1));
        msg_temp.setSensor(2);
        send(msg_temp);
    }
    return temp;
}