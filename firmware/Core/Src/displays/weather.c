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

#include "displays/weather.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "displays/data.h"
#include "displays/launcher.h"
#include <stdio.h>

extern app_data_t g_app_data;

static const char* condition_symbols[] = {"Sunny","Partly Cloudy","Cloudy","Rainy","Storm","Snow"};

void weather_init(void) {}

void weather_update(void) {}

void weather_draw(void) {
    char buf[64];
    draw_text("Weather", CENTER_X, 30, &montserrat_reg, COLOR_WHITE, 1.2, true);

    // loc
    draw_text(g_app_data.weather.location, CENTER_X, 60, &montserrat_reg, COLOR_WHITE, 0.8, true);

    //temperature (metric or imperial lol)
    snprintf(buf, sizeof(buf), "%.1f %c %s",
             g_app_data.weather.temp_current,
             g_app_data.settings.metric ? 'C' : 'F',
             condition_symbols[g_app_data.weather.condition]);
    draw_text(buf, CENTER_X, 100, &montserrat_reg, COLOR_WHITE, 0.8, true);

    // desc
    //draw_text(g_app_data.weather.description, CENTER_X, 130, &montserrat_reg, COLOR_WHITE, 0.8, true);

    // high and low temperatures (metric or imperial lol)
    snprintf(buf, sizeof(buf), "H: %.1f %c  L: %.1f %c",
             g_app_data.weather.temp_high,
             g_app_data.settings.metric ? 'C' : 'F',
             g_app_data.weather.temp_low,
             g_app_data.settings.metric ? 'C' : 'F');
    draw_text(buf, CENTER_X, 135, &montserrat_reg, COLOR_WHITE, 0.8, true);

    // humidity
    snprintf(buf, sizeof(buf), "Humidity: %d%%", g_app_data.weather.humidity);
    draw_text(buf, CENTER_X, 160, &montserrat_reg, COLOR_WHITE, 0.8, true);

    // button desc
    draw_text("Press any button for menu", CENTER_X, 185,&montserrat_reg, COLOR_WHITE, 0.6, true);
}


void weather_input(button_t btn) {
    // any button press returns to launcher
    if (btn == BTN_UP || btn == BTN_DOWN || btn == BTN_SEL) {
        g_app_data.display.active_screen = SCREEN_LAUNCHER;
        launcher_init();
    }
}

const app_t weather_app = {
    .init = weather_init,
    .update = weather_update,
    .draw = weather_draw,
    .input = weather_input
}; 