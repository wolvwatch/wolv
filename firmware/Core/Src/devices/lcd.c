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
#include <math.h>
#include "stm32l4xx_hal.h"
#include "main.h"
#include <stdlib.h>
#include <stdio.h>

#include "gui.h"
#include "lvgl.h"

#define SET_RST_HIGH HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_SET)
#define SET_RST_LOW HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_RESET)
#define SET_CS_HIGH HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_SET)
#define SET_CS_LOW HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_RESET)
#define SET_DC_HIGH HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_SET)
#define SET_DC_LOW HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_RESET)

extern SPI_HandleTypeDef hspi1;
extern lv_display_t *display;
extern lv_indev_t *input;
extern lv_group_t *group;
extern uint8_t rx_buffer[1];
extern lv_subject_t hours, minutes, seconds;
static uint8_t pixels[LCD_1IN28_HEIGHT * LCD_1IN28_WIDTH * 2] = {};
extern gui_data_t gui_data;

static void send_cmd(uint8_t reg) {
	SET_DC_LOW;
	SET_CS_LOW;
	uint8_t buf[1] = {reg};
	HAL_SPI_Transmit(&hspi1, &buf[0], 1, 500);
	SET_CS_HIGH;
}

static void send_byte(uint8_t data) {
	SET_DC_HIGH;
	SET_CS_LOW;
	uint8_t buf[1] = {data};
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

void screen_render(lv_display_t *display, const lv_area_t *area, uint8_t *px_map) {
	lv_draw_sw_rgb565_swap(pixels, LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT);

	screen_set_windows(0, 0, 239, 239);
	SET_DC_HIGH;
	SET_CS_LOW;
	uint32_t bytes_remaining = 240 * 240 * 2;
	uint32_t offset = 0;
	while (bytes_remaining > 0) {
		if (bytes_remaining > 65000) {
			HAL_SPI_Transmit(&hspi1, (uint8_t *) pixels + offset, 65000, HAL_MAX_DELAY);
			offset += 65000;
			bytes_remaining -= 65000;
		} else {
			HAL_SPI_Transmit(&hspi1, (uint8_t *) pixels + offset, bytes_remaining, HAL_MAX_DELAY);
			bytes_remaining = 0;
		}
	}
	SET_DC_LOW;
	SET_CS_HIGH;

	lv_display_flush_ready(display);
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

void button_read(lv_indev_t *indev, lv_indev_data_t *data) {
	switch (*rx_buffer) {
		case 0: {
			data->key = LV_KEY_LEFT;
			break;
		}
		case 1: {
			data->key = LV_KEY_ENTER;
			break;
		}
		case 2: {
			data->key = LV_KEY_RIGHT;
			break;
		}
	}

	data->state = LV_INDEV_STATE_PR;
}

void screen_init() {
	SET_DC_HIGH;
	SET_CS_HIGH;
	SET_RST_HIGH;
	screen_reset();
	LCD_1IN28_InitReg();

	LV_FONT_DECLARE(ultra_90);
	LV_FONT_DECLARE(ultra_40);

	lv_init();
	display = lv_display_create(LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT);
	lv_display_set_flush_cb(display, screen_render);
	lv_display_set_buffers(display, pixels, NULL, sizeof(pixels), LV_DISPLAY_RENDER_MODE_FULL);

	lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x0), LV_PART_MAIN);
	lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
	lv_obj_remove_flag(lv_screen_active(), LV_OBJ_FLAG_SCROLLABLE);

	input = lv_indev_create();
	lv_indev_set_type(input, LV_INDEV_TYPE_KEYPAD);
	lv_indev_set_read_cb(input, button_read);
	lv_indev_set_mode(input, LV_INDEV_MODE_EVENT);

	group = lv_group_create();
	lv_group_set_default(group);
	lv_indev_set_group(input, group);

	lv_obj_t *h = lv_label_create(lv_screen_active());
	lv_label_bind_text(h, &gui_data.ctime_h, "%02d");
	lv_obj_set_pos(h, 22, 90);
	lv_obj_set_style_text_font(h, &ultra_90, LV_PART_MAIN);
	lv_obj_t *m = lv_label_create(lv_screen_active());
	lv_label_bind_text(m, &gui_data.ctime_m, "%02d");
	lv_obj_set_style_text_font(m, &ultra_40, LV_PART_MAIN);
	lv_obj_align_to(m, h, LV_ALIGN_TOP_RIGHT, 60, 0);
	lv_obj_t *d = lv_label_create(lv_screen_active());
	lv_label_set_text(d, "SAT 29");
	lv_obj_set_style_text_font(d, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_obj_set_style_text_color(d, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
	lv_obj_align_to(d, m, LV_ALIGN_BOTTOM_MID, 0, 20);

	lv_obj_t *s = lv_arc_create(lv_screen_active());
	lv_arc_set_rotation(s, 270);
	lv_arc_set_bg_angles(s, 0, 360);
	lv_arc_set_bg_end_angle(s, 360);
	lv_obj_remove_style(s, NULL, LV_PART_KNOB);
	lv_obj_remove_flag(s, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_set_size(s, 65, 65);
	lv_obj_center(s);
	lv_arc_set_value(s, 60);
	lv_obj_set_align(s, LV_ALIGN_BOTTOM_MID);
	lv_obj_set_pos(s, 0, -5);
	lv_obj_set_style_arc_width(s, 4, LV_PART_INDICATOR);
	lv_obj_set_style_arc_width(s, 4, LV_PART_MAIN);
	lv_obj_set_style_arc_color(s, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);

	lv_obj_t *sc = lv_label_create(lv_screen_active());
	lv_obj_set_style_text_font(sc, &lv_font_montserrat_12, LV_PART_MAIN);
	lv_label_bind_text(sc, &gui_data.ctime_s, "%02d");
	lv_obj_set_style_text_color(sc, lv_color_hex(0xAA5555), LV_PART_MAIN);
	lv_obj_align_to(sc, m, LV_ALIGN_OUT_RIGHT_BOTTOM, 3, 0);


	lv_obj_t *st = lv_label_create(lv_screen_active());
	lv_obj_set_style_text_font(st, &lv_font_montserrat_14, LV_PART_MAIN);
	lv_label_set_text(st, "6,281");
	lv_obj_align_to(st, s, LV_ALIGN_CENTER, 0, 9);

	LV_IMAGE_DECLARE(steps);
	lv_obj_t *feet = lv_image_create(lv_screen_active());
	lv_image_set_src(feet, &steps);
	lv_image_set_scale(feet, 100);
	lv_obj_align_to(feet, s, LV_ALIGN_CENTER, 0, -11);

	LV_IMAGE_DECLARE(heart);
	lv_obj_t *ht = lv_image_create(lv_screen_active());
	lv_image_set_src(ht, &heart);
	lv_image_set_scale(ht, 100);
	lv_obj_align_to(ht, d, LV_ALIGN_BOTTOM_MID, 0, 55);

	lv_obj_t *st_val = lv_label_create(lv_screen_active());
	lv_label_bind_text(st_val, &gui_data.hr, "%u");
	lv_obj_align_to(st_val, ht, LV_ALIGN_BOTTOM_MID, 0, -4);
	lv_obj_set_style_text_font(st_val, &lv_font_montserrat_12, LV_PART_MAIN);
}

void screen_clear(uint16_t color) {
	return;
}

void screen_set_point(uint16_t x, uint16_t y, uint16_t color) {
	return;
}
