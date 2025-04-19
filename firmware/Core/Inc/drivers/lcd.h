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

#define LCD_1IN28_HEIGHT 240
#define LCD_1IN28_WIDTH 240

void screen_init();

void screen_clear();

void screen_set_backlight(uint16_t Value);

void screen_render();

void screen_set_pixel(uint16_t x, uint16_t y, uint16_t color);

void set_brightness(uint8_t brightness);

void screen_render_aa();
#endif