#include "gui.h"
#include "lvgl.h"

#define DEFAULT_FONT &lv_font_montserrat_14

static lv_style_t main_style;
#define INIT_SCREEN(s) {\
    extern void init_##s();\
    extern lv_obj_t *s;\
    init_##s();\
    lv_obj_add_style(s, &main_style, 0);\
    screens[SCREEN_##s] = s;\
}

static lv_obj_t *screens[SCREEN_COUNT] = {};
extern gui_data_t gui_data;

void init_gui() {
    lv_style_init(&main_style);
    lv_style_set_bg_color(&main_style, lv_color_hex(0x0));
    lv_style_set_text_color(&main_style, lv_color_hex(0xffffff));
    lv_style_set_text_font(&main_style, DEFAULT_FONT);

    lv_subject_init_int(&gui_data.ctime_h, 0);
    lv_subject_init_int(&gui_data.ctime_m, 0);
    lv_subject_init_int(&gui_data.ctime_s, 0);
    lv_subject_init_int(&gui_data.ctime_ampm, 0);

    lv_subject_init_int(&gui_data.hr, 0);
    lv_subject_init_int(&gui_data.hr_val, 0);
    lv_subject_init_int(&gui_data.spo2, 0);
    lv_subject_init_int(&gui_data.spo2_val, 0);

    INIT_SCREEN(watchface_digital_0);
    INIT_SCREEN(watchface_analog_0);
    INIT_SCREEN(settings);
}

void load_screen(const screen_t screen) {
    lv_screen_load(screens[screen]);
}
