#include "displays/digital.h"
#include "displays/data.h"
#include "ux/display.h"
#include "drivers/lcd.h"
#include <stdio.h>
#include "ux/rasterizer.h"
#include <stdint.h>

extern app_data_t g_app_data;

void watchface_digital_draw(void) {
    char timeStr[16];
    char dateStr[16];

    snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
             g_app_data.timeVal.hour,
             g_app_data.timeVal.minute,
             g_app_data.timeVal.second);

    snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
             g_app_data.timeVal.day,
             g_app_data.timeVal.month,
             g_app_data.timeVal.year);

    draw_text(timeStr, 30, 40, &ultra, COLOR_WHITE, 1.0, false);
    draw_text(dateStr, 30, 120, &montserrat_reg, COLOR_WHITE, 1.0, false);

    // additional stuff needs to be added here like BT

    screen_render();
}