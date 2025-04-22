#include "displays/launcher.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "displays/data.h"
#include "drivers/apps.h"
#include <math.h>
#include "displays/analog.h"
#include "displays/digital.h"
#include "displays/sysmon.h"
#include "displays/flappy.h"
#include "displays/timerApp.h"
#include "displays/notification.h"
#include "displays/flashlight.h"

extern app_data_t g_app_data;

static const char* app_names[APP_COUNT] = {
    "Analog Watch",
    "Digital Watch",
    "App Launcher",
    "Weather",
    "Flappy Bird",
    "Timer",
    //"Medical Card",
    //"Metronome",
    "System Info",
    "Flashlight"
};

// selectable apps 
#define SELECTABLE_APP_COUNT (APP_COUNT - 1)

static app_id_t s_sel; // current app
static float s_scroll_offset; 
static float s_target_scroll_offset;


static int app_id_to_display_index(app_id_t id) {
    return (id >= APP_LAUNCHER) ? (id - 1) : id;
}

static app_id_t display_index_to_app_id(int index) {
    return (index >= APP_LAUNCHER) ? (index + 1) : index;
}

void launcher_init(void) {
    s_sel = APP_WATCHFACE_ANALOG;
    s_scroll_offset = 0;
    s_target_scroll_offset = 0;
}

void launcher_update(void) {
    float diff = s_target_scroll_offset - s_scroll_offset;
    if (fabsf(diff) > 0.1f) {
        s_scroll_offset += diff * 0.2f;
    } else {
        s_scroll_offset = s_target_scroll_offset;
    }
}

void launcher_draw(void) {
    screen_clear(0x0000);

    draw_text("App Launcher", CENTER_X, 30, &montserrat_reg, COLOR_WHITE, 1.0f, true);

    for (int i = 0; i < SELECTABLE_APP_COUNT; i++) {
        app_id_t app_id = display_index_to_app_id(i);
        float y = 80 + i * 30 - s_scroll_offset;
        
        if (y >= 60 && y <= LCD_1IN28_HEIGHT - 30) {
            if (app_id == s_sel) {
                draw_rectangle(20, y - 15, LCD_1IN28_WIDTH - 40, 30, COLOR_WHITE);
                draw_text(app_names[app_id], CENTER_X, y, &montserrat_reg, 0b000, 0.8f, true);
            } else {
                draw_text(app_names[app_id], CENTER_X, y, &montserrat_reg, COLOR_WHITE, 0.7f, true);
            }
        }
    }

    draw_text("UP", 30, LCD_1IN28_HEIGHT - 30, &montserrat_reg, COLOR_WHITE, 0.6f, false);
    draw_text("SELECT", CENTER_X, LCD_1IN28_HEIGHT - 30, &montserrat_reg, COLOR_WHITE, 0.6f, true);
    draw_text("DOWN", LCD_1IN28_WIDTH - 30, LCD_1IN28_HEIGHT - 30, &montserrat_reg, COLOR_WHITE, 0.6f, false);

    screen_render();
}

void launcher_input(button_t btn) {
    int current_index = app_id_to_display_index(s_sel);
    
    switch (btn) {
        case BTN_UP:
            if (current_index > 0) {
                current_index--;
                s_sel = display_index_to_app_id(current_index);
                s_target_scroll_offset = current_index * 30;
            }
            break;
        case BTN_DOWN:
            if (current_index < SELECTABLE_APP_COUNT - 1) {
                current_index++;
                s_sel = display_index_to_app_id(current_index);
                s_target_scroll_offset = current_index * 30;
            }
            break;
        case BTN_SEL:
            apps[s_sel].init();
            g_app_data.display.active_screen = s_sel;
            break;
        default:
            break;
    }
}

const app_t launcher_app = {
    .init = launcher_init,
    .update = launcher_update,
    .draw = launcher_draw,
    .input = launcher_input
}; 