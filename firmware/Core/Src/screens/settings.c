#include "lvgl.h"
#include "gui.h"
#include "devices/lcd.h"

static lv_obj_t *root;

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target_obj(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected month: %s\n", buf);
    }
}

CREATE_SCREEN(settings) {
    settings = lv_obj_create(NULL);

    lv_obj_t * roller1 = lv_roller_create(settings);
    lv_roller_set_options(roller1, LV_SYMBOL_SETTINGS " Date / Time\nUnits\nAbout", LV_ROLLER_MODE_INFINITE);

    lv_roller_set_visible_row_count(roller1, 4);
    lv_obj_center(roller1);
    lv_obj_add_event_cb(roller1, event_handler, LV_EVENT_ALL, NULL);
}
