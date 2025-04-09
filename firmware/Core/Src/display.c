#include "lcd.h"
#include "rasterizer.h"
#include "display.h"
#include "Montserrat.h"
#include <stdint.h>
#include <string.h>



extern const tFont montserrat_reg;

static const char clock_font_mapping[] = "0123456789:";


static int get_glyph_index(char c) {
    if (c < 0x20 || c > 0x7E)
        return -1;
    return c - 0x20;
}

void draw_montserrat_glyph(uint16_t x, uint16_t y, char c, uint16_t color) {
    int index = get_glyph_index(c);
    if (index < 0) return;  // Character not supported.

    // Access the glyph via the font.
    const tChar *glyph = &montserrat_reg.chars[index];
    const tImage *img = glyph->image;

    // Draw the glyph using the bitmap data stored in 'img'.
    // For a 1-bit-per-pixel (monochrome) image, this might involve:
    int width  = img->width;
    int height = img->height;
    int bytes_per_row = (width + 7) / 8;
    const uint8_t *bitmap = img->data;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
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
}

void draw_montserrat_text(uint16_t x, uint16_t y, const char *text, uint16_t color) {
    uint16_t orig_x = x;
    while (*text) {
        char c = *text++;
        if (c == '\n') {
            y += 38;  // Adjust for line spacing (or use the glyph height)
            x = orig_x;
            continue;
        }
        draw_montserrat_glyph(x, y, c, color);
        // Advance x. Here you can use the glyph's width as its advance.
        int glyph_index = get_glyph_index(c);
        if (glyph_index >= 0) {
            int advance = montserrat_reg.chars[glyph_index].image->width;
            x += advance;
        } else {
            // If the glyph wasn't found, advance by a default value.
            x += 6;
        }
    }
}

void display_main_text(uint16_t x, uint16_t y, const char *text, uint16_t color)
{
    draw_montserrat_text(x, y, text, color);
}

// void display_time_text(uint16_t x, uint16_t y, const char *time_str, uint16_t color)
// {
//     draw_text(x, y, time_str, clock_font_bitmap, clock_font_dsc, clock_font_mapping, color);
// }

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