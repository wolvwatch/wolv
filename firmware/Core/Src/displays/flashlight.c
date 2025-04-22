#include "displays/flashlight.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "drivers/apps.h"
#include "displays/data.h"
#include "displays/launcher.h"
#include "main.h"

extern app_data_t g_app_data;
void flash_init(void) {}

void flash_update(void) {}

void flash_draw(void) {
    uint8_t tempBrightness = TIM1->CCR1;
    set_brightness(100);
    draw_rectangle(0, 0, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, COLOR_WHITE);
    screen_render();
}

void flash_input(button_t btn) {
    // button press returns to launcher
    if (btn == BTN_UP || btn == BTN_DOWN || btn == BTN_SEL) {
        g_app_data.display.active_screen = SCREEN_LAUNCHER;
        launcher_init();
    }
}

const app_t flash_app = {
    .init = flash_init,
    .update = flash_update,
    .draw = flash_draw,
    .input = flash_input
};
