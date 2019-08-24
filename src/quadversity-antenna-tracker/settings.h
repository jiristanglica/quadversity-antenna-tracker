/*
 * Setings file by Shea Ivey

The MIT License (MIT)

Copyright (c) 2015 Shea Ivey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

//#define USE_SCREENSAVER

// === Display Module ==========================================================
//
// You can choose one display module only. Only 128x64 OLED displays are
// supported.
//
// =============================================================================

// SSH1106 needs https://github.com/badzz/Adafruit_SH1106 installed.
//#define SH1106

#define OLED_128x64_ADAFRUIT_SCREENS

// Enable this if your screen is upside down.
//#define USE_FLIP_SCREEN

#define OLED_ADDRESS 0x3C // I2C address for display (0x3C or 0x3D, usually)

// === Receiver Module =========================================================
//
// Select which receiever you are using. Required for time tuning.
//
// =============================================================================

#define RX5808
//#define RX5880

// Can enable this to powerdown the audio blocks on the RX58xx if you don't
// need it. Save a tiny bit of power, make your videos less noisy.
//
// WARNING: Makes RSSI act a little weird.
//#define DISABLE_AUDIO

// === Features ===============================================================
//
// Keep in mind that there is limited program memory. Only enable the features
// you need.
//
// =============================================================================

//#define USE_DIVERSITY
#define USE_DUAL_DIVERSITY

// Enable this to switch receivers much faster. This uses the port registers
// to switch rather than the Arduino helper functions.
//
// WARNING: You can only use this if your receivers are on the same port.
// Arduino/ATmega has 3 ports: PORTD, PORTB, and PORTC. The "default" hardware
// configuration uses pins A0 and A1 (PORTC), so if you're using reasonably
// standard hardware then this should be fine.
//
// PORTC: Pins A0-A7
// PORTD: 0-7
// PORTB: 8-13
#define USE_DIVERSITY_FAST_SWITCHING

//#define USE_IR_EMITTER
#define USE_SERIAL_OUT // Not compatible with IR emitter.

// You can use any of the arduino analog pins to measure the voltage of the
// battery. See additional configuration below.
#define USE_VOLTAGE_MONITORING

#define USE_TEMP_MONITORING

// Choose if you wish to use 8 additional channels:
//     5362 MHz
//     5399 MHz
//     5436 MHz
//     5473 MHz
//     5510 MHz
//     5547 MHz
//     5584 MHz
//     5621 MHz
// Local laws may prohibit the use of these frequencies so use at your own risk!
//#define USE_LBAND

// === Pins ====================================================================

// Buttons (required)
#define PIN_BUTTON_UP_PRESSED 44
#define PIN_BUTTON_MODE_PRESSED 45

// Buttons (optional, for comfort)
#define PIN_BUTTON_DOWN_PRESSED 46
#define PIN_BUTTON_OSDSWITCH 40 // TODO: remove

#define PIN_LED 13
#define PIN_BUZZER 6 // Must be an active buzzer, not passive.
#define PIN_OSDCONTROL 8

#define PIN_SPI_DATA 51
#define PIN_SPI_SLAVE_SELECT 53
#define PIN_SPI_CLOCK 52

#define PIN_AUX_FAN 27
#define PIN_AUX_VTX 26
#define PIN_SENSOR_THERMO A8

#define PIN_JOYSTICK_X A9
#define PIN_JOYSTICK_Y A10

#define PIN_SERVO_PAN 6
#define PIN_SERVO_TILT1 5
#define PIN_SERVO_TILT2 7

#define PIN_RSSI_A A0
#define PIN_LED_A A4
#ifdef USE_DIVERSITY
    #define PIN_RSSI_B A1
    #define PIN_LED_B A5
#endif
#ifdef USE_DUAL_DIVERSITY
    #define PIN_RSSI_B A1
    #define PIN_LED_B A5
    #define PIN_RSSI_C A2
    #define PIN_LED_C A6
    #define PIN_RSSI_D A3
    #define PIN_LED_D A7
#endif

#ifdef USE_VOLTAGE_MONITORING
    #define PIN_VBAT_DIVIDED A15
#endif

// === Diversity ===============================================================

#if defined(USE_DIVERSITY) || defined(USE_DUAL_DIVERSITY)
    // RSSI strength should be greater than the value below (percent) over the
    // other receiver before we switch. This pervents flicker when RSSI values
    // are close and delays diversity checks counter.
    #define DIVERSITY_HYSTERESIS 2

    // How long (ms) the RSSI strength has to have a greater difference than the
    // above before switching.
    #define DIVERSITY_HYSTERESIS_PERIOD 5
#endif

// === Voltage Monitoring ======================================================

#ifdef USE_VOLTAGE_MONITORING
    // You can use any Arduino analog input to measure battery voltage. Keep in
    // mind that A4 and A5 is used by OLED and A6 and A7 are used for measuring
    // RSSI.
    //
    // Use a voltage divider to lower the voltage to max 5V - values for max 13V
    // (3s). You can use a 100nF capacitor near the Arduino pin to smooth the
    // voltage.
    //
    //           R1 = 5.6k
    //    BAT+ ----====----+----+---- ARDUINO ANALOG PIN
    //                     |    |
    //                     |    |  (optional)
    //                     |    || 100n CAP
    //                     |    |
    //           R2 = 3.3k |    |
    //    BAT- ----====----|----|

    // Voltage levels
    #define WARNING_VOLTAGE 108 // 3.6V per cell for 3S
    #define CRITICAL_VOLTAGE 100 // 3.3V per cell for 3S
    #define VBAT_SCALE 119
    #define VBAT_OFFSET 0

    // Alarm sounds
    #define ALARM_EVERY_MSEC 5000
    #define CRITICAL_BEEP_EVERY_MSEC 400
    #define CRITICAL_BEEPS 3
    #define WARNING_BEEP_EVERY_MSEC 200
    #define WARNING_BEEPS 2
#endif

// === RSSI ====================================================================

// RSSI default raw range.
#define RSSI_MIN_VAL 90
#define RSSI_MAX_VAL 220

// 75% threshold, when channel is printed in spectrum.
#define RSSI_SEEK_FOUND 75

// 80% under max value for RSSI.
#define RSSI_SEEK_TRESHOLD 80

// Scan loops for setup run.
#define RSSI_SETUP_RUN 3

// === Misc ====================================================================

// Key debounce delay in milliseconds.
// Good values are in the range of 100-200ms.
// Shorter values will make it more reactive, but may lead to double trigger.
#define BUTTON_DEBOUNCE_DELAY 80

#define SCREENSAVER_TIMEOUT 3000 // Seconds to wait before entering screensaver
#define SCREENSAVER_DISPLAY_CYCLE 3 // Seconds between switching logo/channel

// Time needed to hold mode to get to menu
#define BUTTON_WAIT_FOR_MENU 1000

#endif // file_defined
