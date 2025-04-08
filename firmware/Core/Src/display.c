#include "lcd.h"
#include "rasterizer.h"
#include "font.h"
#include "display.h"
#include "clock.h"
#include "text_14.h"
#include <stdint.h>
#include <string.h>


extern const uint8_t text_font_bitmap[];    // text_14.c
extern const font_t text_font_dsc[];          // text_14.c

extern const uint8_t clock_font_bitmap[];   // clock.c
extern const font_t clock_font_dsc[];         // clock.c

// mapping tables for the fonts

static const char text_font_mapping[] =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

static const char clock_font_mapping[] = "0123456789:";


static int get_glyph_index(const char *mapping, char c)
{
    int index = 1;
    for (const char *p = mapping; *p; p++, index++) {
        if (*p == c)
            return index;
    }
    return -1;  // character not supported
}

/*
typedef struct {
    uint16_t bitmap_index;  // Index into the bitmap array for the glyph's data
    uint16_t adv_w;         // Advance width in 1/16th pixels
    uint8_t box_w;          // Glyph bounding box width
    uint8_t box_h;          // Glyph bounding box height
    int8_t ofs_x;           // Horizontal offset from the cursor position
    int8_t ofs_y;           // Vertical offset from the baseline
} font_t;
 */
void draw_glyph(uint16_t x, uint16_t y,
                int glyph_index,
                const uint8_t *glyph_bitmap,
                const font_t *glyph_dsc,
                uint16_t color)
{
    if (glyph_index <= 0) return;

    const font_t *gd = &glyph_dsc[glyph_index];
    int width = gd->box_w;
    int height = gd->box_h;
    int bytes_per_row = (width + 7) / 8;
    const uint8_t *bitmap_start = &glyph_bitmap[gd->bitmap_index];

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int byte_offset = row * bytes_per_row + (col / 8);
            int bit_mask = 0x80 >> (col % 8);

            if (bitmap_start[byte_offset] & bit_mask) {
                int pixel_x = x + col + gd->ofs_x;
                int pixel_y = y + row + gd->ofs_y;

                if (pixel_x >= 0 && pixel_x < LCD_1IN28_WIDTH &&
                    pixel_y >= 0 && pixel_y < LCD_1IN28_HEIGHT) {
                    screen_set_pixel((uint16_t)pixel_x, (uint16_t)pixel_y, color);
                    }
            }
        }
    }
}

void draw_text(uint16_t x, uint16_t y, const char *text,
               const uint8_t *glyph_bitmap, const font_t *glyph_dsc,
               const char *mapping, uint16_t color)
{
    uint16_t orig_x = x;  // Store the starting x coordinate

    while (*text) {
        char c = *text++;
        if (c == '\n') {
            y += 16;   // Adjust line height as needed
            x = orig_x;
            continue;
        }
        int glyph_index = get_glyph_index(mapping, c);
        if (glyph_index < 0) {
            // blank space advance
            x += 8;
            continue;
        }
        draw_glyph(x, y, glyph_index, glyph_bitmap, glyph_dsc, color);
        int adv_from_metric = (glyph_dsc[glyph_index].adv_w + 8) >> 4;

        int drawn_right = glyph_dsc[glyph_index].ofs_x + glyph_dsc[glyph_index].box_w;
        x += drawn_right;

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