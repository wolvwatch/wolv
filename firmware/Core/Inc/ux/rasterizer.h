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

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t bitmap_index; // Index into the bitmap array for the glyph's data
    uint16_t adv_w; // Advance width in 1/16th pixels
    uint8_t box_w; // Glyph bounding box width
    uint8_t box_h; // Glyph bounding box height
    int8_t ofs_x; // Horizontal offset from the cursor position
    int8_t ofs_y; // Vertical offset from the baseline
} font_t;


typedef uint16_t color_t;

typedef struct {
    const uint8_t *data; // Pointer to bitmap data
    uint16_t width; // Glyph (image) width in pixels
    uint16_t height; // Glyph (image) height in pixels
    uint8_t dataSize; // Usually 8 (bits per element)
} tImage;

typedef struct {
    long int code; // Unicode value of the character (for example, 'A' is 65)
    const tImage *image; // Pointer to the image data structure
} tChar;

typedef struct {
    int length; // Number of characters in the font
    const tChar *chars; // Array of tChar entries
} tFont;

extern const tFont montserrat_reg;
extern const tFont ultra;

extern const tFont montserrat_reg;
extern const tFont ultra;

void draw_char(const tChar *c, uint8_t x, uint8_t y, color_t color, float scale);

void draw_text(const char *text, uint8_t x, uint8_t y, const tFont *font, color_t color, float scale, bool center);

void draw_line(uint16_t x0, uint16_t y0,
               uint16_t x1, uint16_t y1,
               color_t color,
               uint16_t stroke);

void draw_arc(uint16_t startAngle,
              uint16_t endAngle,
              uint16_t cx,
              uint16_t cy,
              uint16_t radius,
              color_t color,
              bool fill,
              uint16_t stroke);


void draw_rectangle(uint16_t start_x, uint16_t start_y,
    uint16_t width, uint16_t height, color_t color);

void draw_image(const tImage *image,
                uint16_t x, uint16_t y,
                color_t color,
                float scale,
                bool center);

void draw_radial_gradient(uint16_t cx, uint16_t cy, uint16_t radius,
                          color_t inner_color, color_t outer_color);

#endif // RASTERIZER_H
