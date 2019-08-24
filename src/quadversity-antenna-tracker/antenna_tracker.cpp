#include <Arduino.h>
#include <stdint.h>
#include <Servo.h>

#include "antenna_tracker.h"
#include "settings.h"
#include "pstr_helper.h"

#include "timer.h"

#define SERVO_PAN_STEP 10
#define SERVO_TILT_STEP 10


static void writeSerialData();


namespace AntennaTracker {
    static Timer serialLogTimer = Timer(25);
    static Timer joystickReadTimer = Timer(5);

    int joystickNeutralX = 0;
    int joystickNeutralY = 0;
    int joystickCurrentX = 0;
    int joystickCurrentY = 0;

    Servo servoPan;
    Servo servoTilt1;
    Servo servoTilt2;

    int tiltDeg;
    int panDeg;


    void setup() {
        AntennaTracker::joystickNeutralX = analogRead(PIN_JOYSTICK_X);
        AntennaTracker::joystickNeutralY = analogRead(PIN_JOYSTICK_Y);

        servoPan.attach(PIN_SERVO_PAN);
        servoTilt1.attach(PIN_SERVO_TILT1);
        servoTilt2.attach(PIN_SERVO_TILT2);

        panDeg = 0;
        tiltDeg = 45;        

        panHead(panDeg);
        tiltHead(tiltDeg);
    }

    void update() {
        getJoystickPosition();

        moveServos();

        #ifdef USE_SERIAL_OUT
            //writeSerialData();
        #endif
    }

    void getJoystickPosition() {
        if (AntennaTracker::joystickReadTimer.hasTicked()) {

            joystickCurrentX = analogRead(PIN_JOYSTICK_X) - joystickNeutralX;
            joystickCurrentY = analogRead(PIN_JOYSTICK_Y) - joystickNeutralY;

            if (joystickCurrentX > 0) {
                joystickCurrentX = map(joystickCurrentX, 0, 1023-joystickNeutralX, 0, 100);
            } else {
                joystickCurrentX = map(joystickCurrentX, 0, -joystickNeutralX, 0, -100);
            }
            if (joystickCurrentY > 0) {
                joystickCurrentY = map(joystickCurrentY, 0, 1023-joystickNeutralY, 0, 100);
            } else {
                joystickCurrentY = map(joystickCurrentY, 0, -joystickNeutralY, 0, -100);
            }

            AntennaTracker::joystickReadTimer.reset();
        }
    }

    void moveServos() {
        int joystickPan = map(joystickCurrentX, -100, 100, -SERVO_PAN_STEP, SERVO_PAN_STEP);
        int joystickTilt = map(joystickCurrentY, -100, 100, -SERVO_TILT_STEP, SERVO_TILT_STEP);

        //servoPan.writeMicroseconds(ms);
        //servoTilt1.writeMicroseconds(ms);
        //servoTilt2.writeMicroseconds(ms);

        if (panDeg >= -90 && panDeg <= 90) panDeg += joystickPan;
        if (panDeg < -90) panDeg = -90;
        if (panDeg > 90) panDeg = 90;
        panHead(panDeg);

        if (tiltDeg >= 0 && tiltDeg <= 90) tiltDeg += joystickTilt;
        if (tiltDeg < 0) tiltDeg = 0;
        if (tiltDeg > 90) tiltDeg = 90;
        tiltHead(tiltDeg);
    }

    void panHead(int deg) {
        int ms = map(deg, -90, 90, 900, 2100);

        servoPan.writeMicroseconds(ms);
    }

    void tiltHead(int deg) {
        int ms1 = map(deg, 0, 90, 2000, 1000);
        int ms2 = map(deg, 0, 90, 1000, 2000);

        servoTilt1.writeMicroseconds(ms1);
        servoTilt2.writeMicroseconds(ms2);
    }
}

#ifdef USE_SERIAL_OUT

//#include "pstr_helper.h"
static void writeSerialData() {
    if (AntennaTracker::serialLogTimer.hasTicked()) {
        Serial.print(PSTR2("Antenna: "));
        Serial.print(AntennaTracker::joystickCurrentX);
        Serial.print(PSTR2("  ::  "));
        Serial.print(AntennaTracker::joystickCurrentY);
        

        Serial.println();

        AntennaTracker::serialLogTimer.reset();
    }
}
#endif