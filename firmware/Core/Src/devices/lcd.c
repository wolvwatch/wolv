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
#include "devices/lcd.h"
#include "stm32l4xx_hal.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>

#define SET_RST_HIGH HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_SET)
#define SET_RST_LOW HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_RESET)
#define SET_CS_HIGH HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_SET)
#define SET_CS_LOW HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_RESET)
#define SET_DC_HIGH HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_SET)
#define SET_DC_LOW HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_RESET)

extern SPI_HandleTypeDef hspi1;
uint16_t pixels[LCD_1IN28_HEIGHT][LCD_1IN28_WIDTH] = {};

static void send_cmd(uint8_t reg) {
	SET_DC_LOW;
	SET_CS_LOW;
	uint8_t buf[1] = { reg };
	HAL_SPI_Transmit(&hspi1, &buf[0], 1, 500);
	SET_CS_HIGH;
}

static void send_byte(uint8_t data) {
	SET_DC_HIGH;
	SET_CS_LOW;
	uint8_t buf[1] = { data };
	HAL_SPI_Transmit(&hspi1, &buf[0], 1, 500);
	SET_CS_HIGH;
}

static void screen_set_windows(uint8_t Xstart, uint8_t Ystart, uint8_t Xend, uint8_t Yend) {
	//set the X coordinates
	send_cmd(0x2A);
	send_byte(0x00);
	send_byte(Xstart);
	send_byte(0x00);
	send_byte(Xend);

	//set the Y coordinates
	send_cmd(0x2B);
	send_byte(0x00);
	send_byte(Ystart);
	send_byte(0x00);
	send_byte(Yend);

	send_cmd(0X2C);
}

void screen_render() {
	screen_set_windows(0, 0, 239, 239);
    SET_DC_HIGH;
    SET_CS_LOW;
	uint32_t bytes_remaining = 240 * 240 * 2;
	uint32_t offset = 0;
	while (bytes_remaining > 0) {
		if (bytes_remaining > 65000) {
			HAL_SPI_Transmit(&hspi1, (uint8_t*) pixels + offset, 65000, HAL_MAX_DELAY);
			offset += 65000;
			bytes_remaining -= 65000;
		} else {
			HAL_SPI_Transmit(&hspi1, (uint8_t*) pixels + offset, bytes_remaining, HAL_MAX_DELAY);
			bytes_remaining = 0;
		}
	}
    SET_DC_LOW;
    SET_CS_HIGH;
}

static void screen_reset(void) {
	SET_RST_HIGH;
	HAL_Delay(100);
	SET_RST_LOW;
	HAL_Delay(100);
	SET_RST_HIGH;
	HAL_Delay(100);
}

static void LCD_1IN28_InitReg(void) {
	send_cmd(0x11);
	HAL_Delay(120);
	send_cmd(0x29);
	HAL_Delay(20);

	send_cmd(0xFE);
	send_cmd(0xEF);

	send_cmd(0xB6);
	send_byte(0x00);
	send_byte(0x20);

	send_cmd(0x36);
	send_byte(0x08); //Set as vertical screen

	send_cmd(0x3A);
	send_byte(0x05);

	send_cmd(0xC3);
	send_byte(0x13);

	send_cmd(0xC4);
	send_byte(0x13);

	send_cmd(0xC9);
	send_byte(0x22);

	send_cmd(0xF0);
	send_byte(0x45);
	send_byte(0x09);
	send_byte(0x08);
	send_byte(0x08);
	send_byte(0x26);
	send_byte(0x2A);

	send_cmd(0xF1);
	send_byte(0x43);
	send_byte(0x70);
	send_byte(0x72);
	send_byte(0x36);
	send_byte(0x37);
	send_byte(0x6F);

	send_cmd(0xF2);
	send_byte(0x45);
	send_byte(0x09);
	send_byte(0x08);
	send_byte(0x08);
	send_byte(0x26);
	send_byte(0x2A);

	send_cmd(0xF3);
	send_byte(0x43);
	send_byte(0x70);
	send_byte(0x72);
	send_byte(0x36);
	send_byte(0x37);
	send_byte(0x6F);

	send_cmd(0xCD);
	send_byte(0x63);

	send_cmd(0x70);
	send_byte(0x07);
	send_byte(0x07);
	send_byte(0x04);
	send_byte(0x0E);
	send_byte(0x0F);
	send_byte(0x09);
	send_byte(0x07);
	send_byte(0x08);
	send_byte(0x03);

	send_cmd(0xE8);
	send_byte(0x34);

	send_cmd(0x62);
	send_byte(0x18);
	send_byte(0x0D);
	send_byte(0x71);
	send_byte(0xED);
	send_byte(0x70);
	send_byte(0x70);
	send_byte(0x18);
	send_byte(0x0F);
	send_byte(0x71);
	send_byte(0xEF);
	send_byte(0x70);
	send_byte(0x70);

	send_cmd(0x63);
	send_byte(0x18);
	send_byte(0x11);
	send_byte(0x71);
	send_byte(0xF1);
	send_byte(0x70);
	send_byte(0x70);
	send_byte(0x18);
	send_byte(0x13);
	send_byte(0x71);
	send_byte(0xF3);
	send_byte(0x70);
	send_byte(0x70);

	send_cmd(0x64);
	send_byte(0x28);
	send_byte(0x29);
	send_byte(0xF1);
	send_byte(0x01);
	send_byte(0xF1);
	send_byte(0x00);
	send_byte(0x07);

	send_cmd(0x66);
	send_byte(0x3C);
	send_byte(0x00);
	send_byte(0xCD);
	send_byte(0x67);
	send_byte(0x45);
	send_byte(0x45);
	send_byte(0x10);
	send_byte(0x00);
	send_byte(0x00);
	send_byte(0x00);

	send_cmd(0x67);
	send_byte(0x00);
	send_byte(0x3C);
	send_byte(0x00);
	send_byte(0x00);
	send_byte(0x00);
	send_byte(0x01);
	send_byte(0x54);
	send_byte(0x10);
	send_byte(0x32);
	send_byte(0x98);

	send_cmd(0x74);
	send_byte(0x10);
	send_byte(0x85);
	send_byte(0x80);
	send_byte(0x00);
	send_byte(0x00);
	send_byte(0x4E);
	send_byte(0x00);

	send_cmd(0x98);
	send_byte(0x3e);
	send_byte(0x07);

	send_cmd(0x35);
	send_cmd(0x21);

	send_cmd(0xF6);
	send_byte(0b11001000);

	send_cmd(0xB0);
	send_byte(0b01100001);
}

void screen_init() {
	SET_DC_HIGH;
	SET_CS_HIGH;
	SET_RST_HIGH;
	screen_reset();
	LCD_1IN28_InitReg();
}

void screen_clear(uint16_t color) {
    for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 240; j++) {
			pixels[i][j] = color;
		}
	}
}

void screen_set_point(uint16_t x, uint16_t y, uint16_t color) {
	pixels[x][y] = (color & 0xFF) << 8 | (color & 0xFF00) >> 8;
}
