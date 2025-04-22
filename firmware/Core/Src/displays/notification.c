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

#include "displays/notification.h"
#include "stm32l4xx_hal.h"
#include "drivers/lcd.h"
#include "displays/data.h"
#include "drivers/haptic.h"
#include "ux/rasterizer.h"

void display_notification(const char *message) {
    screen_clear(0x0000);
    uint16_t start_x = 30;
    uint16_t start_y = 50;
    uint16_t width = 180;
    uint16_t height = 140;
    uint16_t radius = 10; // Corner radius

    // Draw the middle filled rectangle background
    draw_rectangle(start_x + radius, start_y + radius, width - (radius * 2), height - (radius * 2), 0b111);
    draw_rectangle(start_x + radius, start_y, width - (radius * 2), height + 1, 0b111);
    draw_rectangle(start_x, start_y + radius, width + 1, height - (radius * 2), 0b111);

    // top left
    draw_arc(180, 270, start_x + radius, start_y + radius, radius, 0b111, true, 1);
    // top right
    draw_arc(270, 360, start_x + width - radius - 1, start_y + radius, radius, 0b111, true, 1);
    // bottom right
    draw_arc(0, 90, start_x + width - radius - 1, start_y + height - radius - 1, radius, 0b111, true, 1);
    // bottom left
    draw_arc(90, 180, start_x + radius, start_y + height - radius - 1, radius, 0b111, true, 1);

    // Draw the straight lines between corners

    // Draw the text
    draw_text(message, CENTER_X, CENTER_Y, &montserrat_reg, 0b000, 1.0, true);
    //screen_render();
    Haptic_Buzz();
}
