// #ifndef FONT_H
// #define FONT_H
//
// #ifdef __cplusplus
// extern "C" {
// #endif
//
// #include <stdint.h>
// #include <stdbool.h>
// #include <stdlib.h>
// #include "lcd.h"   // Must define SET_PIXEL() and color_t
//
// /* Define LVGL-specific macros if not defined.
//    For example, if your generated font data uses LV_ATTRIBUTE_LARGE_CONST,
//    you can define it as empty. */
// #ifndef LV_ATTRIBUTE_LARGE_CONST
// #define LV_ATTRIBUTE_LARGE_CONST
// #endif
//
// /***********************
//  * Font Data Structures
//  ***********************/
//
// // Glyph descriptor: one entry per glyph.
// typedef struct {
//     uint32_t bitmap_index;  /* Start index in glyph_bitmap[] */
//     uint16_t adv_w;         /* Advance width in 1/16th pixel units */
//     uint8_t box_w;          /* Width of the glyph’s bitmap (pixels) */
//     uint8_t box_h;          /* Height of the glyph’s bitmap (pixels) */
//     int8_t ofs_x;           /* X offset for drawing (pixels) */
//     int8_t ofs_y;           /* Y offset relative to baseline (pixels) */
// } lv_font_fmt_txt_glyph_dsc_t;
//
// // Character mapping: maps a range of Unicode codepoints to glyph indices.
// typedef struct {
//     uint32_t range_start;      /* Start of Unicode range */
//     uint32_t range_length;     /* Number of codepoints in this range */
//     uint32_t glyph_id_start;   /* Glyph index corresponding to range_start */
// } lv_font_fmt_txt_cmap_t;
//
// /******************************************
//  * Extern Declarations for Font Data
//  ******************************************/
//
// // These arrays must be generated externally (for example, using lv_font_conv)
// // and included in your project.
// extern const uint8_t glyph_bitmap[];                   // Bitmap data for all glyphs
// extern const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[];     // Glyph descriptors
// extern const lv_font_fmt_txt_cmap_t cmaps[];              // Character mapping info
//
// /******************************************
//  * Inline Helper: Get Glyph Index
//  ******************************************/
//
// // In this example the cmap covers codepoints 48 to 48+75-1,
// // and the glyph indices start at 1.
// // Returns 0 if the glyph is not available.
// static inline int get_glyph_index(uint32_t codepoint) {
//     if (codepoint >= 48 && codepoint < 48 + 75)
//         return 1 + (codepoint - 48);
//     return 0;
// }
//
// /******************************************
//  * Function Prototypes
//  ******************************************/
//
// // Draw the glyph with index 'glyph_index' at position (x, baseline).
// // The glyph's bitmap is drawn at (x + ofs_x, baseline + ofs_y).
// void draw_glyph(int x, int baseline, int glyph_index, color_t color);
//
// // Draw a null-terminated text string starting at (x, baseline).
// // The 'baseline' parameter is the y coordinate where the text's baseline is located.
// void draw_text(int x, int baseline, const char *text, color_t color);
//
// #ifdef __cplusplus
// }
// #endif
//
// #endif /* FONT_H */