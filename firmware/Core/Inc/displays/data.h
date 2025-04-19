
#ifndef DATA_H
#define DATA_H

#include "screen.h"

typedef struct {
    uint8_t brightness;
    bool metric;
    bool bluetooth;
    uint8_t battery_level;
} settings_t;

typedef struct {
    uint8_t heart_rate;
    uint16_t steps;
    float spo2;
} biometrics_t;

typedef struct {
    uint16_t month;
    uint16_t day;
    uint16_t year;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} timeVal_t;

typedef struct {
    display_t display; // in screen.h
    settings_t settings;
    biometrics_t biometrics;
    timeVal_t timeVal;
} app_data_t;

#ifndef COLOR_WHITE
#define COLOR_WHITE 0b111
#endif

#ifndef COLOR_BLACK
#define COLOR_BLACK 0b000
#endif

#ifndef COLOR_RED
#define COLOR_RED 0b100
#endif

#define CENTER_X 121
#define CENTER_Y 120
#define WATCH_RADIUS 118

#endif //DATA_H
