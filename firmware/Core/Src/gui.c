/******************************************************************************
 * | File      	:   GUI_Paint.c
 * | Author      :   Waveshare electronics
 * | Function    :	Achieve drawing: draw points, lines, boxes, circles and
 *                   their size, solid dotted line, solid rectangle hollow
 *                   rectangle, solid circle hollow circle.
 * | Info        :
 *   Achieve display characters: Display a single character, string, number
 *   Achieve time display: adaptive size display time minutes and seconds
 *----------------
 * |	This version:   V3.1
 * | Date        :   2020-08-15
 * | Info        :
 * -----------------------------------------------------------------------------
 * V3.1(2020-08-15):
 * 1.Fix:
 *       Paint_DrawNum
 *         Fixed a BUG where the Paint_DrawNum function failed to display 0
 * 2.Add： Paint_DrawFloatNum
 *     Can display FloatNum
 *
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

#include "gui.h"
#include "Debug.h"
#include "bitmaps.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include <math.h>

volatile PAINT Paint;

void (*DISPLAY)(uint16_t, uint16_t, uint16_t);

void (*CLEAR)(uint16_t);

/******************************************************************************
 function:	Create Image
 parameter:
 image   :   Pointer to the image cache
 width   :   The width of the picture
 Height  :   The height of the picture
 Color   :   Whether the picture is inverted
 ******************************************************************************/
void Paint_NewImage(uint16_t Width, uint16_t Height, uint16_t Rotate,
                    uint16_t Color) {
    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;
    Paint.WidthByte = Width;
    Paint.HeightByte = Height;
    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte,
           Paint.HeightByte);

    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;

    if (Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        Paint.Width = Width;
        Paint.Height = Height;
    } else {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}

/******************************************************************************
 function:	Select Clear Funtion
 parameter:
 Clear :   Pointer to Clear funtion
 ******************************************************************************/
void Paint_SetClearFunction(void (*Clear)(uint16_t)) {
    CLEAR = Clear;
}

/******************************************************************************
 function:	Select DisplayF untion
 parameter:
 Display :   Pointer to display funtion
 ******************************************************************************/
void Paint_SetDisplayFunction(void (*Display)(uint16_t, uint16_t, uint16_t)) {
    DISPLAY = Display;
}

/******************************************************************************
 function:	Select Image Rotate
 parameter:
 Rotate   :   0,90,180,270
 ******************************************************************************/
void Paint_SetRotate(uint16_t Rotate) {
    if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180
        || Rotate == ROTATE_270) {
        Debug("Set image Rotate %d\r\n", Rotate);
        Paint.Rotate = Rotate;
    } else {
        Debug("rotate = 0, 90, 180, 270\r\n");
        //  exit(0);
    }
}

/******************************************************************************
 function:	Select Image mirror
 parameter:
 mirror   :       Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
 ******************************************************************************/
void Paint_SetMirroring(uint8_t mirror) {
    if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL
        || mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
        Debug("mirror image x:%s, y:%s\r\n",
              (mirror & 0x01) ? "mirror" : "none",
              ((mirror >> 1) & 0x01) ? "mirror" : "none");
        Paint.Mirror = mirror;
    } else {
        Debug(
            "mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
        //exit(0);
    }
}

/******************************************************************************
 function:	Draw Pixels
 parameter:
 Xpoint  :   At point X
 Ypoint  :   At point Y
 Color   :   Painted colors
 ******************************************************************************/
void Paint_SetPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color) {
    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        Debug("Exceeding display boundaries\r\n");
        return;
    }
    uint16_t X, Y;

    switch (Paint.Rotate) {
        case 0:
            X = Xpoint;
            Y = Ypoint;
            break;
        case 90:
            X = Paint.WidthMemory - Ypoint - 1;
            Y = Xpoint;
            break;
        case 180:
            X = Paint.WidthMemory - Xpoint - 1;
            Y = Paint.HeightMemory - Ypoint - 1;
            break;
        case 270:
            X = Ypoint;
            Y = Paint.HeightMemory - Xpoint - 1;
            break;

        default:
            return;
    }

    switch (Paint.Mirror) {
        case MIRROR_NONE:
            break;
        case MIRROR_HORIZONTAL:
            X = Paint.WidthMemory - X - 1;
            break;
        case MIRROR_VERTICAL:
            Y = Paint.HeightMemory - Y - 1;
            break;
        case MIRROR_ORIGIN:
            X = Paint.WidthMemory - X - 1;
            Y = Paint.HeightMemory - Y - 1;
            break;
        default:
            return;
    }

    // printf("x = %d, y = %d\r\n", X, Y);
    if (X > Paint.WidthMemory || Y > Paint.HeightMemory) {
        Debug("Exceeding display boundaries\r\n");
        return;
    }

    // UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
    DISPLAY(X, Y, Color);
}

/******************************************************************************
 function:	Clear the color of the picture
 parameter:
 Color   :   Painted colors
 ******************************************************************************/
void Paint_Clear(uint16_t Color) {
    CLEAR(Color);
}

/******************************************************************************
 function:	Clear the color of a window
 parameter:
 Xstart :   x starting point
 Ystart :   Y starting point
 Xend   :   x end point
 Yend   :   y end point
 ******************************************************************************/
void Paint_ClearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                        uint16_t Yend, uint16_t Color) {
    uint16_t X, Y;
    for (Y = Ystart; Y < Yend; Y++) {
        for (X = Xstart; X < Xend; X++) {
            //8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
 function:	Draw Point(Xpoint, Ypoint) Fill the color
 parameter:
 Xpoint		:   The Xpoint coordinate of the point
 Ypoint		:   The Ypoint coordinate of the point
 Color		:   Set color
 Dot_Pixel	:	point size
 ******************************************************************************/
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay) {
    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        Debug("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    int16_t XDir_Num, YDir_Num;
    if (Dot_FillWay == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if (Xpoint + XDir_Num - Dot_Pixel < 0
                    || Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                // printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel,
                               Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1,
                               Color);
            }
        }
    }
}

/******************************************************************************
 function:	Draw a line of arbitrary slope
 parameter:
 Xstart ：Starting Xpoint point coordinates
 Ystart ：Starting Xpoint point coordinates
 Xend   ：End point Xpoint coordinate
 Yend   ：End point Ypoint coordinate
 Color  ：The color of the line segment
 ******************************************************************************/
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                    uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width,
                    LINE_STYLE Line_Style) {
    if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width
        || Yend > Paint.Height) {
        Debug("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    uint16_t Xpoint = Xstart;
    uint16_t Ypoint = Ystart;
    int dx = (int) Xend - (int) Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int) Yend - (int) Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    //Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        //Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //Debug("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width,
                            DOT_STYLE_DFT);
            Dotted_Len = 0;
        } else {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
 function:	Draw a rectangle
 parameter:
 Xstart ：Rectangular  Starting Xpoint point coordinates
 Ystart ：Rectangular  Starting Xpoint point coordinates
 Xend   ：Rectangular  End point Xpoint coordinate
 Yend   ：Rectangular  End point Ypoint coordinate
 Color  ：The color of the Rectangular segment
 Filled : Whether it is filled--- 1 solid 0：empty
 ******************************************************************************/
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                         uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Filled) {
    if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width
        || Yend > Paint.Height) {
        Debug("Input exceeds the normal display range\r\n");
        return;
    }

    if (Filled) {
        uint16_t Ypoint;
        for (Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color, Line_width,
                           LINE_STYLE_SOLID);
        }
    } else {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width,
                       LINE_STYLE_SOLID);
    }
}

/******************************************************************************
 function:	Use the 8-point method to draw a circle of the
 specified size at the specified position->
 parameter:
 X_Center  ：Center X coordinate
 Y_Center  ：Center Y coordinate
 Radius    ：circle Radius
 Color     ：The color of the ：circle segment
 Filled    : Whether it is filled: 1 filling 0：Do not
 ******************************************************************************/
void Paint_DrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
                      uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill) {
    if (X_Center > Paint.Width || Y_Center >= Paint.Height) {
        Debug("Paint_DrawCircle Input exceeds the normal display range\r\n");
        return;
    }

    //Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    //Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1);

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent) {
            //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++) {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT); //7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    } else {
        //Draw a hollow circle
        while (XCurrent <= YCurrent) {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); //0

            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
}

/******************************************************************************
 function:	Display image
 parameter:
 image            ：Image start address
 xStart           : X starting coordinates
 yStart           : Y starting coordinates
 xEnd             ：Image width
 yEnd             : Image height
 ******************************************************************************/
void Paint_DrawImage(const unsigned char *image, uint16_t xStart,
                     uint16_t yStart, uint16_t W_Image, uint16_t H_Image) {
    int i, j;
    for (j = 0; j < H_Image; j++) {
        for (i = 0; i < W_Image; i++) {
            if (xStart + i < Paint.WidthMemory
                && yStart + j < Paint.HeightMemory) //Exceeded part does not display
                Paint_SetPixel(xStart + i, yStart + j,
                               (*(image + j * W_Image * 2 + i * 2 + 1)) << 8
                               | (*(image + j * W_Image * 2 + i * 2)));
            //Using arrays is a property of sequential storage, accessing the original array by algorithm
            //j*W_Image*2 			   Y offset
            //i*2              	   X offset
        }
    }
}

gui_settings_t settings;

void set_gui_settings(gui_settings_t s) {
    settings = s;
}

void draw_img(uint8_t x, uint8_t y, const bitmap_t *img, const color_t foreground, const color_t background,
              const float scale) {
    const uint16_t scaled_width = (uint16_t) (img->width * scale);
    const uint16_t scaled_height = (uint16_t) (img->height * scale);

    uint16_t bytes_per_row = (img->width + 7) / 8;
    for (int i = 0; i < scaled_height; i++) {
        const uint16_t sy = (uint16_t) (i / scale);
        for (int j = 0; j < scaled_width; j++) {
            const uint16_t sx = (uint16_t) (j / scale);

            uint16_t byte_idx = sy * bytes_per_row + (sx / 8);
            uint16_t bit_offset = 7 - (sx % 8);
            if (img->data[byte_idx] >> bit_offset & 0b1) {
                Paint_SetPixel(j + x, i + y, foreground);
            } else {
                Paint_SetPixel(j + x, i + y, background);
            }
        }
    }
}

void draw_string(uint8_t x, uint8_t y, char* str, font_bitmap_t *font, color_t foreground, color_t background, float scale) {
    uint16_t cursor_x = x;
    char *c = &str[0];
    while (*c != '\0') {
        const bitmap_t *c_bitmap = font->chars[*c - 0x20];
        draw_img(cursor_x, y, c_bitmap, foreground, background, scale);
        cursor_x += (uint16_t) (c_bitmap->width * scale);
        ++c;
    }
}

void draw_time(uint8_t start_x, uint8_t start_y, color_t foreground, color_t background, const float scale,
               font_bitmap_t *font, uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t am) {

}

void update_gui() {
}
