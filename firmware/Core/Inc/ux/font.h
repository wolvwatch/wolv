#ifndef FONT_H
#define FONT_H

#include <stdint.h>

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

#endif // FONT_H
