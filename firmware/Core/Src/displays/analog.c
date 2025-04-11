#include "displays/analog.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "ux/display.h"
#include "displays/data.h"
#include "ux/font.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>



#define HOUR_HAND_LENGTH   50
#define MINUTE_HAND_LENGTH 80
#define SECOND_HAND_LENGTH 90

extern app_data_t g_app_data;

void watchface_analog_draw(void) {
    draw_watch_face();
    draw_hour_markers();
    draw_watch_hands(g_app_data.timeVal.hour, g_app_data.timeVal.minute, g_app_data.timeVal.second);
    draw_center_dot();
    screen_render();
}



// Draws the outer circle of the watch face.
void draw_watch_face(void) {
    draw_arc(0, 359, CENTER_X, CENTER_Y, WATCH_RADIUS, 0b111, false, 2);
}

void draw_hour_markers(void) {
    for (int i = 0; i < 12; i++) {
        // Each marker is 30 degrees apart.
        float angle = i * 30.0f;
        float rad = angle * (M_PI / 180.0f);

        int x_outer = CENTER_X + (int)(WATCH_RADIUS * cosf(rad) + 0.5f);
        int y_outer = CENTER_Y + (int)(WATCH_RADIUS * sinf(rad) + 0.5f);
        int x_inner = CENTER_X + (int)((WATCH_RADIUS - 10) * cosf(rad) + 0.5f);
        int y_inner = CENTER_Y + (int)((WATCH_RADIUS - 10) * sinf(rad) + 0.5f);

        draw_line(x_inner, y_inner, x_outer, y_outer, COLOR_BLACK, 2);
    }
}

void draw_watch_hands(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    const float degToRad = M_PI / 180.0f;

    float hour_angle = ((hours % 12) + minutes / 60.0f) * 30.0f;
    float minute_angle = minutes * 6.0f;
    float second_angle = seconds * 6.0f;

    float hour_rad = hour_angle * degToRad;
    int hour_x = CENTER_X + (int)(HOUR_HAND_LENGTH * cosf(hour_rad) + 0.5f);
    int hour_y = CENTER_Y + (int)(HOUR_HAND_LENGTH * sinf(hour_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, hour_x, hour_y, COLOR_WHITE, 4);

    float minute_rad = minute_angle * degToRad;
    int minute_x = CENTER_X + (int)(MINUTE_HAND_LENGTH * cosf(minute_rad) + 0.5f);
    int minute_y = CENTER_Y + (int)(MINUTE_HAND_LENGTH * sinf(minute_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, minute_x, minute_y, COLOR_WHITE, 3);

    float second_rad = second_angle * degToRad;
    int second_x = CENTER_X + (int)(SECOND_HAND_LENGTH * cosf(second_rad) + 0.5f);
    int second_y = CENTER_Y + (int)(SECOND_HAND_LENGTH * sinf(second_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, second_x, second_y, COLOR_RED, 1);
}

void draw_center_dot(void) {
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            int x = CENTER_X + dx;
            int y = CENTER_Y + dy;
            screen_set_pixel((uint16_t)x, (uint16_t)y, COLOR_WHITE);
        }
    }
}