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
#include <stdbool.h>
#include <stdlib.h>
#include "stm32l4xx_hal.h"
#include "main.h"
static uint8_t luma_prev[240];
static uint8_t luma_cur[240];
static uint8_t luma_next[240];

#define EDGE_THRESHOLD_MIN   9     // tweak: ‑> lower = softer, higher = sharper
#define EDGE_THRESHOLD_MAX   15    // stops over‑blurring of very sharp edges
#define BLEND_FRAC_SHIFT     4     // 4‑bit fixed‑point for blend weight [0…16]


#define SET_RST_HIGH HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_SET)
#define SET_RST_LOW HAL_GPIO_WritePin(SCRN_RST_GPIO_Port, SCRN_RST_Pin, GPIO_PIN_RESET)
#define SET_CS_HIGH HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_SET)
#define SET_CS_LOW HAL_GPIO_WritePin(SCRN_CS_GPIO_Port, SCRN_CS_Pin, GPIO_PIN_RESET)
#define SET_DC_HIGH HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_SET)
#define SET_DC_LOW HAL_GPIO_WritePin(SCRN_DC_GPIO_Port, SCRN_DC_Pin, GPIO_PIN_RESET)


extern SPI_HandleTypeDef hspi3;
static uint16_t pixels[LCD_1IN28_HEIGHT][LCD_1IN28_WIDTH];
//static uint16_t aa_buffer[LCD_1IN28_HEIGHT][LCD_1IN28_WIDTH];

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

void screen_render() {
    //fxaa_pass(pixels);
    screen_set_windows(0, 0, 239, 239);
    SET_DC_HIGH;
    SET_CS_LOW;
    uint32_t bytes_remaining = 240 * 240 * 2;
    uint32_t offset = 0;
    while (bytes_remaining > 0) {
        if (bytes_remaining > 65000) {
            HAL_SPI_Transmit(&hspi3, (uint8_t *) pixels + offset, 65000, HAL_MAX_DELAY);
            offset += 65000;
            bytes_remaining -= 65000;
        } else {
            HAL_SPI_Transmit(&hspi3, (uint8_t *) pixels + offset, bytes_remaining, HAL_MAX_DELAY);
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

void screen_set_pixel(uint16_t x, uint16_t y, uint16_t color) {
    pixels[y][x] = color;
}

void screen_clear(uint16_t color) {
    uint16_t *p = &pixels[0][0];
    size_t count = LCD_1IN28_WIDTH * LCD_1IN28_HEIGHT;
    for (size_t i = 0; i < count; i++) {
        p[i] = color;
    }
}

void set_brightness(uint8_t brightness) {
    TIM1->CCR1 = brightness * 65535 / 100;
}

static inline uint16_t blend_channel(uint16_t colC,
                                     uint16_t colP,
                                     uint16_t colN,
                                     uint8_t w,
                                     uint16_t mask,
                                     uint8_t shift) {
    uint16_t c = (colC >> shift) & mask;
    uint16_t p = (colP >> shift) & mask;
    uint16_t n = (colN >> shift) & mask;

    uint16_t avg = (p + n) >> 1; // arithmetic mean
    uint16_t mix = (((16 - w) * c + w * avg) + 8) >> 4; // round‑correct

    return (mix > mask) ? mask : mix;
}

static inline uint8_t fast_luma(uint16_t rgb565) {
    uint8_t r = (rgb565 >> 11) * 255 / 31; // 5 bits → 0‑255
    uint8_t g = ((rgb565 >> 5) & 0x3F) * 255 / 63; // 6 bits
    uint8_t b = (rgb565 & 0x1F) * 255 / 31;

    /*  77  = 0.299*256, 150 = 0.587*256, 29 = 0.114*256  */
    return (uint8_t) ((77 * r + 150 * g + 29 * b) >> 8);
}


void fxaa_pass(uint16_t buf[240][240]) {
    for (uint16_t x = 0; x < 240; ++x) {
        luma_cur[x] = fast_luma(buf[0][x]);
        luma_next[x] = fast_luma(buf[1][x]);
    }

    for (uint16_t y = 1; y < 239; ++y) {
        memcpy(luma_prev, luma_cur, 240);
        memcpy(luma_cur, luma_next, 240);

        for (uint16_t x = 0; x < 240; ++x)
            luma_next[x] = fast_luma(buf[y + 1][x]);

        for (uint16_t x = 1; x < 239; ++x) {
            uint8_t l00 = luma_prev[x - 1], l01 = luma_prev[x], l02 = luma_prev[x + 1];
            uint8_t l10 = luma_cur[x - 1], l11 = luma_cur[x], l12 = luma_cur[x + 1];
            uint8_t l20 = luma_next[x - 1], l21 = luma_next[x], l22 = luma_next[x + 1];

            uint8_t l_min = l00;
            uint8_t l_max = l00;
#define UPD(v) do{ if((v)<l_min)l_min=(v); if((v)>l_max)l_max=(v);}while(0)
            UPD(l01);
            UPD(l02);
            UPD(l10);
            UPD(l11);
            UPD(l12);
            UPD(l20);
            UPD(l21);
            UPD(l22);
#undef UPD
            uint8_t contrast = l_max - l_min;
            if (contrast < EDGE_THRESHOLD_MIN) continue; // skip low‑contrast

            int16_t gx = (l02 + (l12 << 1) + l22) - (l00 + (l10 << 1) + l20);
            int16_t gy = (l20 + (l21 << 1) + l22) - (l00 + (l01 << 1) + l02);

            bool horiz = abs(gx) >= abs(gy);

            uint8_t l_pos = horiz ? l12 : l21;
            uint8_t l_neg = horiz ? l10 : l01;

            uint8_t dif = abs(l_pos - l_neg);
            if (dif < EDGE_THRESHOLD_MIN) continue;
            if (dif > EDGE_THRESHOLD_MAX) dif = EDGE_THRESHOLD_MAX;

            uint8_t w = (dif << BLEND_FRAC_SHIFT) / contrast; // 0…16

            uint16_t colC = buf[y][x];
            uint16_t colP = horiz ? buf[y - 1][x] : buf[y][x - 1];
            uint16_t colN = horiz ? buf[y + 1][x] : buf[y][x + 1];

            uint16_t r_out = blend_channel(colC, colP, colN, w, 0x1F, 11); // R5
            uint16_t g_out = blend_channel(colC, colP, colN, w, 0x3F, 5); // G6
            uint16_t b_out = blend_channel(colC, colP, colN, w, 0x1F, 0); // B5

            buf[y][x] = (r_out << 11) | (g_out << 5) | b_out;

            buf[y][x] = (r_out << 11) | (g_out << 5) | b_out;
        }
    }
}
