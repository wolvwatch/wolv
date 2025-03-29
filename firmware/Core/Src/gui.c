#include "gui.h"
#include "debug.h"
#include "bitmaps.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "devices/lcd.h"
#include "stm32l4xx_hal.h"
#include "sense.h"

void draw_img(uint8_t x, uint8_t y, const bitmap_t *img, const color_t foreground, const color_t background,
              const float scale) {
    const uint16_t scaled_width = (uint16_t) (img->width * scale);
    const uint16_t scaled_height = (uint16_t) (img->height * scale);

    uint16_t bytes_per_row = (img->width + 7) / 8;
    for (uint16_t i = 0; i < scaled_height; i++) {
        const uint16_t sy = (uint16_t) (i / scale);
        for (uint16_t j = 0; j < scaled_width; j++) {
            const uint16_t sx = (uint16_t) (j / scale);

            uint16_t byte_idx = sy * bytes_per_row + (sx / 8);
            uint16_t bit_offset = 7 - (sx % 8);
            if (img->data[byte_idx] >> bit_offset & 0b1) {
                screen_set_point(j + x, i + y, foreground);
            } else {
                screen_set_point(j + x, i + y, background);
            }
        }
    }
}

void draw_string(uint8_t x, uint8_t y, char *str, font_bitmap_t *font, color_t foreground, color_t background,
                 float scale) {
    uint16_t cursor_x = x;
    char *c = &str[0];
    while (*c != '\0') {
        const bitmap_t *c_bitmap = font->chars[*c - 0x20];
        draw_img(cursor_x, y, c_bitmap, foreground, background, scale);
        cursor_x += (uint16_t) (c_bitmap->width * scale);
        ++c;
    }
}

extern RTC_HandleTypeDef hrtc;

void draw_current_time(uint8_t start_x, uint8_t start_y, font_bitmap_t *font, color_t foreground, color_t background,
                       const float scale) {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);

    char time_str[9] = {};
    sprintf(time_str, "%02u:%02u:%02u %s", time.Hours, time.Minutes, time.Seconds,
            time.TimeFormat == RTC_HOURFORMAT12_AM ? "AM" : "PM");

    draw_string(start_x, start_y, time_str, font, foreground, background, scale);
}

extern gui_data_t gui_data;

void draw_circle(uint8_t x, uint8_t y, uint8_t r, color_t foreground) {
    for (uint8_t i = 0; i < 90; i++) {
        uint8_t c_x = r * cos((i * 3.141592654) / 180.0);
        uint8_t c_y = r * sin((i * 3.141592654) / 180.0);

        screen_set_point(c_x + x, c_y + y, foreground);
        screen_set_point(-1 * c_x + x, c_y + y, foreground);
        screen_set_point(c_x + x, -1 * c_y + y, foreground);
        screen_set_point(-1 * c_x + x, -1 * c_y + y, foreground);
    }
}

void draw_line(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y, color_t foreground) {
    // Ensure from_x, y1, x2, y2 are within bounds if you want to be safe:
    // (Not mandatory—just to avoid out-of-range indexing)
    // Here we simply assume the user sends valid coordinates.

    // Bresenham's algorithm
    int dx = abs(to_x - from_x);
    int sx = (from_x < to_x) ? 1 : -1;
    int dy = -abs(to_y - from_y);
    int sy = (from_y < to_y) ? 1 : -1;
    int err = dx + dy; // error value

    int x = from_x;
    int y = from_y;

    while (1) {
        // Set the pixel if within bounds
        if (x >= 0 && x < 240 && y >= 0 && y < 240) screen_set_point(x, y, foreground);

        // If we've reached the end point, break
        if (x == to_x && y == to_y) {
            break;
        }

        // Calculate error and move point
        int e2 = 2 * err;

        if (e2 >= dy) {
            err += dy;
            x += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y += sy;
        }
    }
}

void draw_thick_line(uint8_t from_x, uint8_t from_y, uint8_t to_x, uint8_t to_y, uint8_t thickness,
                     color_t foreground) {
    // If thickness == 1, just do a normal line
    if (thickness <= 1) {
        draw_line(from_x, from_y, to_x, to_y, foreground);
        return;
    }

    // Calculate the "main" line length and the perpendicular direction.
    int dx = to_x - from_x;
    int dy = to_y - from_y;
    float length = sqrtf((float) (dx * dx + dy * dy));

    // Perpendicular direction (normalized)
    //   If the line is along (dx, dy),
    //   a perpendicular direction is (-dy, dx).
    float perpX = -(float) dy / length; // negative y-difference
    float perpY = (float) dx / length; // positive x-difference

    // Center the thickness around the main line. E.g., for thickness = 5,
    // offsets will be -2, -1, 0, 1, 2.
    int half = thickness / 2;

    for (int t = -half; t <= half; t++) {
        // Offset in the perpendicular direction by t
        float offsetX = perpX * (float) t;
        float offsetY = perpY * (float) t;

        // Round to the nearest integer offset
        int intOffsetX1 = (int) lroundf(from_x + offsetX);
        int intOffsetY1 = (int) lroundf(from_y + offsetY);
        int intOffsetX2 = (int) lroundf(to_x + offsetX);
        int intOffsetY2 = (int) lroundf(to_y + offsetY);

        // Draw a Bresenham line at this offset
        draw_line(intOffsetX1, intOffsetY1, intOffsetX2, intOffsetY2, foreground);
    }
}

const int clockTickPoints[132][2] = {
    {120, 10},
    {120, 11},
    {120, 12},
    {120, 13},
    {120, 14},
    {120, 15},
    {120, 16},
    {120, 17},
    {120, 18},
    {120, 19},
    {120, 20},
    {175, 24},
    {174, 25},
    {174, 26},
    {173, 27},
    {173, 28},
    {172, 29},
    {172, 29},
    {171, 30},
    {171, 31},
    {170, 32},
    {170, 33},
    {215, 65},
    {214, 65},
    {213, 66},
    {212, 66},
    {211, 67},
    {210, 67},
    {210, 68},
    {209, 68},
    {208, 69},
    {207, 69},
    {206, 70},
    {230, 120},
    {229, 120},
    {228, 120},
    {227, 120},
    {226, 120},
    {225, 120},
    {224, 120},
    {223, 120},
    {222, 120},
    {221, 120},
    {220, 120},
    {215, 175},
    {214, 174},
    {213, 174},
    {212, 173},
    {211, 173},
    {210, 172},
    {210, 172},
    {209, 171},
    {208, 171},
    {207, 170},
    {206, 170},
    {175, 215},
    {174, 214},
    {174, 213},
    {173, 212},
    {173, 211},
    {172, 210},
    {172, 210},
    {171, 209},
    {171, 208},
    {170, 207},
    {170, 206},
    {120, 230},
    {120, 229},
    {120, 228},
    {120, 227},
    {120, 226},
    {120, 225},
    {120, 224},
    {120, 223},
    {120, 222},
    {120, 221},
    {120, 220},
    {65, 215},
    {65, 214},
    {66, 213},
    {66, 212},
    {67, 211},
    {67, 210},
    {68, 210},
    {68, 209},
    {69, 208},
    {69, 207},
    {70, 206},
    {24, 175},
    {25, 174},
    {26, 174},
    {27, 173},
    {28, 173},
    {29, 172},
    {29, 172},
    {30, 171},
    {31, 171},
    {32, 170},
    {33, 170},
    {10, 120},
    {11, 120},
    {12, 120},
    {13, 120},
    {14, 120},
    {15, 120},
    {16, 120},
    {17, 120},
    {18, 120},
    {19, 120},
    {20, 120},
    {24, 64},
    {25, 65},
    {26, 65},
    {27, 66},
    {28, 66},
    {29, 67},
    {29, 67},
    {30, 68},
    {31, 68},
    {32, 69},
    {33, 69},
    {64, 24},
    {65, 25},
    {65, 26},
    {66, 27},
    {66, 28},
    {67, 29},
    {67, 29},
    {68, 30},
    {68, 31},
    {69, 32},
    {69, 33}
};

#define HOUR_HAND_LENGTH 90

void draw_analog_hands() {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BCD);

    uint32_t seconds = time.Seconds + time.Minutes * 60 + time.Hours * 3600;

    double second_angle = (seconds % 3600 * 12 / 3600 * 12.0) * 2 * 3.141592;
    double minute_angle = (seconds % 3600 / 3600.0) * 2 * 3.141592;
    double hour_angle = (seconds % 60 / 60.0) * 2 * 3.141592;

    double x_s = HOUR_HAND_LENGTH * cos(hour_angle);
    double y_s = HOUR_HAND_LENGTH * sin(hour_angle);

    double x_m = HOUR_HAND_LENGTH * cos(minute_angle);
    double y_m = HOUR_HAND_LENGTH * sin(minute_angle);

    double x_h = HOUR_HAND_LENGTH * 0.66 * cos(second_angle);
    double y_h = HOUR_HAND_LENGTH * 0.66 * sin(second_angle);

    screen_clear(BLACK);
    for (uint16_t i = 0; i < 132; i++) screen_set_point(clockTickPoints[i][0], clockTickPoints[i][1], WHITE);
    draw_line(120, 120, 120 + x_s, 120 + y_s, BLUE);
    draw_thick_line(120, 120, 120 + x_m, 120 + y_m, 2, WHITE);
    draw_thick_line(120, 120, 120 + x_h, 120 + y_h, 2, LIGHTGRAY);
    draw_circle(120, 120, 5, WHITE);
}

void set_gui_state(gui_state_t state) {
    screen_clear(BLACK);
    switch (state) {
        case HOME_DIGITAL: {
            draw_img(55, 145, &heart, 0xF800, BLACK, 0.12);
            draw_img(55, 175, &steps, 0x344B, BLACK, 0.12);
            draw_string(90, 180, "6281 steps", &roboto, WHITE, BLACK, 0.25);
            break;
        }

        case HOME_ANALOG: {
            draw_analog_hands();
            screen_render();
            break;
        }

        case SET_TIME_HR: {
            gui_data.set_time_hr = 0;
            gui_data.set_time_min = 0;
            gui_data.set_time_sec = 0;
            gui_data.set_time_AM = true;
            draw_string(30, 90, "00:00:00 AM", &roboto, WHITE, BLACK, 0.5);
            draw_string(30, 90, "00", &roboto, RED, BLACK, 0.5);
            screen_render();
            break;
        }
        default: break;
    }

    screen_render();
    gui_data.state = state;
}

extern sense_t sensor_data;

void update_gui() {
    switch (gui_data.state) {
        case SET_TIME_HR: {
            char time_str[9] = {};
            sprintf(time_str, "%02u:00:00 AM", gui_data.set_time_hr);
            draw_string(30, 90, time_str, &roboto, WHITE, BLACK, 0.5);
            break;
        }

        case SET_TIME_MIN: {
            char time_str[9] = {};
            sprintf(time_str, "%02u:%02u:00 AM", gui_data.set_time_hr, gui_data.set_time_min);
            draw_string(30, 90, time_str, &roboto, WHITE, BLACK, 0.5);
            break;
        }

        case SET_TIME_SEC: {
            char time_str[9] = {};
            sprintf(time_str, "%02u:%02u:%02u AM", gui_data.set_time_hr, gui_data.set_time_min, gui_data.set_time_sec);
            draw_string(30, 90, time_str, &roboto, WHITE, BLACK, 0.5);
            break;
        }

        case SET_TIME_AMPM: {
            char time_str[9] = {};
            sprintf(time_str, "%02u:%02u:%02u %s", gui_data.set_time_hr, gui_data.set_time_min, gui_data.set_time_sec,
                    gui_data.set_time_AM ? "AM" : "PM");
            draw_string(30, 90, time_str, &roboto, WHITE, BLACK, 0.5);
            break;
        }

        case HOME_DIGITAL: {
            draw_current_time(30, 90, &roboto, WHITE, BLACK, 0.5);
            char hr[4];
            if (sensor_data.val_hr) {
                sprintf(hr, "%u", sensor_data.hr);
                hr[3] = '\0';
                draw_string(90, 150, hr, &roboto, WHITE, BLACK, 0.25);
            } else {
                draw_string(90, 150, "---         ", &roboto, WHITE, BLACK, 0.25);
            }
            break;
        }

        case HOME_ANALOG: {
            draw_analog_hands();
            break;
        }

        default: break;
    }

    screen_render();
}
