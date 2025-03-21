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

#include "fonts.h"
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

#define WHITE					0xFFFF
#define BLACK					0x0000
#define BLUE 					0x001F
#define BRED 					0XF81F
#define GRED 					0XFFE0
#define GBLUE					0X07FF
#define RED  					0xF800
#define MAGENTA				0xF81F
#define GREEN					0x07E0
#define CYAN 					0x7FFF
#define YELLOW				0xFFE0
#define BROWN					0XBC40
#define BRRED					0XFC07
#define GRAY 					0X8430
#define DARKBLUE			0X01CF
#define LIGHTBLUE			0X7D7C
#define GRAYBLUE      0X5458
#define LIGHTGREEN    0X841F
#define LGRAY 			  0XC618
#define LGRAYBLUE     0XA651
#define LBBLUE        0X2B12

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

typedef struct {
	const uint16_t *data;
	uint16_t width;
	uint16_t height;
	uint8_t dataSize;
} tImage;
typedef struct {
	long int code;
	const tImage *image;
} tChar;
typedef struct {
	int length;
	const tChar *chars;
} tFont;

void set_gui_settings(gui_settings_t settings);
void update_gui();
void draw_img(uint8_t x, uint8_t y, tImage img);
void draw_img_c(uint8_t x, uint8_t y, tImage img);

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

//Display string
void Paint_DrawChar(uint16_t Xstart, uint16_t Ystart, const char Acsii_Char,
		sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground);
void Paint_DrawString_EN(uint16_t Xstart, uint16_t Ystart, const char *pString,
		sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground);
void Paint_DrawString_CN(uint16_t Xstart, uint16_t Ystart, const char *pString,
		cFONT *font, uint16_t Color_Background, uint16_t Color_Foreground);
void Paint_DrawNum(uint16_t Xpoint, uint16_t Ypoint, int32_t Nummber,
		sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground);
void Paint_DrawFloatNum(uint16_t Xpoint, uint16_t Ypoint, double Nummber,
		uint8_t Decimal_Point, sFONT *Font, uint16_t Color_Background,
		uint16_t Color_Foreground);
void Paint_DrawTime(uint16_t Xstart, uint16_t Ystart, PAINT_TIME *pTime,
		sFONT *Font, uint16_t Color_Background, uint16_t Color_Foreground);

//pic
void Paint_DrawImage(const unsigned char *image, uint16_t Startx,
		uint16_t Starty, uint16_t Endx, uint16_t Endy);

#endif

