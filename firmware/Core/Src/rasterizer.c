#define STB_TRUETYPE_IMPLEMENTATION
#include "rasterizer.h"
#include "Montserrat-Regular.h"
#include <math.h>
#include "lcd.h"
#include "stb_truetype.h"


static stbtt_fontinfo g_font;
static float g_scale;

void font_init(int font_pixel_height) {
    // stb_truetype
    int offset = stbtt_GetFontOffsetForIndex(Montserrat_Regular_Subset_ttf, 0);
    stbtt_InitFont(&g_font, Montserrat_Regular_Subset_ttf, offset);

    // Compute the scale factor for the desired pixel height
    g_scale = stbtt_ScaleForPixelHeight(&g_font, (float)font_pixel_height);
}

// draw singular char
void draw_char(int x, int y, char c, color_t color) {
    int codepoint = (unsigned char)c;

    int width, height, xoffset, yoffset;
    unsigned char *bitmap = stbtt_GetCodepointBitmap(
        &g_font,
        0,
        g_scale,
        codepoint,
        &width,
        &height,
        &xoffset,
        &yoffset
    );
    if (!bitmap) {
        return; // glyph not found
    }

    // The bitmap is 'width' x 'height', 1 byte per pixel (0–255).
    // xoffset, yoffset let us position relative to the baseline.
    //
    // Typically, yoffset is negative for letters that sit above the baseline,
    // so we offset the draw position by yoffset.

    int draw_x = x + xoffset;
    int draw_y = y + yoffset;

    // Loop through the bitmap and set pixels
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            unsigned char alpha = bitmap[row * width + col];
            // alpha ~ [0..255], with 255 = fully "on"

            // Simple threshold approach:
            if (alpha > 128) {
                SET_PIXEL(draw_x + col, draw_y + row, color);
            }
        }
    }

    // Free the stb_truetype bitmap
    stbtt_FreeBitmap(bitmap, NULL);
}

//iterate over more than one char
void draw_text(int x, int y, const char *text, color_t color) {
    int xpos = x;

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&g_font, &ascent, &descent, &lineGap);
    float fAscent = ascent * g_scale;
    // Typically, we position text so that y is the "baseline," so shift up by ascent
    int baseline = y + (int)fAscent;

    while (*text) {
        char c = *text++;
        if (c == '\n') {
            // new lines if needed
            xpos = x;
            baseline += (int)(-descent * g_scale + lineGap * g_scale + 0.5f);
            continue;
        }

        // Get metrics for spacing
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&g_font, (int)c, &advanceWidth, &leftSideBearing);

        // Draw the character. We pass 'baseline' so we place the glyph properly.
        // In draw_char(), we might use y+yoffset or baseline+yoffset, etc.
        draw_char(xpos, baseline, c, color);

        // Advance for next character
        xpos += (int)(advanceWidth * g_scale + 0.5f);
    }
}


static void draw_horizontal_line(uint16_t x0, uint16_t x1, uint16_t y, color_t color) {
    if (x0 > x1) {
        uint16_t temp = x0;
        x0 = x1;
        x1 = temp;
    }
    
    for (uint16_t x = x0; x <= x1; x++) {
        SET_PIXEL(x, y, color);
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
            SET_PIXEL(x0, y0, color);
        } else {
            for (int16_t i = -stroke_offset; i <= stroke_offset; i++) {
                if (dx > dy) {
                    SET_PIXEL(x0, y0 + i, color);
                } else {
                    SET_PIXEL(x0 + i, y0, color);
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
                        SET_PIXEL(x + px, y + py + s, color); \
                        SET_PIXEL(x + px, y + py - s, color); \
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