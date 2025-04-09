#ifndef BITMAPS_H
#define BITMAPS_H
#include <stdint.h>

typedef struct {
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} bitmap_t;

typedef struct {
    int length;
    const bitmap_t **chars;
} font_bitmap_t;

extern font_bitmap_t roboto;
extern bitmap_t heart;
extern bitmap_t steps;

#endif //BITMAPS_H
