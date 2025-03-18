/*****************************************************************************
* | File      	:   LCD_1inch28.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-06
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_1IN28_H
#define __LCD_1IN28_H

#include <stdint.h>
#include <stdlib.h>		//itoa()
#include <stdio.h>

#define LCD_1IN28_HEIGHT 240
#define LCD_1IN28_WIDTH 240

#define HORIZONTAL 0
#define VERTICAL   1

typedef struct{
	uint16_t WIDTH;
	uint16_t HEIGHT;
	uint8_t SCAN_DIR;
}LCD_1IN28_ATTRIBUTES;
extern LCD_1IN28_ATTRIBUTES LCD_1IN28;

/********************************************************************************
function:
			Macro definition variable name
********************************************************************************/
void screen_init(uint8_t scan_dir);
void screen_clear(uint16_t Color);
void screen_display(uint16_t *Image);
void screen_display_windows(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend, uint8_t *Image);
void screen_set_point(uint16_t x, uint16_t y, uint16_t color);

void screen_draw_paint(uint16_t x, uint16_t y, uint16_t Color);
void screen_set_backlight(uint16_t Value);
void screen_render();
#endif
