/******************************************************************************
 * | File      	:   GUI_Paint.h
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V3.0
 * | Date        :   2019-04-18
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.0(2019-04-18):
 * 1.Change:
 *    Paint_DrawPoint(..., DOT_STYLE DOT_STYLE)
 * => Paint_DrawPoint(..., DOT_STYLE Dot_Style)
 *    Paint_DrawLine(..., LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawLine(..., DOT_PIXEL Line_width, LINE_STYLE Line_Style)
 *    Paint_DrawRectangle(..., DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawRectangle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
 *    Paint_DrawCircle(..., DRAW_FILL Draw_Fill, DOT_PIXEL Dot_Pixel)
 * => Paint_DrawCircle(..., DOT_PIXEL Line_width, DRAW_FILL Draw_Filll)
 *
 * -----------------------------------------------------------------------------
 * V2.0(2018-11-15):
 * 1.add: Paint_NewImage()
 *    Create an image's properties
 * 2.add: Paint_SelectImage()
 *    Select the picture to be drawn
 * 3.add: Paint_SetRotate()
 *    Set the direction of the cache
 * 4.add: Paint_RotateImage()
 *    Can flip the picture, Support 0-360 degrees,
 *    but only 90.180.270 rotation is better
 * 4.add: Paint_SetMirroring()
 *    Can Mirroring the picture, horizontal, vertical, origin
 * 5.add: Paint_DrawString_CN()
 *    Can display Chinese(GB1312)
 *
 * -----------------------------------------------------------------------------
 * V1.0(2018-07-17):
 *   Create library
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documnetation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to  whom the Software is
 * furished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ******************************************************************************/

#ifndef __GUI_PAINT_H
#define __GUI_PAINT_H

#include <stdint.h>
#include "bitmaps.h"
/**
 * Image attributes
 **/
typedef struct {
	volatile uint8_t *Image;
	volatile uint16_t Width;
	volatile uint16_t Height;
	volatile uint16_t WidthMemory;
	volatile uint16_t HeightMemory;
	volatile uint16_t Color;
	volatile uint16_t Rotate;
	volatile uint16_t Mirror;
	volatile uint16_t WidthByte;
	volatile uint16_t HeightByte;
} PAINT;
extern volatile PAINT Paint;

/**
 * Display rotate
 **/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
 **/
typedef enum {
	MIRROR_NONE = 0x00,
	MIRROR_HORIZONTAL = 0x01,
	MIRROR_VERTICAL = 0x02,
	MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
 **/

#define ALICEBLUE 0xDFF7
#define ANTIQUEWHITE 0x5DEF
#define AQUA 0xFF07
#define AQUAMARINE 0xFA7F
#define AZURE 0xFFF7
#define BEIGE 0xBBF7
#define BISQUE 0x38FF
#define BLACK 0x0000
#define BLANCHEDALMOND 0x59FF
#define BLUE 0xF800
#define BLUEVIOLET 0x5C89
#define BROWN 0x45A1
#define BURLYWOOD 0xD0DD
#define CADETBLUE 0xF45C
#define CHARTREUSE 0xE07F
#define CHOCOLATE 0x43D3
#define CORAL 0xEAFB
#define CORNFLOWERBLUE 0xBD64
#define CORNSILK 0xDBFF
#define CRIMSON 0xA7D8
#define CYAN 0xFF07
#define DARKBLUE 0x1100
#define DARKCYAN 0x5104
#define DARKGOLDENROD 0x21BC
#define DARKGRAY 0x55AD
#define DARKGREEN 0x2003
#define DARKGREY 0x55AD
#define DARKKHAKI 0xADBD
#define DARKMAGENTA 0x1188
#define DARKOLIVEGREEN 0x4553
#define DARKORANGE 0x60FC
#define DARKORCHID 0x9999
#define DARKRED 0x0088
#define DARKSALMON 0xAFEC
#define DARKSEAGREEN 0xF18D
#define DARKSLATEBLUE 0xF149
#define DARKSLATEGRAY 0x692A
#define DARKSLATEGREY 0x692A
#define DARKTURQUOISE 0x7A06
#define DARKVIOLET 0x1A90
#define DEEPPINK 0xB2F8
#define DEEPSKYBLUE 0xFF05
#define DIMGRAY 0x4D6B
#define DIMGREY 0x4D6B
#define DODGERBLUE 0x9F1C
#define FIREBRICK 0x04B1
#define FLORALWHITE 0xDEFF
#define FORESTGREEN 0x4424
#define FUCHSIA 0x1FF8
#define GAINSBORO 0xFBDE
#define GHOSTWHITE 0xBFF7
#define GOLD 0xA0FE
#define GOLDENROD 0x24DD
#define GRAY 0x1084
#define GREEN 0x0004
#define GREENYELLOW 0xE5AF
#define GREY 0x1084
#define HONEYDEW 0xFEF7
#define HOTPINK 0x56FB
#define INDIANRED 0xEBCA
#define INDIGO 0x1048
#define IVORY 0xFEFF
#define KHAKI 0x31F7
#define LAVENDER 0x3FE7
#define LAVENDERBLUSH 0x9EFF
#define LAWNGREEN 0xE07F
#define LEMONCHIFFON 0xD9FF
#define LIGHTBLUE 0xDCAE
#define LIGHTCORAL 0x10F4
#define LIGHTCYAN 0xFFE7
#define LIGHTGOLDENRODYELLOW 0xDAFF
#define LIGHTGRAY 0x9AD6
#define LIGHTGREEN 0x7297
#define LIGHTGREY 0x9AD6
#define LIGHTPINK 0xB8FD
#define LIGHTSALMON 0x0FFD
#define LIGHTSEAGREEN 0x9525
#define LIGHTSKYBLUE 0x7F86
#define LIGHTSLATEGRAY 0x5374
#define LIGHTSLATEGREY 0x5374
#define LIGHTSTEELBLUE 0x3BB6
#define LIGHTYELLOW 0xFCFF
#define LIME 0xE007
#define LIMEGREEN 0x6636
#define LINEN 0x9CFF
#define MAGENTA 0x1FF8
#define MAROON 0x0080
#define MEDIUMAQUAMARINE 0x7566
#define MEDIUMBLUE 0x1900
#define MEDIUMORCHID 0xBABA
#define MEDIUMPURPLE 0x9B93
#define MEDIUMSEAGREEN 0x8E3D
#define MEDIUMSLATEBLUE 0x5D7B
#define MEDIUMSPRINGGREEN 0xD307
#define MEDIUMTURQUOISE 0x994E
#define MEDIUMVIOLETRED 0xB0C0
#define MIDNIGHTBLUE 0xCE18
#define MINTCREAM 0xFFF7
#define MISTYROSE 0x3CFF
#define MOCCASIN 0x36FF
#define NAVAJOWHITE 0xF5FE
#define NAVY 0x1000
#define OLDLACE 0xBCFF
#define OLIVE 0x0084
#define OLIVEDRAB 0x646C
#define ORANGE 0x20FD
#define ORANGERED 0x20FA
#define ORCHID 0x9ADB
#define PALEGOLDENROD 0x55EF
#define PALEGREEN 0xD39F
#define PALETURQUOISE 0x7DAF
#define PALEVIOLETRED 0x92DB
#define PAPAYAWHIP 0x7AFF
#define PEACHPUFF 0xD7FE
#define PERU 0x27CC
#define PINK 0x19FE
#define PLUM 0x1BDD
#define POWDERBLUE 0xDDAE
#define PURPLE 0x1080
#define REBECCAPURPLE 0x9160
#define RED 0x001F
#define ROSYBROWN 0x71BC
#define ROYALBLUE 0x5C43
#define SADDLEBROWN 0x228A
#define SALMON 0x0EFC
#define SANDYBROWN 0x2CF5
#define SEAGREEN 0x4A2C
#define SEASHELL 0xBDFF
#define SIENNA 0x85A2
#define SILVER 0x18C6
#define SKYBLUE 0x7D86
#define SLATEBLUE 0xD96A
#define SLATEGRAY 0x1274
#define SLATEGREY 0x1274
#define SNOW 0xDFFF
#define SPRINGGREEN 0xEF07
#define STEELBLUE 0x1644
#define TAN 0xB1D5
#define TEAL 0x1004
#define THISTLE 0xFBDD
#define TOMATO 0x08FB
#define TURQUOISE 0x1A47
#define VIOLET 0x1DEC
#define WHEAT 0xF6F6
#define WHITE 0xFFFF
#define WHITESMOKE 0xBEF7
#define YELLOW 0xE0FF
#define YELLOWGREEN 0x669E


#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

typedef uint16_t color_t;

typedef enum {
	DIGITAL,
	ANALOG
} clock_face_t;

typedef struct {
	clock_face_t face;
	color_t bg_color;
	color_t txt_color;
	color_t accent_color;
} gui_settings_t;

void set_gui_settings(gui_settings_t settings);
void update_gui();
void draw_img(uint8_t x, uint8_t y, const bitmap_t *img, color_t foreground, color_t background, float scale);
void draw_current_time(uint8_t start_x, uint8_t start_y, font_bitmap_t *font, color_t foreground, color_t background, float scale);
void draw_string(uint8_t x, uint8_t y, char* str, font_bitmap_t *font, color_t foreground, color_t background, float scale);

/**
 * The size of the point
 **/
typedef enum {
	DOT_PIXEL_1X1 = 1,		// 1 x 1
	DOT_PIXEL_2X2, 		// 2 X 2
	DOT_PIXEL_3X3,		// 3 X 3
	DOT_PIXEL_4X4,		// 4 X 4
	DOT_PIXEL_5X5, 		// 5 X 5
	DOT_PIXEL_6X6, 		// 6 X 6
	DOT_PIXEL_7X7, 		// 7 X 7
	DOT_PIXEL_8X8, 		// 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
 **/
typedef enum {
	DOT_FILL_AROUND = 1,		// dot pixel 1 x 1
	DOT_FILL_RIGHTUP, 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
 **/
typedef enum {
	LINE_STYLE_SOLID = 0, LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
 **/
typedef enum {
	DRAW_FILL_EMPTY = 0, DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
 **/
typedef struct {
	uint16_t Year;  //0000
	uint8_t Month; //1 - 12
	uint8_t Day;   //1 - 30
	uint8_t Hour;  //0 - 23
	uint8_t Min;   //0 - 59
	uint8_t Sec;   //0 - 59
} PAINT_TIME;
extern PAINT_TIME sPaint_time;

//init and Clear
void Paint_NewImage(uint16_t Width, uint16_t Height, uint16_t Rotate,
		uint16_t Color);
void Paint_SelectImage(uint8_t *image);
void Paint_SetClearFunction(void (*Clear)(uint16_t));
void Paint_SetDisplayFunction(void (*Display)(uint16_t, uint16_t, uint16_t));
void Paint_SetRotate(uint16_t Rotate);
void Paint_SetMirroring(uint8_t mirror);
void Paint_SetPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);

void Paint_Clear(uint16_t Color);
void Paint_ClearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
		uint16_t Yend, uint16_t Color);

//Drawing
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
		DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
		uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width,
		LINE_STYLE Line_Style);
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
		uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Filled);
void Paint_DrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
		uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);

//pic
void Paint_DrawImage(const unsigned char *image, uint16_t Startx,
		uint16_t Starty, uint16_t Endx, uint16_t Endy);

#endif

