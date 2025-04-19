#include "displays/analog.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "ux/display.h"
#include "displays/data.h"
#include "ux/bitmaps.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

#define HOUR_HAND_LENGTH   50
#define MINUTE_HAND_LENGTH 80
#define SECOND_HAND_LENGTH 90
#define PROGRESS_RADIUS    30
#define GOAL_STEPS        10000

extern app_data_t g_app_data;
extern tImage hr;
extern tImage battery;

void draw_biometric_data(void) {
    char date_str[10];
    char hr_str[10];
    char battery_str[10];
    char steps_str[10];
    char dayStr[4];
    char dateStr[8];

    // day of week
    int dayOfWeek = (g_app_data.timeVal.day + 2 * g_app_data.timeVal.month +
                    3 * (g_app_data.timeVal.month + 1) / 5 + g_app_data.timeVal.year +
                    g_app_data.timeVal.year / 4 - g_app_data.timeVal.year / 100 +
                    g_app_data.timeVal.year / 400) % 7;

    const char* days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    snprintf(dayStr, sizeof(dayStr), "%s", days[dayOfWeek]);
    snprintf(dateStr, sizeof(dateStr), "%02d / %02d",
             g_app_data.timeVal.month, g_app_data.timeVal.day);

    // day
    draw_text(dayStr, CENTER_X, 90, &montserrat_reg, COLOR_WHITE, 0.75, true);

    // date
    snprintf(date_str, sizeof(date_str), "%d / %d",
             g_app_data.timeVal.month,
             g_app_data.timeVal.day);
    draw_text(date_str, 120, 55, &montserrat_reg, COLOR_WHITE, 1.4, true);

    // hr icon
    draw_image(&hr, 75, 170, COLOR_WHITE, 0.25, true);
    // hr value
    snprintf(hr_str, sizeof(hr_str), "%d", g_app_data.biometrics.heart_rate);
    draw_text(hr_str, 75, 150, &montserrat_reg, COLOR_WHITE, 0.75, true);
    draw_arc(0, 359, 75, 155, PROGRESS_RADIUS, COLOR_WHITE, false, 1);
    float heart_arc_angle = g_app_data.biometrics.heart_rate / 200.0f;
    uint16_t filled = (uint16_t)roundf(360.0f * heart_arc_angle);
    draw_arc(180, 180+filled, 75, 155, PROGRESS_RADIUS, COLOR_RED, false, 2);



    // battery value
    draw_image(&battery, 165, 170, COLOR_WHITE, 1, true);
    const int bmp_tl_x = 165 - battery.width  / 2;
    const int bmp_tl_y = 170 - battery.height / 2;
    const uint16_t INNER_X = bmp_tl_x + 1;
    const uint16_t INNER_Y = bmp_tl_y + 3;
    const uint16_t INNER_W = battery.width  - 2;
    const uint16_t INNER_H = battery.height - 5;
    uint16_t fill_h = (INNER_H * g_app_data.settings.battery_level) / 100;
    if (fill_h && fill_h < 1) fill_h = 1;

    draw_rectangle(INNER_X, INNER_Y+(INNER_H-fill_h),INNER_W,  fill_h,COLOR_WHITE);
    snprintf(battery_str, sizeof(battery_str), "%d%%", g_app_data.settings.battery_level);
    draw_text(battery_str, 165, 150, &montserrat_reg, COLOR_WHITE, 0.75, true);
    draw_arc(0, 359, 165, 155, PROGRESS_RADIUS, COLOR_WHITE, false, 1);
    float battery_val_angle = g_app_data.settings.battery_level / 100.0f;
    uint16_t battery_fill = (uint16_t)roundf(360.0f * battery_val_angle);
    draw_arc(0, battery_fill, 165, 155, PROGRESS_RADIUS, COLOR_RED, false, 1);

    // steps icon
    //draw_image(&steps, 120 - 9, 190 - 6, COLOR_WHITE);
    // steps value
    snprintf(steps_str, sizeof(steps_str), "%d", g_app_data.biometrics.steps);
    draw_text(steps_str, 120, 195, &montserrat_reg, COLOR_WHITE, 0.65, true);
    draw_arc(180, 360, 120, 205, PROGRESS_RADIUS, COLOR_RED, false, 2); // Light gray background
    uint16_t steps_arc_angle = (g_app_data.biometrics.steps * 180) / GOAL_STEPS;
    if (steps_arc_angle > 180) steps_arc_angle = 180;
    draw_arc(180, 180 + steps_arc_angle, 120, 205, PROGRESS_RADIUS, COLOR_WHITE, false, 2);
}



void watchface_analog_draw(void) {
    draw_radial_gradient(
      CENTER_X, CENTER_Y,
      WATCH_RADIUS,
      COLOR_LIGHT_GRAY,
      0x0000     // outer: black
    );
    draw_watch_face();
    draw_hour_markers();
    draw_biometric_data();
    draw_watch_hands(g_app_data.timeVal.hour, g_app_data.timeVal.minute, g_app_data.timeVal.second);
    draw_center_dot();
    //screen_render_aa();
    screen_render();
}

void draw_watch_face(void) {
    draw_arc(0, 359, CENTER_X, CENTER_Y, WATCH_RADIUS, COLOR_WHITE, false, 1);
}


void draw_hour_markers(void) {
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f;
        float rad = angle * (M_PI / 180.0f);

        int x_outer = CENTER_X + (int)(WATCH_RADIUS * cosf(rad) + 0.5f);
        int y_outer = CENTER_Y + (int)(WATCH_RADIUS * sinf(rad) + 0.5f);
        int x_inner = CENTER_X + (int)((WATCH_RADIUS - 10) * cosf(rad) + 0.5f);
        int y_inner = CENTER_Y + (int)((WATCH_RADIUS - 10) * sinf(rad) + 0.5f);

        draw_line(x_inner, y_inner, x_outer, y_outer, COLOR_WHITE, 1);
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
    draw_arc(0, 359, CENTER_X, CENTER_Y, 4, COLOR_WHITE, true, 1);
}