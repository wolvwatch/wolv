/******************************************************************************
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#ifndef __GUI_PAINT_H
#define __GUI_PAINT_H

#include <stdbool.h>
#include <stdint.h>
#include "bitmaps.h"

/* RGB565 COLORS */
#define WHITE 0xFFFF
#define BLACK 0x0000
#define RED 0xFF00
#define GREEN 0x07E0
#define BLUE 0x001F
#define LIGHTGRAY 0xAD75

typedef uint16_t color_t;

typedef enum {
	HOME_DIGITAL,
	HOME_ANALOG,
	SET_TIME_HR,
	SET_TIME_MIN,
	SET_TIME_SEC,
	SET_TIME_AMPM
} gui_state_t;

typedef struct {
	uint8_t set_time_hr, set_time_min, set_time_sec;
	bool set_time_AM;
	gui_state_t state;
} gui_data_t;

void set_gui_state(gui_state_t state);

void draw_img(uint8_t x, uint8_t y, const bitmap_t *img, color_t foreground, color_t background, float scale);

void draw_current_time(uint8_t start_x, uint8_t start_y, font_bitmap_t *font, color_t foreground, color_t background,
                       float scale);

void draw_string(uint8_t x, uint8_t y, char *str, font_bitmap_t *font, color_t foreground, color_t background,
                 float scale);

void draw_circle(uint8_t x, uint8_t y, uint8_t r, color_t foreground);

void update_gui();
#endif

