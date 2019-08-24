#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H


#include <stdint.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_internal.h"
#include "receiver.h"


struct EepromSettings {
    uint32_t magic;
    uint8_t startChannel;

    uint8_t beepEnabled;

    uint8_t searchManual;
    uint8_t searchOrderByChannel;

    uint16_t rssiAMin;
    uint16_t rssiAMax;

    #ifdef USE_DIVERSITY
        Receiver::DiversityMode diversityMode;
        uint16_t rssiBMin;
        uint16_t rssiBMax;
    #endif
    #ifdef USE_DUAL_DIVERSITY
        Receiver::DiversityMode diversityMode;
        uint16_t rssiBMin;
        uint16_t rssiBMax;
        uint16_t rssiCMin;
        uint16_t rssiCMax;
        uint16_t rssiDMin;
        uint16_t rssiDMax;
    #endif


    void update();

    void load();
    void save();
    void markDirty();

    void initDefaults();
};


PROGMEM const struct {
    uint32_t magic = EEPROM_MAGIC;
    uint8_t startChannel = 0;

    uint8_t beepEnabled = true;

    uint8_t searchManual = false;
    uint8_t searchOrderByChannel = false;

    uint16_t rssiAMin = RSSI_MIN_VAL;
    uint16_t rssiAMax = RSSI_MAX_VAL;

    #ifdef USE_DIVERSITY
        Receiver::DiversityMode diversityMode = Receiver::DiversityMode::AUTO;
        uint16_t rssiBMin = RSSI_MIN_VAL;
        uint16_t rssiBMax = RSSI_MAX_VAL;
    #endif
    #ifdef USE_DUAL_DIVERSITY
        Receiver::DiversityMode diversityMode = Receiver::DiversityMode::AUTO;
        uint16_t rssiBMin = RSSI_MIN_VAL;
        uint16_t rssiBMax = RSSI_MAX_VAL;
        uint16_t rssiCMin = RSSI_MIN_VAL;
        uint16_t rssiCMax = RSSI_MAX_VAL;
        uint16_t rssiDMin = RSSI_MIN_VAL;
        uint16_t rssiDMax = RSSI_MAX_VAL;
    #endif
} EepromDefaults;


extern EepromSettings EepromSettings;


#endif
