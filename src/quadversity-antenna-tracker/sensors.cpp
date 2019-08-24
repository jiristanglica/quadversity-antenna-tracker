#include <Arduino.h>
#include <stdint.h>

#include "sensors.h"
#include "settings.h"

#include "timer.h"


static void writeSerialData();


namespace Sensors {
    static Timer serialLogTimer = Timer(25);

    uint16_t voltageValueReading = 0;
    float calculatedVoltage = 0;

    float thermistorValueReading = 0;
    float calculatedTemperature = 0;

    // Temp reading helper variables
    float R1 = 10000;
    float R2, logR2, T, Tc;
    float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

    // Controls
    bool isFanOn = false;

    void setup() {
    }

    void update() {
        #ifdef USE_VOLTAGE_MONITORING
            voltageValueReading = analogRead(PIN_VBAT_DIVIDED);
            calculatedVoltage = ((voltageValueReading * 5) / 1024.0) * 4.06; // 4.06 is a measured divider
        #endif

        #ifdef USE_TEMP_MONITORING
            thermistorValueReading = analogRead(PIN_SENSOR_THERMO);
            R2 = (R1 * (1023.0 / thermistorValueReading - 1.0));
            logR2 = log(R2);
            T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
            Tc = T - 273.15;
            calculatedTemperature = Tc;

            if (calculatedTemperature > 40 && !isFanOn) {
                isFanOn = true;
                digitalWrite(PIN_AUX_FAN, HIGH);
            }
            if (calculatedTemperature <= 40 && isFanOn) {
                isFanOn = false;
                digitalWrite(PIN_AUX_FAN, LOW);
            }
        #endif

        #ifdef USE_SERIAL_OUT
            // writeSerialData();
        #endif
    }
}

#ifdef USE_SERIAL_OUT

#include "pstr_helper.h"
static void writeSerialData() {
    if (Sensors::serialLogTimer.hasTicked()) {
        Serial.print(PSTR2("Voltage: "));
        Serial.print(Sensors::voltageValueReading);
        Serial.print(PSTR2("  "));
        Serial.print(Sensors::calculatedVoltage);

        Serial.print(PSTR2("  ||  Thermo: "));
        Serial.print(Sensors::thermistorValueReading);
        Serial.print(PSTR2("  "));
        Serial.print(Sensors::calculatedTemperature);

        Serial.println();

        Sensors::serialLogTimer.reset();
    }
}
#endif