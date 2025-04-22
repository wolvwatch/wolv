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

#include "drivers/apps.h"

extern const app_t analog_app;
extern const app_t digital_app;
extern const app_t launcher_app;
extern const app_t weather_app;
extern const app_t flappy_app;
extern const app_t timer_app;
extern const app_t sysmon_app;
extern const app_t flash_app;

app_t apps[APP_COUNT];

void init_apps(void) {
    apps[APP_WATCHFACE_ANALOG] = analog_app;
    apps[APP_WATCHFACE_DIGITAL] = digital_app;
    apps[APP_LAUNCHER] = launcher_app;
    apps[APP_WEATHER] = weather_app;
    apps[APP_GAMES] = flappy_app;
    apps[APP_TIMER] = timer_app;
    apps[APP_SYSINFO] = sysmon_app;
    apps[APP_FLASHLIGHT] = flash_app;
} 