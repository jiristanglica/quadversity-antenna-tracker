#ifndef ANTENNA_TRACKER_H
#define ANTENNA_TRACKER_H


#include <stdint.h>


namespace AntennaTracker {

    void setup();
    void update();

    void getJoystickPosition();
    void moveServos();

    void panHead(int deg);
    void tiltHead(int deg);
}


#endif
