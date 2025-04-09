#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint16_t bitmap_index;  // Index into the bitmap array for the glyph's data
    uint16_t adv_w;         // Advance width in 1/16th pixels
    uint8_t box_w;          // Glyph bounding box width
    uint8_t box_h;          // Glyph bounding box height
    int8_t ofs_x;           // Horizontal offset from the cursor position
    int8_t ofs_y;           // Vertical offset from the baseline
} font_t;


typedef uint16_t color_t;

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

#endif // RASTERIZER_H