
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
#define COLOR_WHITE       0xFFFF
#endif

#ifndef COLOR_BLACK
#define COLOR_BLACK       0x0000
#endif

#ifndef COLOR_RED
#define COLOR_RED         0xF800
#endif

#ifndef COLOR_GREEN
#define COLOR_GREEN       0x07E0
#endif

#ifndef COLOR_BLUE
#define COLOR_BLUE        0x001F
#endif

#ifndef COLOR_CYAN
#define COLOR_CYAN        0x07FF
#endif

#ifndef COLOR_MAGENTA
#define COLOR_MAGENTA     0xF81F
#endif

#ifndef COLOR_YELLOW
#define COLOR_YELLOW      0xFFE0
#endif

#ifndef COLOR_ORANGE
#define COLOR_ORANGE      0xFD20
#endif

#ifndef COLOR_PINK
#define COLOR_PINK        0xFC18
#endif

#ifndef COLOR_PURPLE
#define COLOR_PURPLE      0x8010
#endif

#ifndef COLOR_BROWN
#define COLOR_BROWN       0xA145
#endif

#ifndef COLOR_GRAY
#define COLOR_GRAY        0x8410
#endif

#ifndef COLOR_LIGHT_GRAY
#define COLOR_LIGHT_GRAY  0xC618
#endif

#ifndef COLOR_DARK_GRAY
#define COLOR_DARK_GRAY   0x4208
#endif

#ifndef COLOR_GOLD
#define COLOR_GOLD        0xFEA0
#endif

#define CENTER_X 121
#define CENTER_Y 120
#define WATCH_RADIUS 118

#endif //DATA_H
