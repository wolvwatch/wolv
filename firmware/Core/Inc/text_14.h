#ifndef TEXT_FONT_H
#define TEXT_FONT_H

#include <stdint.h>
#include "rasterizer.h"

#ifdef __cplusplus
extern "C" {
#endif

    extern const uint8_t text_bitmap[];

    extern const font_t text_font_dsc[];

#ifdef __cplusplus
}
#endif

#endif /* TEXT_FONT_H */