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

#include "displays/flashlight.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "drivers/apps.h"
#include "displays/data.h"
#include "displays/launcher.h"
#include "main.h"

extern app_data_t g_app_data;

void flash_init(void) {
}

void flash_update(void) {
}

void flash_draw(void) {
    uint8_t tempBrightness = TIM1->CCR1;
    set_brightness(100);
    draw_rectangle(0, 0, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, COLOR_WHITE);
}

void flash_input(button_t btn) {
    // button press returns to launcher
    if (btn == BTN_UP || btn == BTN_DOWN || btn == BTN_SEL) {
        g_app_data.display.active_screen = SCREEN_LAUNCHER;
        launcher_init();
    }
}

const app_t flash_app = {
    .init = flash_init,
    .update = flash_update,
    .draw = flash_draw,
    .input = flash_input
};
