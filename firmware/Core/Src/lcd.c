/*****************************************************************************
 * | File      	:   LCD_1inch28.c
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
#include "lcd.h"
#include "stm32l4xx_hal.h"
#include "main.h"
#include <stdlib.h>		//itoa()
#include <stdio.h>

#define SET_RST_HIGH HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_SET)
#define SET_RST_LOW HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_RESET)
#define SET_CS_HIGH HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_SET)
#define SET_CS_LOW HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_RESET)
#define SET_DC_HIGH HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_SET)
#define SET_DC_LOW HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_RESET)

LCD_1IN28_ATTRIBUTES LCD_1IN28;
extern SPI_HandleTypeDef hspi1;

uint8_t delta_modified = 0;
uint16_t dt_x_min;
uint16_t dt_x_max;
uint16_t dt_y_min;
uint16_t dt_y_max;

uint16_t pixels[LCD_1IN28_HEIGHT][LCD_1IN28_WIDTH] = {};

static void _update_delta(uint16_t min_x, uint16_t min_y, uint16_t max_x, uint16_t max_y) {
	if (!delta_modified || min_x < dt_x_min) dt_x_min = min_x;
	if (!delta_modified || max_x > dt_x_max) dt_x_max = max_x;
	if (!delta_modified || min_y < dt_y_min) dt_y_min = min_y;
	if (!delta_modified || max_y > dt_y_max) dt_y_max = max_y;
	delta_modified = 1;
}

void screen_render() {
	uint16_t y_diff = dt_y_max - dt_y_min + 1;
	uint16_t x_diff = dt_x_max - dt_x_min + 1;
	uint32_t n_pix = y_diff * x_diff;

	int index = 0;
	uint16_t buf[n_pix] = {};
	for (int i = dt_y_min; i <= dt_y_max; i++) {
		for (int j = dt_x_min; j <= dt_x_max; j++) buf[index++] = pixels[i][j];
	}

	screen_set_windows(dt_x_min, dt_y_min, dt_x_max, dt_y_max);
    SET_DC_HIGH;
    SET_CS_LOW;
	uint32_t bytes_remaining = n_pix * 2;
	uint32_t offset = 0;
	while (bytes_remaining > 0) {
		if (bytes_remaining > 60000) {
			HAL_SPI_Transmit(&hspi1, (uint8_t*) buf + offset, 60000, HAL_MAX_DELAY);
			offset += 60000;
			bytes_remaining -= 60000;
		} else {
			HAL_SPI_Transmit(&hspi1, (uint8_t*) buf + offset, bytes_remaining, HAL_MAX_DELAY);
			bytes_remaining = 0;
		}
	}
    SET_DC_LOW;
    SET_CS_HIGH;


	delta_modified = 0;
}
/******************************************************************************
 function :	Hardware reset
 parameter:
 ******************************************************************************/
static void screen_reset(void) {
	SET_RST_HIGH;
	HAL_Delay(100);
	SET_RST_LOW;
	HAL_Delay(100);
	SET_RST_HIGH;
	HAL_Delay(100);
}

/******************************************************************************
 function :	send command
 parameter:
 Reg : Command register
 ******************************************************************************/
static void LCD_1IN28_SendCommand(uint8_t reg) {
	SET_DC_LOW;
	SET_CS_LOW;
	uint8_t buf[1] = { reg };
	HAL_SPI_Transmit(&hspi1, &buf[0], 1, 500);
	SET_CS_HIGH;
}

/******************************************************************************
 function :	send data
 parameter:
 Data : Write data
 ******************************************************************************/
static void LCD_1IN28_SendData_8Bit(uint8_t data) {
	SET_DC_HIGH;
	SET_CS_LOW;
	uint8_t buf[1] = { data };
	HAL_SPI_Transmit(&hspi1, &buf[0], 1, 500);
	SET_CS_HIGH;
}

/******************************************************************************
 function :	send data
 parameter:
 Data : Write data
 ******************************************************************************/
static void LCD_1IN28_SendData_16Bit(uint16_t data) {
	SET_DC_HIGH;
	SET_CS_LOW;
	uint8_t buf[2] = { data >> 8, data & 0xFF };
	HAL_SPI_Transmit(&hspi1, &buf[0], 2, 500);
	SET_CS_HIGH;
}

/******************************************************************************
 function :	Initialize the lcd register
 parameter:
 ******************************************************************************/
static void LCD_1IN28_InitReg(void) {
	LCD_1IN28_SendCommand(0xEF);
	LCD_1IN28_SendCommand(0xEB);
	LCD_1IN28_SendData_8Bit(0x14);

	LCD_1IN28_SendCommand(0xFE);
	LCD_1IN28_SendCommand(0xEF);

	LCD_1IN28_SendCommand(0xEB);
	LCD_1IN28_SendData_8Bit(0x14);

	LCD_1IN28_SendCommand(0x84);
	LCD_1IN28_SendData_8Bit(0x40);

	LCD_1IN28_SendCommand(0x85);
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x86);
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x87);
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x88);
	LCD_1IN28_SendData_8Bit(0x0A);

	LCD_1IN28_SendCommand(0x89);
	LCD_1IN28_SendData_8Bit(0x21);

	LCD_1IN28_SendCommand(0x8A);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0x8B);
	LCD_1IN28_SendData_8Bit(0x80);

	LCD_1IN28_SendCommand(0x8C);
	LCD_1IN28_SendData_8Bit(0x01);

	LCD_1IN28_SendCommand(0x8D);
	LCD_1IN28_SendData_8Bit(0x01);

	LCD_1IN28_SendCommand(0x8E);
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x8F);
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0xB6);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x20);

	LCD_1IN28_SendCommand(0x36);
	LCD_1IN28_SendData_8Bit(0x08); //Set as vertical screen

	LCD_1IN28_SendCommand(0x3A);
	LCD_1IN28_SendData_8Bit(0x05);

	LCD_1IN28_SendCommand(0x90);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);

	LCD_1IN28_SendCommand(0xBD);
	LCD_1IN28_SendData_8Bit(0x06);

	LCD_1IN28_SendCommand(0xBC);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0xFF);
	LCD_1IN28_SendData_8Bit(0x60);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x04);

	LCD_1IN28_SendCommand(0xC3);
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendCommand(0xC4);
	LCD_1IN28_SendData_8Bit(0x13);

	LCD_1IN28_SendCommand(0xC9);
	LCD_1IN28_SendData_8Bit(0x22);

	LCD_1IN28_SendCommand(0xBE);
	LCD_1IN28_SendData_8Bit(0x11);

	LCD_1IN28_SendCommand(0xE1);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x0E);

	LCD_1IN28_SendCommand(0xDF);
	LCD_1IN28_SendData_8Bit(0x21);
	LCD_1IN28_SendData_8Bit(0x0c);
	LCD_1IN28_SendData_8Bit(0x02);

	LCD_1IN28_SendCommand(0xF0);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x26);
	LCD_1IN28_SendData_8Bit(0x2A);

	LCD_1IN28_SendCommand(0xF1);
	LCD_1IN28_SendData_8Bit(0x43);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x72);
	LCD_1IN28_SendData_8Bit(0x36);
	LCD_1IN28_SendData_8Bit(0x37);
	LCD_1IN28_SendData_8Bit(0x6F);

	LCD_1IN28_SendCommand(0xF2);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x26);
	LCD_1IN28_SendData_8Bit(0x2A);

	LCD_1IN28_SendCommand(0xF3);
	LCD_1IN28_SendData_8Bit(0x43);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x72);
	LCD_1IN28_SendData_8Bit(0x36);
	LCD_1IN28_SendData_8Bit(0x37);
	LCD_1IN28_SendData_8Bit(0x6F);

	LCD_1IN28_SendCommand(0xED);
	LCD_1IN28_SendData_8Bit(0x1B);
	LCD_1IN28_SendData_8Bit(0x0B);

	LCD_1IN28_SendCommand(0xAE);
	LCD_1IN28_SendData_8Bit(0x77);

	LCD_1IN28_SendCommand(0xCD);
	LCD_1IN28_SendData_8Bit(0x63);

	LCD_1IN28_SendCommand(0x70);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x04);
	LCD_1IN28_SendData_8Bit(0x0E);
	LCD_1IN28_SendData_8Bit(0x0F);
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x03);

	LCD_1IN28_SendCommand(0xE8);
	LCD_1IN28_SendData_8Bit(0x34);

	LCD_1IN28_SendCommand(0x62);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0D);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xED);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0F);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xEF);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x63);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x11);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF3);
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x64);
	LCD_1IN28_SendData_8Bit(0x28);
	LCD_1IN28_SendData_8Bit(0x29);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x66);
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0xCD);
	LCD_1IN28_SendData_8Bit(0x67);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0x67);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x54);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x32);
	LCD_1IN28_SendData_8Bit(0x98);

	LCD_1IN28_SendCommand(0x74);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x85);
	LCD_1IN28_SendData_8Bit(0x80);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x4E);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0x98);
	LCD_1IN28_SendData_8Bit(0x3e);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x35);
	LCD_1IN28_SendCommand(0x21);

	LCD_1IN28_SendCommand(0x11);
	HAL_Delay(120);
	LCD_1IN28_SendCommand(0x29);
	HAL_Delay(20);
}

/********************************************************************************
 function:	Set the resolution and scanning method of the screen
 parameter:
 Scan_dir:   Scan direction
 ********************************************************************************/
static void LCD_1IN28_SetAttributes(uint8_t scan_dir) {
	//Get the screen scan direction
	LCD_1IN28.SCAN_DIR = scan_dir;
	uint8_t MemoryAccessReg = 0x08;

	//Get GRAM and LCD width and height
	if (scan_dir == HORIZONTAL) {
		LCD_1IN28.HEIGHT = LCD_1IN28_HEIGHT;
		LCD_1IN28.WIDTH = LCD_1IN28_WIDTH;
		MemoryAccessReg = 0XC8;
	} else {
		LCD_1IN28.HEIGHT = LCD_1IN28_WIDTH;
		LCD_1IN28.WIDTH = LCD_1IN28_HEIGHT;
		MemoryAccessReg = 0X68;
	}

	// Set the read / write scan direction of the frame memory
	LCD_1IN28_SendCommand(0x36); //MX, MY, RGB mode
	LCD_1IN28_SendData_8Bit(MemoryAccessReg);	//0x08 set RGB
}

/********************************************************************************
 function :	Initialize the lcd
 parameter:
 ********************************************************************************/
void screen_init(uint8_t scan_dir) {
	SET_DC_HIGH;
	SET_CS_HIGH;
	SET_RST_HIGH;
	//Hardware reset
	screen_reset();
	//Set the resolution and scanning method of the screen
	LCD_1IN28_SetAttributes(scan_dir);
	//Set the initialization register
	LCD_1IN28_InitReg();
}

/********************************************************************************
 function:	Sets the start position and size of the display area
 parameter:
 Xstart 	:   X direction Start coordinates
 Ystart  :   Y direction Start coordinates
 Xend    :   X direction end coordinates
 Yend    :   Y direction end coordinates
 ********************************************************************************/
void screen_set_windows(uint8_t Xstart, uint8_t Ystart, uint8_t Xend,
		uint8_t Yend) {
	//set the X coordinates
	LCD_1IN28_SendCommand(0x2A);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(Xstart);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(Xend);

	//set the Y coordinates
	LCD_1IN28_SendCommand(0x2B);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(Ystart);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(Yend);

	LCD_1IN28_SendCommand(0X2C);
}

/******************************************************************************
 function :	Clear screen
 parameter:
 ******************************************************************************/
void screen_clear(uint16_t color) {
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 240; j++) {
			pixels[i][j] = color;
		}
	}
	_update_delta(0, 0, 239, 239);
}

void screen_set_point(uint16_t x, uint16_t y, uint16_t color) {
	pixels[y][x] = color;
	_update_delta(x, y, x, y);
}


/******************************************************************************
 function :	Sends the image buffer in RAM to displays
 parameter:
 ******************************************************************************/
void screen_display(uint16_t *img) {
	uint16_t i, j;
	screen_set_windows(0, 0, LCD_1IN28_WIDTH - 1, LCD_1IN28_HEIGHT - 1);
	SET_DC_HIGH;
	uint16_t buf[LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT];
	for (i = 0; i < LCD_1IN28_WIDTH; i++) {
		for (j = 0; j < LCD_1IN28_HEIGHT; j++) {
			buf[i * j] = *(img + i * LCD_1IN28_WIDTH + j);
		}
	}
}

void screen_display_windows(uint8_t Xstart, uint8_t Ystart, uint8_t Xend,
		uint8_t Yend, uint8_t *Image) {
	// display
	uint32_t Addr = 0;

	uint16_t i, j;
	screen_set_windows(Xstart, Ystart, Xend - 1, Yend - 1);
	//LCD_1IN28_DC_1;
	for (i = Ystart; i < Yend - 1; i++) {
		Addr = Xstart + i * LCD_1IN28_WIDTH;
		for (j = Xstart; j < Xend - 1; j++) {
			//DEV_SPI_WRITE(*(Image+Addr+j)>>8);
			//DEV_SPI_WRITE(*(Image+Addr+j));
		}
	}
}

/******************************************************************************
 function: Draw a point
 parameter	:
 X	: 	Set the X coordinate
 Y	:	Set the Y coordinate
 Color :	Set the color
 ******************************************************************************/
void screen_draw_paint(uint16_t x, uint16_t y, uint16_t Color) {
	screen_set_windows(x,y,x,y);
	LCD_1IN28_SendData_16Bit(Color);
}

/*******************************************************************************
 function:
 Setting backlight
 parameter	:
 value : Range 0~1000   Duty cycle is value/1000
 *******************************************************************************/
void LCD_1IN28_SetBackLight(uint16_t Value) {
	//DEV_Set_PWM(Value);
}
