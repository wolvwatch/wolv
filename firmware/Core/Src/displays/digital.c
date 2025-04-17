#include "displays/digital.h"
#include "displays/data.h"
#include "ux/display.h"
#include "drivers/lcd.h"
#include <stdio.h>
#include "ux/rasterizer.h"
#include <stdint.h>
#include <math.h>

extern app_data_t g_app_data;

// Constants for the watchface
#define INNER_RADIUS 110
#define BEZEL_WIDTH 4
#define ACCENT_COLOR 0b100  // Blue color for accents

// Helper function to draw the background and bezel
static void draw_background_and_bezel(void) {
    // Draw black background circle
    //draw_arc(0, 359, CENTER_X, CENTER_Y, WATCH_RADIUS, COLOR_BLACK, true, 1);

    // Draw bezel ring
    draw_arc(0, 359, CENTER_X, CENTER_Y, WATCH_RADIUS, 0b111, false, BEZEL_WIDTH);
}

// Helper function to draw day and date
static void draw_day_and_date(void) {
    char dayStr[4];
    char dateStr[8];

    // Get day of week (0=Sunday, 6=Saturday)
    int dayOfWeek = (g_app_data.timeVal.day + 2 * g_app_data.timeVal.month +
                    3 * (g_app_data.timeVal.month + 1) / 5 + g_app_data.timeVal.year +
                    g_app_data.timeVal.year / 4 - g_app_data.timeVal.year / 100 +
                    g_app_data.timeVal.year / 400) % 7;

    const char* days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    snprintf(dayStr, sizeof(dayStr), "%s", days[dayOfWeek]);
    snprintf(dateStr, sizeof(dateStr), "%02d / %02d",
             g_app_data.timeVal.month, g_app_data.timeVal.day);

    // Draw day
    draw_text(dayStr, CENTER_X, 40, &montserrat_reg, COLOR_WHITE, 0.85, true);
    // Draw date
    draw_text(dateStr, CENTER_X, 65, &montserrat_reg, COLOR_WHITE, 0.75, true);
}

// Helper function to draw the digital time
static void draw_digital_time(void) {
    char timeStr[16];
    char ampmStr[3];
    char secStr[3];

    snprintf(timeStr, sizeof(timeStr), "%02d:%02d",
             g_app_data.timeVal.hour % 12 ? g_app_data.timeVal.hour % 12 : 12,
             g_app_data.timeVal.minute);
    snprintf(ampmStr, sizeof(ampmStr), "%s",
             g_app_data.timeVal.hour >= 12 ? "pm" : "am");
    snprintf(secStr, sizeof(secStr), "%02d", g_app_data.timeVal.second);

    // hours
    char hourStr[3];
    snprintf(hourStr, sizeof(hourStr), "%02d",
             g_app_data.timeVal.hour % 12 ? g_app_data.timeVal.hour % 12 : 12);
    draw_text(hourStr, CENTER_X - 55, CENTER_Y + 6, &ultra, COLOR_WHITE, 0.5, true);

    // colon
    draw_text(":", CENTER_X-15, CENTER_Y + 6, &ultra, COLOR_WHITE, 0.5, true);

    // minutes
    char minStr[3];
    snprintf(minStr, sizeof(minStr), "%02d", g_app_data.timeVal.minute);
    draw_text(minStr, CENTER_X + 25, CENTER_Y + 6, &ultra, ACCENT_COLOR, 0.5, true);

    // AM/PM
    draw_text(ampmStr, CENTER_X + 75, CENTER_Y - 5, &montserrat_reg, ACCENT_COLOR, 0.8, true);

    // seconds
    draw_text(secStr, CENTER_X + 75, CENTER_Y + 15, &montserrat_reg, COLOR_WHITE, 0.8, true);
}

// Helper function to draw gauge arcs
static void draw_gauge_arcs(void) {
    // Battery gauge
    const int blocks       = 10;
    const float span       = 75.0f;
    const float start_base = 45.0f;
    const float block_ang  = span / blocks;
    const float gap_deg      = 2.0f;     // total gap between blocks
    const float half_gap     = gap_deg / 2.0f;

    int lvl = g_app_data.settings.battery_level;
    int filled = (lvl + 5) / 10;

    for (int i = 0; i < blocks; i++) {

        float a0f = start_base + i * block_ang + half_gap;
        float a1f = start_base + (i+1) * block_ang - half_gap;

        uint16_t a0 = (uint16_t)floorf(a0f + 0.5f);
        uint16_t a1 = (uint16_t)floorf(a1f + 0.5f);

        color_t col = (i < filled)? ACCENT_COLOR:0b111;

        draw_arc(a0, a1,CENTER_X, CENTER_Y,INNER_RADIUS,col,false,  4);     // stroke thickness
    }
    // draw_arc(45, 105, CENTER_X, CENTER_Y, INNER_RADIUS, 0b111, false, 4); // Background
    // uint16_t battery_angle = (uint16_t)(60 * g_app_data.settings.battery_level / 100.0f);
    // draw_arc(45, 45 + battery_angle, CENTER_X, CENTER_Y, INNER_RADIUS, ACCENT_COLOR, false, 4);

    // Battery label
    float bat_angle = 0.0f * M_PI / 180.0f;
    int bat_x = CENTER_X + (int)(INNER_RADIUS * cosf(bat_angle));
    int bat_y = CENTER_Y - (int)(INNER_RADIUS * sinf(bat_angle));

    // steps guage
    draw_arc(165, 225, CENTER_X, CENTER_Y, INNER_RADIUS, 0b111, false, 4); // Background
    uint16_t steps_angle = (g_app_data.biometrics.steps*(225-165)) / 10000.0f;
    draw_arc(165, 165 + steps_angle, CENTER_X, CENTER_Y, INNER_RADIUS, ACCENT_COLOR, false, 4);


    // hr gauge
    draw_arc(315, 375, CENTER_X, CENTER_Y, INNER_RADIUS, 0b111, false, 4); // Background
    uint16_t hr_percentage = (uint16_t)((g_app_data.biometrics.heart_rate * 100) / 200);
    if (hr_percentage > 100) hr_percentage = 100;
    uint16_t hr_angle = (uint16_t)(60 * hr_percentage / 100.0f);
    draw_arc(315, 315 + hr_angle, CENTER_X, CENTER_Y, INNER_RADIUS, ACCENT_COLOR, false, 4);
}

// Helper function to draw side metrics
static void draw_side_metrics(void) {
    char stepsStr[8];
    char hrStr[8];

    snprintf(stepsStr, sizeof(stepsStr), "%d", g_app_data.biometrics.steps);
    snprintf(hrStr, sizeof(hrStr), "%d", g_app_data.biometrics.heart_rate);

    // Calculate positions for left side (145°)
    float left_angle = 145.0f * M_PI / 180.0f;
    int left_x = CENTER_X + (int)(75 * cosf(left_angle));
    int left_y = CENTER_Y - (int)(75 * sinf(left_angle));

    // Calculate positions for right side (35°)
    float right_angle = 35.0f * M_PI / 180.0f;
    int right_x = CENTER_X + (int)(75 * cosf(right_angle));
    int right_y = CENTER_Y - (int)(75 * sinf(right_angle));

    float bottom_angle = 160.0f * M_PI / 180.0f;
    int bottom_x = CENTER_X + (int)(75 * sinf(bottom_angle));
    int bottom_y = CENTER_Y - (int)(75 * cosf(bottom_angle));



    // Draw steps
    draw_text(stepsStr, left_x-10, left_y, &montserrat_reg, COLOR_WHITE, 0.7, true);
    draw_text("Steps", left_x-10, left_y+15, &montserrat_reg, COLOR_WHITE, 0.5, true);

    // Draw heart rate
    draw_text(hrStr, right_x+10, right_y, &montserrat_reg, COLOR_WHITE, 0.7, true);
    draw_text("HR", right_x+10, right_y+15, &montserrat_reg, COLOR_WHITE, 0.5, true);

    // Draw battery %
    draw_text("BATT %", bottom_x+5, bottom_y+15, &montserrat_reg, COLOR_WHITE, 0.5, true);
}


void watchface_digital_draw(void) {
    draw_background_and_bezel();
    draw_day_and_date();
    draw_digital_time();
    draw_gauge_arcs();
    draw_side_metrics();
    screen_render();
}