#include <Arduino.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "receiver_spi.h"
#include "channels.h"

#include "timer.h"

static void updateRssiLimits();
static void writeSerialData();


namespace Receiver {
    ReceiverId activeReceiver = ReceiverId::A;
    uint8_t activeChannel = 0;

    uint8_t rssiA = 0;
    uint16_t rssiARaw = 0;
    uint8_t rssiALast[RECEIVER_LAST_DATA_SIZE] = { 0 };
    #ifdef USE_DIVERSITY
        uint8_t rssiB = 0;
        uint16_t rssiBRaw = 0;
        uint8_t rssiBLast[RECEIVER_LAST_DATA_SIZE] = { 0 };

        ReceiverId diversityTargetReceiver = activeReceiver;
        Timer diversityHysteresisTimer = Timer(DIVERSITY_HYSTERESIS_PERIOD);
    #endif
    #ifdef USE_DUAL_DIVERSITY
        uint8_t rssiB = 0;
        uint16_t rssiBRaw = 0;
        uint8_t rssiBLast[RECEIVER_LAST_DATA_SIZE] = { 0 };
        
        uint8_t rssiC = 0;
        uint16_t rssiCRaw = 0;
        uint8_t rssiCLast[RECEIVER_LAST_DATA_SIZE] = { 0 };
        
        uint8_t rssiD = 0;
        uint16_t rssiDRaw = 0;
        uint8_t rssiDLast[RECEIVER_LAST_DATA_SIZE] = { 0 };

        ReceiverId diversityTargetReceiver = activeReceiver;
        Timer diversityHysteresisTimer = Timer(DIVERSITY_HYSTERESIS_PERIOD);
    #endif

    static Timer rssiStableTimer = Timer(MIN_TUNE_TIME);
    static Timer rssiLogTimer = Timer(RECEIVER_LAST_DELAY);
    #ifdef USE_SERIAL_OUT
        static Timer serialLogTimer = Timer(25);
    #endif


    void setChannel(uint8_t channel)
    {
        ReceiverSpi::setSynthRegisterB(Channels::getSynthRegisterB(channel));

        rssiStableTimer.reset();
        activeChannel = channel;
    }

    void setActiveReceiver(ReceiverId receiver) {
        #ifdef USE_DIVERSITY
            #ifdef USE_DIVERSITY_FAST_SWITCHING
                uint8_t targetPin, disablePin;
                if (receiver == ReceiverId::A) {
                    targetPin = PIN_LED_A;
                    disablePin = PIN_LED_B;
                } else {
                    targetPin = PIN_LED_B;
                    disablePin = PIN_LED_A;
                }

                uint8_t port = digitalPinToPort(targetPin);
                uint8_t targetBit = digitalPinToBitMask(targetPin);
                uint8_t disablebit = digitalPinToBitMask(disablePin);
                volatile uint8_t *out = portOutputRegister(port);

                *out = (*out | targetBit) & ~disablebit;
            #else
                digitalWrite(PIN_LED_A, receiver == ReceiverId::A);
                digitalWrite(PIN_LED_B, receiver == ReceiverId::B);
            #endif
        #elif defined(USE_DUAL_DIVERSITY)
            #ifdef USE_DIVERSITY_FAST_SWITCHING
                uint8_t targetPin, disablePin;

                switch (receiver) {
                  case ReceiverId::A:
                    targetPin = PIN_LED_A;
                  break;
                  case ReceiverId::B:
                    targetPin = PIN_LED_B;
                  break;
                  case ReceiverId::C:
                    targetPin = PIN_LED_C;
                  break;
                  case ReceiverId::D:
                    targetPin = PIN_LED_D;
                  break; 
                }
                
                switch (activeReceiver) {
                  case ReceiverId::A:
                    disablePin = PIN_LED_A;
                  break;
                  case ReceiverId::B:
                    disablePin = PIN_LED_B;
                  break;
                  case ReceiverId::C:
                    disablePin = PIN_LED_C;
                  break;
                  case ReceiverId::D:
                    disablePin = PIN_LED_D;
                  break; 
                }

                uint8_t port = digitalPinToPort(targetPin);
                uint8_t targetBit = digitalPinToBitMask(targetPin);
                uint8_t disablebit = digitalPinToBitMask(disablePin);
                volatile uint8_t *out = portOutputRegister(port);

                *out = (*out | targetBit) & ~disablebit;
                
            #else
                digitalWrite(PIN_LED_A, receiver == ReceiverId::A);
                digitalWrite(PIN_LED_B, receiver == ReceiverId::B);
                digitalWrite(PIN_LED_C, receiver == ReceiverId::C);
                digitalWrite(PIN_LED_D, receiver == ReceiverId::D);
                
                Serial.println(receiver == ReceiverId::A);
                Serial.println(receiver == ReceiverId::B);
                Serial.println(receiver == ReceiverId::C);
                Serial.println(receiver == ReceiverId::D);
            #endif
        #else
            digitalWrite(PIN_LED_A, HIGH);
        #endif

        activeReceiver = receiver;
    }

    bool isRssiStable() {
        return rssiStableTimer.hasTicked();
    }

    uint16_t updateRssi() {

        analogRead(PIN_RSSI_A); // Fake read to let ADC settle.
        rssiARaw = (analogRead(PIN_RSSI_A)+analogRead(PIN_RSSI_A)+analogRead(PIN_RSSI_A)) / 3;
        #ifdef USE_DIVERSITY
            analogRead(PIN_RSSI_B);
            rssiBRaw = (analogRead(PIN_RSSI_B)+analogRead(PIN_RSSI_B)+analogRead(PIN_RSSI_B)) / 3;
        #endif
        #ifdef USE_DUAL_DIVERSITY
            analogRead(PIN_RSSI_B);
            rssiBRaw = (analogRead(PIN_RSSI_B)+analogRead(PIN_RSSI_B)+analogRead(PIN_RSSI_B)) / 3;
            analogRead(PIN_RSSI_C);
            rssiCRaw = (analogRead(PIN_RSSI_C)+analogRead(PIN_RSSI_C)+analogRead(PIN_RSSI_C)) / 3;
            analogRead(PIN_RSSI_D);
            rssiDRaw = (analogRead(PIN_RSSI_D)+analogRead(PIN_RSSI_D)+analogRead(PIN_RSSI_D)) / 3;
       #endif
       
        rssiA = constrain(
            map(
                rssiARaw,
                EepromSettings.rssiAMin,
                EepromSettings.rssiAMax,
                0,
                100
            ),
            0,
            100
        );
        #ifdef USE_DIVERSITY
            rssiB = constrain(
                map(
                    rssiBRaw,
                    EepromSettings.rssiBMin,
                    EepromSettings.rssiBMax,
                    0,
                    100
                ),
                0,
                100
            );
        #endif
        #ifdef USE_DUAL_DIVERSITY
            rssiB = constrain(
                map(
                    rssiBRaw,
                    EepromSettings.rssiBMin,
                    EepromSettings.rssiBMax,
                    0,
                    100
                ),
                0,
                100
            );
            rssiC = constrain(
                map(
                    rssiCRaw,
                    EepromSettings.rssiCMin,
                    EepromSettings.rssiCMax,
                    0,
                    100
                ),
                0,
                100
            );
            rssiD = constrain(
                map(
                    rssiDRaw,
                    EepromSettings.rssiDMin,
                    EepromSettings.rssiDMax,
                    0,
                    100
                ),
                0,
                100
            );
        #endif

        if (rssiLogTimer.hasTicked()) {
            for (uint8_t i = 0; i < RECEIVER_LAST_DATA_SIZE - 1; i++) {
                rssiALast[i] = rssiALast[i + 1];
                #ifdef USE_DIVERSITY
                    rssiBLast[i] = rssiBLast[i + 1];
                #endif
                #ifdef USE_DUAL_DIVERSITY
                    rssiBLast[i] = rssiBLast[i + 1];
                    rssiCLast[i] = rssiCLast[i + 1];
                    rssiDLast[i] = rssiDLast[i + 1];
                #endif
            }

            rssiALast[RECEIVER_LAST_DATA_SIZE - 1] = rssiA;
            #ifdef USE_DIVERSITY
                rssiBLast[RECEIVER_LAST_DATA_SIZE - 1] = rssiB;
            #endif
            #ifdef USE_DUAL_DIVERSITY
                rssiBLast[RECEIVER_LAST_DATA_SIZE - 1] = rssiB;
                rssiCLast[RECEIVER_LAST_DATA_SIZE - 1] = rssiC;
                rssiDLast[RECEIVER_LAST_DATA_SIZE - 1] = rssiD;
            #endif

            rssiLogTimer.reset();
        }
    }

#ifdef USE_DIVERSITY
    void setDiversityMode(DiversityMode mode) {
        EepromSettings.diversityMode = mode;
        switchDiversity();
    }

    void switchDiversity() {
        ReceiverId nextReceiver = activeReceiver;

        if (EepromSettings.diversityMode == DiversityMode::AUTO) {
            int8_t rssiDiff = (int8_t) rssiA - (int8_t) rssiB;
            uint8_t rssiDiffAbs = abs(rssiDiff);
            ReceiverId currentBestReceiver = activeReceiver;

            if (rssiDiff > 0) {
                currentBestReceiver = ReceiverId::A;
            } else if (rssiDiff < 0) {
                currentBestReceiver = ReceiverId::B;
            } else {
                currentBestReceiver = activeReceiver;
            }

            if (rssiDiffAbs >= DIVERSITY_HYSTERESIS) {
                if (currentBestReceiver == diversityTargetReceiver) {
                    if (diversityHysteresisTimer.hasTicked()) {
                        nextReceiver = diversityTargetReceiver;
                    }
                } else {
                    diversityTargetReceiver = currentBestReceiver;
                    diversityHysteresisTimer.reset();
                }
            } else {
                diversityHysteresisTimer.reset();
            }
        } else {
            switch (EepromSettings.diversityMode) {
                case DiversityMode::FORCE_A:
                    nextReceiver = ReceiverId::A;
                    break;

                case DiversityMode::FORCE_B:
                    nextReceiver = ReceiverId::B;
                    break;
            }
        }
        
        //  Do not change activeReceiver if OSD is on.  This will turn on a 4066 Rx switch simultaneously with the OSD.
        if (!digitalRead(PIN_OSDCONTROL)) {
          setActiveReceiver(nextReceiver);
        }
    }
#endif
#ifdef USE_DUAL_DIVERSITY
        
    void setDiversityMode(DiversityMode mode) {
        EepromSettings.diversityMode = mode;
        switchDiversity();
    }

    void switchDiversity() {
        ReceiverId nextReceiver = activeReceiver;

        if (EepromSettings.diversityMode == DiversityMode::AUTO) {
          
            int8_t rssiMax = max(max(rssiA, rssiB), max(rssiC, rssiD));
            uint8_t rssiDiff = 0;
            uint8_t rssiDiffAbs = 0;
            ReceiverId currentBestReceiver = activeReceiver;


            // Find which Rx has the highest RSSI.
            if (rssiA == rssiMax) {
              currentBestReceiver = ReceiverId::A;
            } else if (rssiB == rssiMax) {
              currentBestReceiver = ReceiverId::B;
            } else if (rssiC == rssiMax) {
              currentBestReceiver = ReceiverId::C;
            } else if (rssiD == rssiMax) {
              currentBestReceiver = ReceiverId::D;
            }

            // Difference against currently active Rx.
            if (ReceiverId::A == activeReceiver) {
              rssiDiff = rssiMax - rssiA;
            } else if (ReceiverId::B == activeReceiver) {
              rssiDiff = rssiMax - rssiB;
            } else if (ReceiverId::C == activeReceiver) {
              rssiDiff = rssiMax - rssiC;
            } else if (ReceiverId::D == activeReceiver) {
              rssiDiff = rssiMax - rssiD;
            }
                
            rssiDiffAbs = abs(rssiDiff);

            if (rssiDiffAbs >= DIVERSITY_HYSTERESIS) {
                if (currentBestReceiver == diversityTargetReceiver) {
                    if (diversityHysteresisTimer.hasTicked()) {
                        nextReceiver = diversityTargetReceiver;
                    }
                } else {
                    diversityTargetReceiver = currentBestReceiver;
                    diversityHysteresisTimer.reset();
                }
            } else {
                diversityHysteresisTimer.reset();
            }
        } else {
            switch (EepromSettings.diversityMode) {
                case DiversityMode::FORCE_A:
                    nextReceiver = ReceiverId::A;
                    break;

                case DiversityMode::FORCE_B:
                    nextReceiver = ReceiverId::B;
                    break;
                    
                case DiversityMode::FORCE_C:
                    nextReceiver = ReceiverId::C;
                    break;

                case DiversityMode::FORCE_D:
                    nextReceiver = ReceiverId::D;
                    break;
            }
        }
        
        //  Do not attempt to change Rx if it is the same the currently active Rx.  If attempted this may have issues with port output.
        //  Do not change activeReceiver if OSD is on.  This will turn on a 4066 Rx switch simultaneously with the OSD.
        if (nextReceiver != activeReceiver && !digitalRead(PIN_OSDCONTROL)) {
          setActiveReceiver(nextReceiver);
        }
    }
#endif

    void setup() {
        #ifdef DISABLE_AUDIO
            ReceiverSpi::setPowerDownRegister(0b00010000110111110011);
        #endif
    }

    void update() {
        if (rssiStableTimer.hasTicked()) {
            updateRssi();

            #ifdef USE_SERIAL_OUT
                writeSerialData();
            #endif

            #if defined(USE_DIVERSITY) || defined(USE_DUAL_DIVERSITY)
                switchDiversity();
            #endif
        }
    }
}


#ifdef USE_SERIAL_OUT

#include "pstr_helper.h"

static void writeSerialData() {
    return;
    if (Receiver::serialLogTimer.hasTicked()) {
        Serial.print(Receiver::activeChannel, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiA, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiARaw, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiB, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiBRaw, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiC, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiCRaw, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiD, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiDRaw, DEC);
        Serial.println();

        Receiver::serialLogTimer.reset();
    }
}
#endif
