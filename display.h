#ifndef display_h
#define display_h

#include "globals.h"

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
void updateDisplay(uint8_t page_index) {
    float trend;
    lcd.noBlink();
    if (page_index == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(sensor.temperature());
      lcd.print("F");
      trend = sensor.temp_trend();
      if (trend > 0) {
        lcd.print("^");
      } else {
        lcd.print(" ");
      }

      lcd.setCursor(8,0);
      lcd.print(mode_txt);
      lcd.setCursor(0,1);
      if (settings.mode == MODE_COOL) {
        lcd.print(settings.setpoint_cool);
        lcd.print("F");
      } else if (settings.mode == MODE_HEAT) {
        lcd.print(settings.setpoint_heat);
        lcd.print("F");
      } else if (settings.mode == MODE_AUTO) {
        lcd.print(settings.setpoint_cool);
        lcd.print("F");
      } else {
        lcd.print("-=-=-=-");
      }
    } else if (page_index == 1) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(sensor.temperature());
      lcd.print("F ");
      lcd.setCursor(8,0);
      lcd.print(sensor.humidity());
      lcd.print("%");
    } else if (page_index == 2) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(sensor.temperature());
      lcd.print("F ");
      lcd.print(fixed_decimal(sensor.temp_trend()*10, 2));
      lcd.setCursor(0,1);
      lcd.print(fixed_decimal(sensor.pressure(), 1));
      lcd.print("hPa");
      trend = sensor.pressure_trend();
      if (trend > 0.1) {
        lcd.print(" rising");
      } else if (trend < -0.1) {
        lcd.print(" falling");
      } else {
        lcd.print(" stable");
      }
    }
}

#endif