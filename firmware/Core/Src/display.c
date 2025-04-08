#include "lcd.h"
#include "rasterizer.h"

#include <stdint.h>
#include <string.h>


extern const uint8_t text_font_bitmap[];    // text_14.c
extern const font_t text_font_dsc[];          // text_14.c

extern const uint8_t clock_font_bitmap[];   // clock.c
extern const font_t clock_font_dsc[];         // clock.c

// mapping tables for the fonts

static const char text_font_mapping[] =
    "23456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz";

static const char clock_font_mapping[] = "0123456789";


static int get_glyph_index(const char *mapping, char c)
{
    int index = 1;
    for (const char *p = mapping; *p; p++, index++) {
        if (*p == c)
            return index;
    }
    return -1;  // character not supported
}

void draw_glyph(uint16_t x, uint16_t y,
                int glyph_index,
                const uint8_t *glyph_bitmap,
                const font_t *glyph_dsc,
                uint16_t color)
{
    if(glyph_index <= 0) return;  // Invalid index

    font_t gd = glyph_dsc[glyph_index];
    int bx = gd.box_w;
    int bh = gd.box_h;

    // calculate the num of bytes
    int bytes_per_row = (bx + 7) / 8;

    for (int row = 0; row < bh; row++) {
        for (int col = 0; col < bx; col++) {
            int byte_index = gd.bitmap_index + row * bytes_per_row + (col / 8);
            int bit_index  = 7 - (col % 8);
            if (glyph_bitmap[byte_index] & (1 << bit_index)) {
                // glyph offset (ofs_x, ofs_y)
                screen_set_pixel(x + col + gd.ofs_x, y + row + gd.ofs_y, color);
            }
        }
    }
}

void draw_text(uint16_t x, uint16_t y,
               const char *text,
               const uint8_t *glyph_bitmap,
               const font_t *glyph_dsc,
               const char *mapping,
               uint16_t color)
{
    uint16_t orig_x = x;

    while (*text) {
        char c = *text++;

        // move to the next line and reset x position.
        if (c == '\n') {
            y += 16;   // line length (set as needed)
            x = orig_x;
            continue;
        }

        // glyph index
        int glyph_index = get_glyph_index(mapping, c);
        if (glyph_index < 0) {
            // if character not found
            x += 8;  // blank space value
            continue;
        }

        // draw at the current cursor position.
        draw_glyph(x, y, glyph_index, glyph_bitmap, glyph_dsc, color);

        // Advance x position by the glyph’s advance width.
        int adv = (glyph_dsc[glyph_index].adv_w + 8) / 16;
        x += adv;
    }
}


void display_main_text(uint16_t x, uint16_t y, const char *text, uint16_t color)
{
    draw_text(x, y, text, text_font_bitmap, text_font_dsc, text_font_mapping, color);
}

void display_time_text(uint16_t x, uint16_t y, const char *time_str, uint16_t color)
{
    draw_text(x, y, time_str, clock_font_bitmap, clock_font_dsc, clock_font_mapping, color);
}

void display_bluetooth_icon(uint16_t x, uint16_t y, uint16_t color)
{
  // need to do bluetooth icon
}

void display_heartrate_icon(uint16_t x, uint16_t y, uint16_t color)
{
    // need to do step icon
}

void display_steps_text(uint16_t x, uint16_t y, const char *steps_str, uint16_t color)
{
  // need to find step count
    display_main_text(x, y, steps_str, color);
}