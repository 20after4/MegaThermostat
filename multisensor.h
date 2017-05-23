#ifndef multisensor_h
#define multisensor_h

#define ALTITUDE 390;

#include "globals.h"
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Adafruit_BMP085_U.h>

class Multisensor {

    private:
        void rotate_array(float *arr, float newval) {
            uint8_t i = 0;
            for (i=0; i<9; i++) {
                arr[i+1] = arr[i];
            }
            arr[0] = newval;
        }

        /**
         * evaluate an array of historical sensor values to
         * determine the direction and amplitude of the trend.
         * @return greater than 0 if the value is climbing, 0 if stable,
         *         less than 0 if the value is falling..
         */
        float trend(float *arr, uint8_t samples=4) {
            if (samples > 10) {
                samples = 10;
            }
            uint8_t i = 0;
            float trends[10];
            float cur_trend = 0;
            uint8_t cnt = 0;
            float cur = arr[0];
            float recent_max = cur;
            float recent_min = cur;

            for (i=0; i < samples; i++) {
                if (arr[i] != 0) {
                    cur_trend += arr[i];
                    cnt++;
                    if (arr[i] > recent_max) {
                        recent_max = arr[i];
                    } else if (arr[i] < recent_min) {
                        recent_min = arr[i];
                    }
                }
            }
            if (!cnt) {
                return 0.0;
            }
            if (cur == recent_max) {
                return cur - recent_min;
            } else if (cur == recent_min) {
                return cur - recent_max;
            } else {
                return (cur) - (cur_trend / cnt);
            }
        }

    public:
        DHT_Unified* dht;
        Adafruit_BMP085_Unified* bmp;
        sensor_t sensor;
        uint32_t delayMS = 100;

        float temp_log[10];
        float humi_log[10];
        float baro_log[10];

        float temp_f = 0.0;
        float temp2 = 0.0;
        float baro = 0.0;
        float rel_humidity = 0.0;

        Multisensor(uint8_t pin) {
            dht = new DHT_Unified(pin, DHT22);
            bmp = new Adafruit_BMP085_Unified(10180);
        }

        bool begin() {
            bool result = true;
            dht->begin();
            dht->temperature().getSensor(&sensor);
            delayMS = sensor.min_delay / 1000;

            if (!bmp->begin()) {
                result = false;
            }
            return result;
        }

        float temperature() {
            return temp_f;
        }

        float temp_trend() {
            return trend(temp_log, 9);
        }

        float humidity() {
            return rel_humidity;
        }

        float pressure() {
            return baro;
        }

        float pressure_trend() {
            return trend(baro_log, 9);
        }

        float temperature2() {
            return temp2;
        }

        bool refresh(bool rotate) {
            bool err = false;
            delay(delayMS);
            sensors_event_t event;
            dht->temperature().getEvent(&event);
            if (!isnan(event.temperature)) {
                temp_f = (event.temperature * 1.8) + 32.0;
                temp_f = fixed_decimal(temp_f, 2);
            } else {
                err = true;
            }

            dht->humidity().getEvent(&event);
            if (!isnan(event.relative_humidity)) {
                rel_humidity = event.relative_humidity;
                if (rotate) {
                    rotate_array(humi_log, rel_humidity);
                }
            } else {
                err = true;
            }

            bmp->getEvent(&event);
            bmp->getTemperature(&temp2);
            if (!isnan(temp2)) {
                temp2 = (temp2 * 1.8) + 32.0;
                temp_f = (temp_f + temp2) / 2;
                temp_f = fixed_decimal(temp_f, 2);
            }
            if (rotate) {
                rotate_array(temp_log, temp_f);
            }

            if (!isnan(event.pressure) && event.pressure != baro) {
                baro = event.pressure;
                if (rotate) {
                    rotate_array(baro_log, baro);
                }
            }
            return !err;
        }
};

Multisensor sensor(40);
#endif