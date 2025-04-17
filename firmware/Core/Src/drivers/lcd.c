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
#include "../Inc/drivers/lcd.h"
#include <math.h>
#include <string.h>
#include "../../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include "../Inc/main.h"

#define SET_RST_HIGH HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_SET)
#define SET_RST_LOW HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_RESET)
#define SET_CS_HIGH HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_SET)
#define SET_CS_LOW HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_RESET)
#define SET_DC_HIGH HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_SET)
#define SET_DC_LOW HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_RESET)

extern SPI_HandleTypeDef hspi3;
extern uint8_t rx_buffer[1];
static uint8_t pixels[(240 * 240 * 3) / 8];
static uint8_t aa_buffer[(240 * 240 * 3) / 8];

static void send_cmd(uint8_t reg) {
	SET_DC_LOW;
	SET_CS_LOW;
	uint8_t buf[1] = {reg};
	HAL_SPI_Transmit(&hspi3, &buf[0], 1, 500);
	SET_CS_HIGH;
}

static void send_byte(uint8_t data) {
	SET_DC_HIGH;
	SET_CS_LOW;
	uint8_t buf[1] = {data};
	HAL_SPI_Transmit(&hspi3, &buf[0], 1, 500);
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

uint8_t getPixel(const uint8_t *frameBuffer, int width, int x, int y) {
	// Calculate the overall pixel index in row-major order.
	int pixelIndex = y * width + x;
	// Each pixel occupies 3 bits.
	int bitIndex = pixelIndex * 3;
	// Determine which byte holds the first bit.
	int byteIndex = bitIndex / 8;
	// Determine the bit offset within that byte.
	int bitOffset = bitIndex % 8;

	// Case 1: The 3 bits for this pixel are all contained in the same byte.
	if (bitOffset <= 5) {
		return (frameBuffer[byteIndex] >> bitOffset) & 0x07;
	}
	// Case 2: The 3 bits span across two consecutive bytes.
	else {
		// How many bits are in the first byte.
		int bitsInFirstByte = 8 - bitOffset; // will be 1 or 2 bits.
		// The remaining bits are in the next byte.
		int bitsInSecondByte = 3 - bitsInFirstByte;

		// Extract the lower part from the first byte.
		uint8_t part1 = frameBuffer[byteIndex] >> bitOffset;
		// Extract the remaining bits from the next byte.
		uint8_t part2 = frameBuffer[byteIndex + 1] & ((1 << bitsInSecondByte) - 1);
		// Combine the two parts and mask to 3 bits.
		return ((part2 << bitsInFirstByte) | part1) & 0x07;
	}
}

uint16_t rgb111_to_rgb565(uint8_t color) {
	// Extract each 1-bit color component.
	//green is left
	//red is middle
	//blue is right
	uint8_t r = (color >> 2) & 0x01; // Red is the most significant bit.
	uint8_t g = (color >> 1) & 0x01; // Green is the middle bit.
	uint8_t b = (color >> 0) & 0x01; // Blue is the least significant bit.

	// Map the 1-bit value to the full scale of the RGB565 channel:
	// For red and blue (5 bits): 0 -> 0, 1 -> 31 (0x1F)
	// For green (6 bits): 0 -> 0, 1 -> 63 (0x3F)
	uint16_t r565 = r ? 0x1F : 0;
	uint16_t g565 = g ? 0x3F : 0;
	uint16_t b565 = b ? 0x1F : 0;

	// Combine into a single 16-bit RGB565 value.
	// RGB565 format: bits 15-11: red, bits 10-5: green, bits 4-0: blue.
	return (r565 << 11) | (g565 << 5) | b565;
}

void screen_render() {
	screen_set_windows(0, 0, 239, 239);
	SET_DC_HIGH;
	SET_CS_LOW;
	for (uint16_t j = 0; j < 240; j++) {
		for (uint16_t i = 0; i < 240; i++) {
			uint16_t pixel = rgb111_to_rgb565(getPixel(pixels, 240, i, j));
			HAL_SPI_Transmit(&hspi3, (uint8_t *) &pixel, 2, HAL_MAX_DELAY);
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
	send_byte(0x18); //Set as vertical screen

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
	set_brightness(100);
}

void setPixel(uint8_t *frameBuffer, int width, int x, int y, uint16_t color) {
	int pixelIndex = y * width + x;
	int bitIndex = pixelIndex * 3;
	int byteIndex = bitIndex / 8;
	int bitOffset = bitIndex % 8;

	if (bitOffset <= 5) {
		uint8_t mask = 0x07 << bitOffset;
		frameBuffer[byteIndex] = (frameBuffer[byteIndex] & ~mask) | ((color & 0x07) << bitOffset);
	} else {
		int bitsInFirstByte = 8 - bitOffset;
		int bitsInSecondByte = 3 - bitsInFirstByte;

		uint8_t mask1 = ((1 << bitsInFirstByte) - 1) << bitOffset;
		frameBuffer[byteIndex] = (frameBuffer[byteIndex] & ~mask1) | (((color & 0x07) << bitOffset) & mask1);
		uint8_t mask2 = (1 << bitsInSecondByte) - 1;
		frameBuffer[byteIndex + 1] = (frameBuffer[byteIndex + 1] & ~mask2) | ((color & 0x07) >> bitsInFirstByte);
	}
}

static void apply_fast_aa(const uint8_t *in_buf,
						  uint8_t       *out_buf,
						  int            width,
						  int            height)
{
	/* first: copy the whole frame.  Faster than per‑pixel border handling and
	   guarantees we do not leave stale data in out_buf.                        */
	memcpy(out_buf, in_buf, (width * height * 3) / 8);

	for (int y = 1; y < height - 1; ++y) {
		for (int x = 1; x < width  - 1; ++x) {

			int r_cnt = 0, g_cnt = 0, b_cnt = 0;

			/* count the ON bits for the 3×3 neighbourhood */
			for (int dy = -1; dy <= 1; ++dy) {
				for (int dx = -1; dx <= 1; ++dx) {
					uint8_t c = getPixel(in_buf, width, x + dx, y + dy);
					r_cnt += (c >> 2) & 1;
					g_cnt += (c >> 1) & 1;
					b_cnt +=  c       & 1;
				}
			}

			/* majority vote (≥ 5 of 9) – tweak threshold to taste            */
			uint8_t r = (r_cnt >= 5);
			uint8_t g = (g_cnt >= 5);
			uint8_t b = (b_cnt >= 5);

			/* re‑pack into RGB111                                            */
			uint8_t packed = (r << 2) | (g << 1) | b;
			setPixel(out_buf, width, x, y, packed);
		}
	}
}

void screen_render_aa() {
	// 1) run AA pass, writing into aa_buffer
	apply_fast_aa(pixels, aa_buffer, 240, 240);

	// 2) push to display exactly as before, but read from aa_buffer
	screen_set_windows(0, 0, 239, 239);
	SET_DC_HIGH;
	SET_CS_LOW;
	for (uint16_t j = 0; j < 240; j++) {
		for (uint16_t i = 0; i < 240; i++) {
			uint16_t pixel = rgb111_to_rgb565(
								getPixel(aa_buffer, 240, i, j)
							  );
			HAL_SPI_Transmit(&hspi3, (uint8_t*)&pixel, 2, HAL_MAX_DELAY);
		}
	}
	SET_DC_LOW;
	SET_CS_HIGH;
}

void screen_clear(void) {
	for (int i = 0; i < 240; i++) {
		for (int j = 0; j < 240; j++) {
			setPixel(pixels, 240, i, j, 0b000);
		}
	}
	screen_render();
}

void screen_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
	setPixel(pixels, 240, x, y, color);
}

void set_brightness(uint8_t brightness) {
	TIM1->CCR1 = brightness * 65535/100;
}
