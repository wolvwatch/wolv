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

#define RED 0x100
#define BLACK 0x000
#define GREEN 0x010
#define BLUE 0x001
#define YELLOW 0x011
#define CYAN 0x101
#define MAGENTA 0x110

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


#endif // RASTERIZER_H
