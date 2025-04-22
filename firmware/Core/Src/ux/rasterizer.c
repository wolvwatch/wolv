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

#include "../Inc/ux/rasterizer.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "drivers/lcd.h"

static inline void put_pixel_safe(int x, int y, color_t color) {
    if (x >= 0 && x < 240 && y >= 0 && y < 240) {
        screen_set_pixel((uint16_t) x, (uint16_t) y, color);
    }
}

static void draw_line_aa(int x0, int y0, int x1, int y1, color_t color) {
    int steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        int temp = x0;
        x0 = y0;
        y0 = temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
    }

    if (x0 > x1) {
        int temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    float gradient = dx == 0 ? 1.0f : (float) dy / (float) dx;

    int xend = roundf(x0);
    float yend = y0 + gradient * (xend - x0);
    float xgap = 1.0f - (x0 + 0.5f - floorf(x0 + 0.5f));
    int xpxl1 = xend;
    int ypxl1 = floorf(yend);

    if (steep) {
        put_pixel_safe(ypxl1, xpxl1, color);
        put_pixel_safe(ypxl1 + 1, xpxl1, color);
    } else {
        put_pixel_safe(xpxl1, ypxl1, color);
        put_pixel_safe(xpxl1, ypxl1 + 1, color);
    }

    float intery = yend + gradient;

    xend = roundf(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = x1 + 0.5f - floorf(x1 + 0.5f);
    int xpxl2 = xend;
    int ypxl2 = floorf(yend);

    if (steep) {
        put_pixel_safe(ypxl2, xpxl2, color);
        put_pixel_safe(ypxl2 + 1, xpxl2, color);
    } else {
        put_pixel_safe(xpxl2, ypxl2, color);
        put_pixel_safe(xpxl2, ypxl2 + 1, color);
    }

    for (int x = xpxl1 + 1; x < xpxl2; x++) {
        if (steep) {
            put_pixel_safe(floorf(intery), x, color);
            put_pixel_safe(floorf(intery) + 1, x, color);
        } else {
            put_pixel_safe(x, floorf(intery), color);
            put_pixel_safe(x, floorf(intery) + 1, color);
        }
        intery += gradient;
    }
}

void draw_line(uint16_t x0, uint16_t y0,
               uint16_t x1, uint16_t y1,
               color_t color,
               uint16_t stroke) {
    if (stroke == 1) {
        draw_line_aa(x0, y0, x1, y1, color);
    } else {
        int dx = (int) x1 - (int) x0;
        int dy = (int) y1 - (int) y0;

        int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

        float xInc = (float) dx / (float) steps;
        float yInc = (float) dy / (float) steps;

        float x = (float) x0;
        float y = (float) y0;

        int halfStroke = stroke / 2;

        for (int i = 0; i <= steps; i++) {
            if (abs(dx) >= abs(dy)) {
                for (int offset = -halfStroke; offset <= halfStroke; offset++) {
                    put_pixel_safe((int) (x + 0.5f), (int) (y + 0.5f) + offset, color);
                }
            } else {
                for (int offset = -halfStroke; offset <= halfStroke; offset++) {
                    put_pixel_safe((int) (x + 0.5f) + offset, (int) (y + 0.5f), color);
                }
            }

            x += xInc;
            y += yInc;
        }
    }
}

// Anti-aliased arc drawing implementation
static void draw_arc_aa(uint16_t startAngle,
                        uint16_t endAngle,
                        uint16_t cx,
                        uint16_t cy,
                        uint16_t radius,
                        color_t color) {
    float startRad = (float) startAngle * M_PI / 180.0f;
    float endRad = (float) endAngle * M_PI / 180.0f;

    if (endRad < startRad) {
        endRad += 2.0f * M_PI;
    }

    float step = 1.0f / (float) radius;
    if (step > 0.1f) step = 0.1f; // Maximum step size

    for (float angle = startRad; angle <= endRad; angle += step) {
        float x = (float) cx + (float) radius * cosf(angle);
        float y = (float) cy + (float) radius * sinf(angle);

        float xf = x - floorf(x);
        float yf = y - floorf(y);

        float intensity1 = (1.0f - xf) * (1.0f - yf);
        float intensity2 = xf * (1.0f - yf);
        float intensity3 = (1.0f - xf) * yf;
        float intensity4 = xf * yf;

        int x0 = (int) floorf(x);
        int y0 = (int) floorf(y);

        put_pixel_safe(x0, y0, color);
        put_pixel_safe(x0 + 1, y0, color);
        put_pixel_safe(x0, y0 + 1, color);
        put_pixel_safe(x0 + 1, y0 + 1, color);
    }
}

void draw_arc(uint16_t startAngle,
              uint16_t endAngle,
              uint16_t cx,
              uint16_t cy,
              uint16_t radius,
              color_t color,
              bool fill,
              uint16_t stroke) {
    startAngle = startAngle % 360;
    endAngle = endAngle % 360;
    if (endAngle < startAngle) {
        endAngle += 360;
    }
    float step = 1.0f;
    float degToRad = (float) M_PI / 180.0f;
    int xPrev = cx + (int) (radius * cosf(startAngle * degToRad) + 0.5f);
    int yPrev = cy + (int) (radius * sinf(startAngle * degToRad) + 0.5f);

    for (float angle = (float) startAngle; angle <= (float) endAngle; angle += step) {
        float rad = angle * degToRad;
        int xArc = cx + (int) (radius * cosf(rad) + 0.5f);
        int yArc = cy + (int) (radius * sinf(rad) + 0.5f);

        if (fill) {
            draw_line(cx, cy, xArc, yArc, color, stroke);
            draw_line(xPrev, yPrev, xArc, yArc, color, stroke);
        } else {
            draw_line(xPrev, yPrev, xArc, yArc, color, stroke);
        }

        xPrev = xArc;
        yPrev = yArc;
    }
}

void draw_rectangle(uint16_t start_x, uint16_t start_y,
                    uint16_t width, uint16_t height,
                    color_t color) {
    for (uint16_t y = start_y; y < start_y + height; y++) {
        for (uint16_t x = start_x; x < start_x + width; x++) {
            if (x < LCD_1IN28_WIDTH && y < LCD_1IN28_HEIGHT) {
                screen_set_pixel(x, y, color);
            }
        }
    }
}

int get_char_index(const char *c, const tFont *font) {
    for (int i = 0; i < font->length; i++) {
        if (font->chars[i].code == *c) return i;
    }
    return -1;
}

static void draw_char_aa(const tChar *c, uint8_t x, uint8_t y, color_t color, float scale) {
    const tImage *img = c->image;
    int bytes_per_row = (img->width + 7) / 8;
    const uint8_t *bitmap = img->data;

    float scaled_width = img->width * scale;
    float scaled_height = img->height * scale;

    for (float row = 0; row < scaled_height; row += 1.0f) {
        for (float col = 0; col < scaled_width; col += 1.0f) {
            float orig_row = row / scale;
            float orig_col = col / scale;

            int orig_row_int = (int) orig_row;
            int orig_col_int = (int) orig_col;

            float row_frac = orig_row - orig_row_int;
            float col_frac = orig_col - orig_col_int;

            float intensities[4] = {0.0f, 0.0f, 0.0f, 0.0f};

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    int r = orig_row_int + i;
                    int c = orig_col_int + j;

                    if (r >= 0 && r < img->height && c >= 0 && c < img->width) {
                        int byte_offset = r * bytes_per_row + (c / 8);
                        int bit_mask = 0x80 >> (c % 8);
                        if (bitmap[byte_offset] & bit_mask) {
                            intensities[i * 2 + j] = 1.0f;
                        }
                    }
                }
            }

            // Bilinear interpolation
            float intensity =
                    intensities[0] * (1.0f - col_frac) * (1.0f - row_frac) +
                    intensities[1] * col_frac * (1.0f - row_frac) +
                    intensities[2] * (1.0f - col_frac) * row_frac +
                    intensities[3] * col_frac * row_frac;

            if (intensity > 0.0f) {
                int pixel_x = x + (int) col;
                int pixel_y = y + (int) row;

                if (pixel_x >= 0 && pixel_x < LCD_1IN28_WIDTH &&
                    pixel_y >= 0 && pixel_y < LCD_1IN28_HEIGHT) {
                    screen_set_pixel(pixel_x, pixel_y, color);
                }
            }
        }
    }
}

void draw_char(const tChar *c, uint8_t x, uint8_t y, color_t color, float scale) {
    if (scale == 1.0f) {
        const tImage *img = c->image;
        int bytes_per_row = (img->width + 7) / 8;
        const uint8_t *bitmap = img->data;
        for (int row = 0; row < img->height; row++) {
            for (int col = 0; col < img->width; col++) {
                int byte_offset = row * bytes_per_row + (col / 8);
                int bit_mask = 0x80 >> (col % 8);
                if (bitmap[byte_offset] & bit_mask) {
                    int pixel_x = x + col;
                    int pixel_y = y + row;
                    if (pixel_x >= 0 && pixel_x < LCD_1IN28_WIDTH &&
                        pixel_y >= 0 && pixel_y < LCD_1IN28_HEIGHT) {
                        screen_set_pixel(pixel_x, pixel_y, color);
                    }
                }
            }
        }
    } else {
        // anti-aliased version for scaled text
        draw_char_aa(c, x, y, color, scale);
    }
}

void draw_text(const char *text, uint8_t x, uint8_t y, const tFont *font, color_t color, float scale, bool center) {
    if (center) {
        uint16_t h = font->chars[0].image->height * scale;
        float w = 0;
        for (int i = 0; i < strlen(text); i++) {
            const tChar *c = &font->chars[get_char_index(text + i, font)];
            w += c->image->width * scale;
        }
        x -= w / 2;
        y -= h / 2;
    }

    float offset = 0;
    for (int i = 0; i < strlen(text); i++) {
        const tChar *c = &font->chars[get_char_index(text + i, font)];
        draw_char(c, x + (int) offset, y, color, scale);
        offset += c->image->width * scale;
    }
}

// Helper function for bilinear interpolation
static float bilinear_interpolate(float x, float y, const uint8_t *data, uint16_t width, uint16_t height) {
    int x0 = (int) floorf(x);
    int y0 = (int) floorf(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // Clamp coordinates to image bounds
    x0 = x0 < 0 ? 0 : (x0 >= width ? width - 1 : x0);
    y0 = y0 < 0 ? 0 : (y0 >= height ? height - 1 : y0);
    x1 = x1 < 0 ? 0 : (x1 >= width ? width - 1 : x1);
    y1 = y1 < 0 ? 0 : (y1 >= height ? height - 1 : y1);

    // Calculate fractional parts
    float x_frac = x - x0;
    float y_frac = y - y0;

    // Get pixel values
    float p00 = (data[(y0 * width + x0) / 8] & (1 << (7 - (x0 % 8)))) ? 1.0f : 0.0f;
    float p10 = (data[(y0 * width + x1) / 8] & (1 << (7 - (x1 % 8)))) ? 1.0f : 0.0f;
    float p01 = (data[(y1 * width + x0) / 8] & (1 << (7 - (x0 % 8)))) ? 1.0f : 0.0f;
    float p11 = (data[(y1 * width + x1) / 8] & (1 << (7 - (x1 % 8)))) ? 1.0f : 0.0f;

    // Perform bilinear interpolation
    float top = p00 * (1 - x_frac) + p10 * x_frac;
    float bottom = p01 * (1 - x_frac) + p11 * x_frac;
    return top * (1 - y_frac) + bottom * y_frac;
}

void draw_image(const tImage *image,
                uint16_t x, uint16_t y,
                color_t color,
                float scale,
                bool center) {
    if (!image || !image->data || scale <= 0.0f) return;

    uint16_t src_w = image->width;
    uint16_t src_h = image->height;
    const uint8_t *data = image->data;
    int bytes_per_row = (src_w + 7) / 8;

    uint16_t dst_w = (uint16_t) ceilf(src_w * scale);
    uint16_t dst_h = (uint16_t) ceilf(src_h * scale);

    int start_x = center
                      ? (int) x - (int) (dst_w / 2)
                      : (int) x;
    int start_y = center
                      ? (int) y - (int) (dst_h / 2)
                      : (int) y;

    for (uint16_t dst_row = 0; dst_row < dst_h; dst_row++) {
        for (uint16_t dst_col = 0; dst_col < dst_w; dst_col++) {
            uint16_t src_x = (uint16_t) floorf(dst_col / scale);
            uint16_t src_y = (uint16_t) floorf(dst_row / scale);

            if (src_x >= src_w) src_x = src_w - 1;
            if (src_y >= src_h) src_y = src_h - 1;

            int byte_offset = src_y * bytes_per_row + (src_x / 8);
            uint8_t bit_mask = 0x80 >> (src_x % 8);

            if (data[byte_offset] & bit_mask) {
                put_pixel_safe(start_x + dst_col,
                               start_y + dst_row,
                               color);
            }
        }
    }
}


void draw_radial_gradient(uint16_t cx, uint16_t cy, uint16_t radius,
                          color_t inner_color, color_t outer_color) {
    for (int16_t dy = -radius; dy <= radius; dy++) {
        for (int16_t dx = -radius; dx <= radius; dx++) {
            int16_t x = cx + dx;
            int16_t y = cy + dy;
            float dist = sqrtf((float) (dx * dx + dy * dy));
            if (dist <= radius) {
                float t = dist / (float) radius;

                uint8_t r1 = (inner_color >> 11) & 0x1F,
                        g1 = (inner_color >> 5) & 0x3F,
                        b1 = inner_color & 0x1F;
                uint8_t r2 = (outer_color >> 11) & 0x1F,
                        g2 = (outer_color >> 5) & 0x3F,
                        b2 = outer_color & 0x1F;

                uint8_t r = r1 + (uint8_t) ((r2 - r1) * t);
                uint8_t g = g1 + (uint8_t) ((g2 - g1) * t);
                uint8_t b = b1 + (uint8_t) ((b2 - b1) * t);

                color_t c = (r << 11) | (g << 5) | b;
                put_pixel_safe(x, y, c);
            }
        }
    }
}
