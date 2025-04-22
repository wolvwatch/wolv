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

#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>
#include "drivers/apps.h"

void analog_init(void);

void analog_update(uint8_t hours, uint8_t minutes, uint8_t seconds);

void draw_watch_face(void);

void draw_hour_markers(void);

void draw_watch_hands(uint8_t hours, uint8_t minutes, uint8_t seconds);

void draw_biometric_data(void);

void draw_center_dot(void);

void watchface_analog_init(void);

void watchface_analog_update(void);

void watchface_analog_draw(void);

void watchface_analog_input(button_t button);


#endif /* ANALOG_H */
