/*
The MIT License (MIT)

Copyright (c) 2025 Sandro Petrovski, Austin Sierco, Ryan Kaelle, and Tenzin Sherab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#ifndef DATA_H
#define DATA_H

#include "screen.h"

#define COLOR_WHITE       0xFFFF
#define COLOR_BLACK       0x0000
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_CYAN        0x07FF
#define COLOR_MAGENTA     0xF81F
#define COLOR_YELLOW      0xFFE0
#define COLOR_ORANGE      0xFD20
#define COLOR_PINK        0xFC18
#define COLOR_PURPLE      0x8010
#define COLOR_BROWN       0xA145
#define COLOR_GRAY        0x8410
#define COLOR_LIGHT_GRAY  0xC618
#define COLOR_DARK_GRAY   0x4208
#define COLOR_GOLD        0xFEA0

#define CENTER_X 121
#define CENTER_Y 120
#define WATCH_RADIUS 118

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
    float temp_current; // current temperature
    float temp_high; // high temperature
    float temp_low; // low temperature
    uint8_t humidity; // humidity percentage
    uint8_t condition; // condition code (0=sunny, 1=partly cloudy, 2=cloudy, 3=rain, 4=storm, 5=snow)
    char location[32]; // location name
    char description[32]; // weather description
} weather_t;

typedef struct {
    uint16_t month;
    uint16_t day;
    uint16_t year;
    uint16_t hour;
    uint16_t minute;
    uint16_t second;
} timeVal_t;

typedef struct {
    display_t display; // defined in screen.h
    settings_t settings;
    biometrics_t biometrics;
    timeVal_t timeVal;
    weather_t weather;
} app_data_t;

#endif //DATA_H
