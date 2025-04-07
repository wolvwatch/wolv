#include "rasterizer.h"
#include <math.h>
#include "devices/lcd.h"

static void set_pixel(uint16_t x, uint16_t y, color_t color) {
    // check if coordinates are within screen bounds
    if (x >= LCD_1IN28_WIDTH || y >= LCD_1IN28_HEIGHT) return;
    
    // calculate the position in the pixel buffer
    uint32_t pos = (y * LCD_1IN28_WIDTH + x) * 2;
    
    // Write color value to the pixel buffer (16-bit format)
    extern uint8_t pixels[];
    pixels[pos] = color >> 8;
    pixels[pos + 1] = color & 0xFF;
}

static void draw_horizontal_line(uint16_t x0, uint16_t x1, uint16_t y, color_t color) {
    if (x0 > x1) {
        uint16_t temp = x0;
        x0 = x1;
        x1 = temp;
    }
    
    for (uint16_t x = x0; x <= x1; x++) {
        set_pixel(x, y, color);
    }
}

void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint16_t stroke) {
    // Bresenham's line algorithm (i think)
    int16_t dx = abs((int16_t)x1 - (int16_t)x0);
    int16_t dy = -abs((int16_t)y1 - (int16_t)y0);
    int16_t sx = x0 < x1 ? 1 : -1;
    int16_t sy = y0 < y1 ? 1 : -1;
    int16_t err = dx + dy;
    
    int16_t stroke_offset = stroke / 2;
    
    while (1) {
        // Draw the main pixel
        if (stroke == 1) {
            set_pixel(x0, y0, color);
        } else {
            for (int16_t i = -stroke_offset; i <= stroke_offset; i++) {
                if (dx > dy) {
                    set_pixel(x0, y0 + i, color);
                } else {
                    set_pixel(x0 + i, y0, color);
                }
            }
        }
        
        if (x0 == x1 && y0 == y1) break;
        
        int16_t e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

void draw_arc(uint16_t start_angle, uint16_t end_angle, uint16_t x, uint16_t y, 
              uint16_t radius, color_t color, bool fill, uint16_t stroke) {
    // Convert angles to radians and normalize
    float start_rad = (start_angle % 360) * M_PI / 180.0f;
    float end_rad = (end_angle % 360) * M_PI / 180.0f;
    
    // Ensure end angle is greater than start angle
    if (end_rad < start_rad) {
        end_rad += 2 * M_PI;
    }
    
    int16_t xm = 0;
    int16_t ym = radius;
    int16_t err = 2 - 2 * radius;
    
    do {
        float current_angle;
        
        // Calculate angle for each point and check if it's within arc range
        #define CHECK_AND_DRAW_POINT(px, py) do { \
            current_angle = atan2f(py, px); \
            if (current_angle < 0) current_angle += 2 * M_PI; \
            if (current_angle >= start_rad && current_angle <= end_rad) { \
                if (fill) { \
                    draw_horizontal_line(x, x + px, y + py, color); \
                    if (py != 0) draw_horizontal_line(x, x + px, y - py, color); \
                } else { \
                    for (uint16_t s = 0; s < stroke; s++) { \
                        set_pixel(x + px, y + py + s, color); \
                        set_pixel(x + px, y + py - s, color); \
                    } \
                } \
            } \
        } while(0)
        
        // Check all octants
        CHECK_AND_DRAW_POINT(xm, ym);   // Octant 1
        CHECK_AND_DRAW_POINT(ym, xm);   // Octant 2
        CHECK_AND_DRAW_POINT(-xm, ym);  // Octant 4
        CHECK_AND_DRAW_POINT(-ym, xm);  // Octant 3
        CHECK_AND_DRAW_POINT(-xm, -ym); // Octant 5
        CHECK_AND_DRAW_POINT(-ym, -xm); // Octant 6
        CHECK_AND_DRAW_POINT(xm, -ym);  // Octant 8
        CHECK_AND_DRAW_POINT(ym, -xm);  // Octant 7
        
        #undef CHECK_AND_DRAW_POINT
        
        int16_t r = err;
        if (r <= xm) err += ++xm * 2 + 1;
        if (r > ym || err > xm) err += ++ym * 2 + 1;
    } while (xm < 0 || ym > 0);
} 