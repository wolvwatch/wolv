#include "stdint.h"
#include "rasterizer.h"
#include "text_14.h"

/*Store the image of the glyphs*/

const uint8_t text_font_bitmap[] = {
    /* U+0021 "!" */
    0xfe, 0x20,

    /* U+0022 "\"" */
    0xb6, 0xd0,

    /* U+0023 "#" */
    0x11, 0x8, 0x84, 0x8f, 0xf1, 0x21, 0x10, 0x89,
    0xff, 0x22, 0x11, 0x8, 0x80,

    /* U+0024 "$" */
    0x10, 0x21, 0xf6, 0x89, 0x12, 0x34, 0x1c, 0x16,
    0x24, 0x4c, 0xb7, 0xc2, 0x4, 0x0,

    /* U+0025 "%" */
    0x70, 0x91, 0x22, 0x24, 0x45, 0x8, 0xa0, 0xeb,
    0x82, 0x88, 0x51, 0x12, 0x24, 0x44, 0x87, 0x0,

    /* U+0026 "&" */
    0x38, 0x44, 0x44, 0x4c, 0x38, 0x30, 0x49, 0x85,
    0x86, 0xc7, 0x79,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x49, 0x49, 0x24, 0x92, 0x24, 0x80,

    /* U+0029 ")" */
    0x49, 0x12, 0x49, 0x24, 0xa4, 0x80,

    /* U+002A "*" */
    0x25, 0x5d, 0xf2, 0x0,

    /* U+002B "+" */
    0x10, 0x20, 0x47, 0xf1, 0x2, 0x4, 0x0,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x8, 0x44, 0x21, 0x10, 0x84, 0x42, 0x11, 0x8,
    0x40,

    /* U+0030 "0" */
    0x3c, 0x62, 0x42, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x42, 0x42, 0x3c,

    /* U+0031 "1" */
    0xe4, 0x92, 0x49, 0x24, 0x80,

    /* U+0032 "2" */
    0x3d, 0x8c, 0x8, 0x10, 0x20, 0x82, 0x8, 0x20,
    0x83, 0xf8,

    /* U+0033 "3" */
    0xfe, 0x8, 0x20, 0x41, 0x7, 0x81, 0x81, 0x3,
    0xd, 0xf0,

    /* U+0034 "4" */
    0x4, 0x2, 0x2, 0x2, 0x3, 0x21, 0x11, 0x9,
    0xff, 0x2, 0x1, 0x0, 0x80,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x4, 0xf, 0x81, 0x81, 0x3,
    0xd, 0xf0,

    /* U+0036 "6" */
    0x3c, 0xc1, 0x4, 0x8, 0x17, 0xb1, 0xc1, 0x82,
    0x8c, 0xf0,

    /* U+0037 "7" */
    0xff, 0x6, 0x10, 0x20, 0xc1, 0x2, 0x8, 0x10,
    0x20, 0x80,

    /* U+0038 "8" */
    0x7c, 0xc6, 0x82, 0x82, 0xc6, 0x7c, 0xc3, 0x81,
    0x81, 0xc3, 0x3c,

    /* U+0039 "9" */
    0x79, 0x8a, 0xc, 0x18, 0x28, 0xde, 0x81, 0x4,
    0x9, 0xe0,

    /* U+003A ":" */
    0x81,

    /* U+003B ";" */
    0x81, 0xc0,

    /* U+003C "<" */
    0xc, 0xcc, 0x18, 0x18, 0x10,

    /* U+003D "=" */
    0xfc, 0x0, 0x3f,

    /* U+003E ">" */
    0xc0, 0xc0, 0xc6, 0x62, 0x0,

    /* U+003F "?" */
    0x7d, 0x8c, 0x8, 0x10, 0x61, 0x82, 0x8, 0x0,
    0x0, 0x40,

    /* U+0040 "@" */
    0xf, 0x81, 0x83, 0x18, 0xc, 0x8e, 0xa8, 0x8c,
    0xc8, 0x26, 0x41, 0x32, 0x9, 0x90, 0x4c, 0x46,
    0xd1, 0xdc, 0xc0, 0x3, 0x0, 0x7, 0xc0,

    /* U+0041 "A" */
    0xc, 0x3, 0x1, 0xa0, 0x48, 0x12, 0x8, 0x42,
    0x11, 0xfe, 0x40, 0x90, 0x28, 0x4,

    /* U+0042 "B" */
    0xfe, 0x83, 0x81, 0x81, 0x83, 0xfc, 0x83, 0x81,
    0x81, 0x83, 0xfe,

    /* U+0043 "C" */
    0x1e, 0x10, 0xd0, 0x10, 0x8, 0x4, 0x2, 0x1,
    0x0, 0x40, 0x30, 0xc7, 0x80,

    /* U+0044 "D" */
    0xfc, 0x41, 0xa0, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x81, 0x41, 0xbf, 0x0,

    /* U+0045 "E" */
    0xff, 0x2, 0x4, 0x8, 0x1f, 0xa0, 0x40, 0x81,
    0x3, 0xf8,

    /* U+0046 "F" */
    0xff, 0x2, 0x4, 0x8, 0x10, 0x3f, 0x40, 0x81,
    0x2, 0x0,

    /* U+0047 "G" */
    0x1f, 0x10, 0xd0, 0x10, 0x8, 0x4, 0x6, 0x3,
    0x1, 0x40, 0x90, 0xc7, 0xc0,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81,
    0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xe0,

    /* U+004A "J" */
    0x7c, 0x10, 0x41, 0x4, 0x10, 0x41, 0x7, 0x37,
    0x80,

    /* U+004B "K" */
    0x83, 0x86, 0x84, 0x88, 0x90, 0xb0, 0xc8, 0xcc,
    0x84, 0x82, 0x81,

    /* U+004C "L" */
    0x81, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x81,
    0x3, 0xf8,

    /* U+004D "M" */
    0x80, 0x70, 0x3c, 0xe, 0x85, 0xa1, 0x64, 0x99,
    0x26, 0x31, 0x8c, 0x60, 0x18, 0x4,

    /* U+004E "N" */
    0x81, 0xc1, 0xe1, 0xa1, 0xb1, 0x99, 0x8d, 0x85,
    0x87, 0x83, 0x81,

    /* U+004F "O" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x40, 0x98, 0x61, 0xe0,

    /* U+0050 "P" */
    0xfc, 0x82, 0x81, 0x81, 0x81, 0x81, 0x82, 0xfc,
    0x80, 0x80, 0x80,

    /* U+0051 "Q" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x40, 0x98, 0x61, 0xe0, 0x18, 0x3,
    0xc0,

    /* U+0052 "R" */
    0xfc, 0x82, 0x81, 0x81, 0x81, 0x81, 0x82, 0xfc,
    0x84, 0x82, 0x81,

    /* U+0053 "S" */
    0x7d, 0x82, 0x4, 0xc, 0x7, 0x1, 0x81, 0x3,
    0xd, 0xf0,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,
    0x20, 0x40,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x42, 0x3c,

    /* U+0056 "V" */
    0x80, 0xd0, 0x24, 0x9, 0x84, 0x21, 0x8, 0xc1,
    0x20, 0x48, 0x1c, 0x3, 0x0, 0xc0,

    /* U+0057 "W" */
    0x82, 0x6, 0xc, 0x18, 0x30, 0x91, 0x42, 0x44,
    0x89, 0x12, 0x46, 0x89, 0xa, 0x14, 0x28, 0x70,
    0xc1, 0x81, 0x6, 0x0,

    /* U+0058 "X" */
    0x41, 0x30, 0x88, 0x82, 0x81, 0xc0, 0x40, 0x50,
    0x2c, 0x22, 0x20, 0x90, 0x60,

    /* U+0059 "Y" */
    0x80, 0xa0, 0x98, 0x44, 0x41, 0x40, 0xa0, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x0,

    /* U+005A "Z" */
    0xff, 0x3, 0x2, 0x4, 0x8, 0x18, 0x10, 0x20,
    0x60, 0xc0, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0xc0,

    /* U+005C "\\" */
    0x84, 0x20, 0x84, 0x20, 0x84, 0x20, 0x84, 0x20,
    0x84,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x93, 0xc0,

    /* U+005E "^" */
    0x0, 0xc3, 0x12, 0x49, 0x28, 0x40,

    /* U+005F "_" */
    0xfe,

    /* U+0060 "`" */
    0xc4,

    /* U+0061 "a" */
    0x7a, 0x30, 0x5f, 0x86, 0x18, 0xdd,

    /* U+0062 "b" */
    0x81, 0x2, 0x5, 0xcc, 0x50, 0x60, 0xc1, 0x83,
    0x8a, 0xe0,

    /* U+0063 "c" */
    0x3c, 0x8e, 0x4, 0x8, 0x10, 0x11, 0x9e,

    /* U+0064 "d" */
    0x2, 0x4, 0x9, 0xd4, 0x70, 0x60, 0xc1, 0x82,
    0x8c, 0xe8,

    /* U+0065 "e" */
    0x38, 0x8a, 0xf, 0xf8, 0x10, 0x11, 0x1e,

    /* U+0066 "f" */
    0x3a, 0x11, 0xe4, 0x21, 0x8, 0x42, 0x10,

    /* U+0067 "g" */
    0x3d, 0x43, 0x81, 0x81, 0x81, 0x81, 0x43, 0x3d,
    0x1, 0x42, 0x3c,

    /* U+0068 "h" */
    0x81, 0x2, 0x5, 0xec, 0x70, 0x60, 0xc1, 0x83,
    0x6, 0x8,

    /* U+0069 "i" */
    0x9f, 0xe0,

    /* U+006A "j" */
    0x10, 0x1, 0x11, 0x11, 0x11, 0x11, 0x16,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x28, 0x92, 0x2c, 0x6c, 0x89,
    0xa, 0x8,

    /* U+006C "l" */
    0xff, 0xe0,

    /* U+006D "m" */
    0xbc, 0xf6, 0x38, 0xe0, 0x83, 0x4, 0x18, 0x20,
    0xc1, 0x6, 0x8, 0x30, 0x41,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x1c,

    /* U+0070 "p" */
    0xb9, 0x8a, 0xc, 0x18, 0x30, 0x71, 0x5c, 0x81,
    0x2, 0x0,

    /* U+0071 "q" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x4, 0x8,

    /* U+0072 "r" */
    0xbc, 0x88, 0x88, 0x88,

    /* U+0073 "s" */
    0x7a, 0x8, 0x38, 0x1c, 0x18, 0x5e,

    /* U+0074 "t" */
    0x42, 0x3c, 0x84, 0x21, 0x8, 0x41, 0xc0,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x71, 0xbd,

    /* U+0076 "v" */
    0x82, 0x42, 0x42, 0x44, 0x24, 0x28, 0x18, 0x18,

    /* U+0077 "w" */
    0x82, 0x14, 0x61, 0x46, 0x24, 0x92, 0x29, 0x22,
    0x94, 0x30, 0xc1, 0xc,

    /* U+0078 "x" */
    0x42, 0xc8, 0xa0, 0xc1, 0x85, 0x11, 0x21,

    /* U+0079 "y" */
    0x82, 0x42, 0x42, 0x44, 0x24, 0x28, 0x18, 0x18,
    0x10, 0x30, 0xe0,

    /* U+007A "z" */
    0xfc, 0x20, 0x84, 0x21, 0x4, 0x3f,

    /* U+007B "{" */
    0x69, 0x24, 0xa2, 0x49, 0x24, 0xc0,

    /* U+007C "|" */
    0xff, 0xfc,

    /* U+007D "}" */
    0xc9, 0x24, 0x8a, 0x49, 0x25, 0x0,

    /* U+007E "~" */
    0xe6, 0x70
};


const font_t text_font_dsc[] = {
        {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 58, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 2, .adv_w = 84, .box_w = 3, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 4, .adv_w = 156, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 138, .box_w = 7, .box_h = 15, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 31, .adv_w = 186, .box_w = 11, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 47, .adv_w = 150, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 45, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 59, .adv_w = 74, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 65, .adv_w = 74, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 71, .adv_w = 86, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 6},
    {.bitmap_index = 75, .adv_w = 129, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 82, .adv_w = 47, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 83, .adv_w = 86, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 84, .adv_w = 47, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 75, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 94, .adv_w = 148, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 105, .adv_w = 81, .box_w = 3, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 110, .adv_w = 127, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 120, .adv_w = 126, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 130, .adv_w = 148, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 143, .adv_w = 127, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 136, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 163, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 173, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 136, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 47, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 47, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 197, .adv_w = 129, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 202, .adv_w = 129, .box_w = 6, .box_h = 4, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 205, .adv_w = 129, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 210, .adv_w = 127, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 220, .adv_w = 231, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 243, .adv_w = 161, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 257, .adv_w = 169, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 268, .adv_w = 159, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 281, .adv_w = 185, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 294, .adv_w = 150, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 304, .adv_w = 142, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 173, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 327, .adv_w = 182, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 338, .adv_w = 68, .box_w = 1, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 340, .adv_w = 112, .box_w = 6, .box_h = 11, .ofs_x = -1, .ofs_y = 0},
    {.bitmap_index = 349, .adv_w = 159, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 360, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 370, .adv_w = 214, .box_w = 10, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 384, .adv_w = 182, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 395, .adv_w = 188, .box_w = 10, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 161, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 188, .box_w = 10, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 437, .adv_w = 162, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 448, .adv_w = 138, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 458, .adv_w = 129, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 468, .adv_w = 177, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 479, .adv_w = 156, .box_w = 10, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 493, .adv_w = 249, .box_w = 14, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 513, .adv_w = 147, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 142, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 539, .adv_w = 146, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 550, .adv_w = 71, .box_w = 3, .box_h = 14, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 556, .adv_w = 75, .box_w = 5, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 565, .adv_w = 71, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 571, .adv_w = 129, .box_w = 6, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 577, .adv_w = 112, .box_w = 7, .box_h = 1, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 578, .adv_w = 134, .box_w = 3, .box_h = 2, .ofs_x = 2, .ofs_y = 9},
    {.bitmap_index = 579, .adv_w = 132, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 585, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 595, .adv_w = 126, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 602, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 612, .adv_w = 135, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 619, .adv_w = 76, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 637, .adv_w = 151, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 647, .adv_w = 60, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 649, .adv_w = 61, .box_w = 4, .box_h = 14, .ofs_x = -2, .ofs_y = -3},
    {.bitmap_index = 656, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 666, .adv_w = 60, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 668, .adv_w = 238, .box_w = 13, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 681, .adv_w = 151, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 688, .adv_w = 140, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 695, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 705, .adv_w = 152, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 715, .adv_w = 90, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 719, .adv_w = 109, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 725, .adv_w = 91, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 732, .adv_w = 151, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 121, .box_w = 8, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 197, .box_w = 12, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 759, .adv_w = 120, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 766, .adv_w = 121, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 777, .adv_w = 114, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 783, .adv_w = 75, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 789, .adv_w = 66, .box_w = 1, .box_h = 14, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 791, .adv_w = 75, .box_w = 3, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 797, .adv_w = 129, .box_w = 6, .box_h = 2, .ofs_x = 1, .ofs_y = 4}
};