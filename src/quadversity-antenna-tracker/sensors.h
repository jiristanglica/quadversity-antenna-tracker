#ifndef SENSORS_H
#define SENSORS_H


#include <stdint.h>


namespace Sensors {
    // Voltage readings
    extern uint16_t voltageValueReading;
    extern float calculatedVoltage;

    // Temperature readings
    extern float thermistorValueReading;
    extern float calculatedTemperature;

    void setup();
    void update();
}


#endif
