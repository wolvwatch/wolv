#include <stdio.h>

#include "gui.h"
#include "lvgl.h"

static void hr_observer(lv_observer_t *observer, lv_subject_t *subject);
static void spo2_observer(lv_observer_t *observer, lv_subject_t *subject);

CREATE_SCREEN(watchface_digital_0) {
    watchface_digital_0 = lv_obj_create(NULL);

    LV_FONT_DECLARE(ultra_90);
    LV_FONT_DECLARE(ultra_40);
    LV_IMAGE_DECLARE(img_steps);
    LV_IMAGE_DECLARE(img_heart);
    LV_IMAGE_DECLARE(img_spo2);
    LV_IMAGE_DECLARE(img_bluetooth);
    LV_IMAGE_DECLARE(img_active_time);

    lv_obj_t *hours = lv_label_create(watchface_digital_0);
    lv_label_bind_text(hours, &gui_data.ctime_h, "%02d");
    lv_obj_set_pos(hours, 22, 90);
    lv_obj_set_style_text_font(hours, &ultra_90, LV_PART_MAIN);

    lv_obj_t *minutes = lv_label_create(watchface_digital_0);
    lv_label_bind_text(minutes, &gui_data.ctime_m, "%02d");
    lv_obj_set_style_text_font(minutes, &ultra_40, LV_PART_MAIN);
    lv_obj_align_to(minutes, hours, LV_ALIGN_TOP_RIGHT, 60, 0);

    lv_obj_t *seconds = lv_label_create(watchface_digital_0);
    lv_obj_set_style_text_font(seconds, &lv_font_montserrat_12, LV_PART_MAIN);
    lv_label_bind_text(seconds, &gui_data.ctime_s, "%02d");
    lv_obj_set_style_text_color(seconds, lv_color_hex(0xAA5555), LV_PART_MAIN);
    lv_obj_align_to(seconds, minutes, LV_ALIGN_OUT_RIGHT_BOTTOM, 3, 0);

    lv_obj_t *date = lv_label_create(watchface_digital_0);
    lv_label_set_text(date, "SAT 29");
    lv_obj_set_style_text_font(date, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_obj_set_style_text_color(date, lv_color_hex(0xAAAAAA), LV_PART_MAIN);
    lv_obj_align_to(date, minutes, LV_ALIGN_BOTTOM_MID, 0, 20);

    lv_obj_t *steps_progress = lv_arc_create(watchface_digital_0);
    lv_arc_set_rotation(steps_progress, 270);
    lv_arc_set_bg_angles(steps_progress, 0, 360);
    lv_arc_set_bg_end_angle(steps_progress, 360);
    lv_obj_remove_style(steps_progress, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(steps_progress, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(steps_progress, 65, 65);
    lv_obj_center(steps_progress);
    lv_arc_set_value(steps_progress, 60);
    lv_obj_set_align(steps_progress, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_pos(steps_progress, 0, -5);
    lv_obj_set_style_arc_width(steps_progress, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(steps_progress, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(steps_progress, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);

    lv_obj_t *steps_count = lv_label_create(watchface_digital_0);
    lv_obj_set_style_text_font(steps_count, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(steps_count, "6,281");
    lv_obj_align_to(steps_count, steps_progress, LV_ALIGN_CENTER, 0, 9);

    lv_obj_t *steps_icon = lv_image_create(watchface_digital_0);
    lv_image_set_src(steps_icon, &img_steps);
    lv_image_set_scale(steps_icon, 100);
    lv_obj_align_to(steps_icon, steps_progress, LV_ALIGN_CENTER, 0, -11);

    lv_obj_t *hr_icon = lv_image_create(watchface_digital_0);
    lv_image_set_src(hr_icon, &img_heart);
    lv_image_set_scale(hr_icon, 100);
    lv_obj_set_pos(hr_icon, 25, 10);

    lv_obj_t *hr_val = lv_label_create(watchface_digital_0);
    lv_subject_add_observer_obj(&gui_data.hr, hr_observer, hr_val, NULL);
    lv_obj_set_width(hr_val, 20);
    lv_obj_align_to(hr_val, hr_icon, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_text_align(hr_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(hr_val, &lv_font_montserrat_12, LV_PART_MAIN);

    lv_obj_t *spo2_icon = lv_image_create(watchface_digital_0);
    lv_image_set_src(spo2_icon, &img_spo2);
    lv_image_set_scale(spo2_icon, 128);
    lv_obj_set_pos(spo2_icon, 154, 10);

    lv_obj_t *spo2_val = lv_label_create(watchface_digital_0);
    lv_subject_add_observer_obj(&gui_data.spo2, spo2_observer, spo2_val, NULL);
    lv_obj_set_width(spo2_val, 50);
    lv_obj_align_to(spo2_val, spo2_icon, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_align(spo2_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(spo2_val, &lv_font_montserrat_12, LV_PART_MAIN);

    lv_obj_t *bluetooth_icon = lv_image_create(watchface_digital_0);
    lv_image_set_src(bluetooth_icon, &img_bluetooth);
    lv_image_set_scale(bluetooth_icon, 100);
    lv_obj_set_pos(bluetooth_icon, 20, 150);

    lv_obj_t *active_time_progress = lv_arc_create(watchface_digital_0);
    lv_arc_set_rotation(active_time_progress, 270);
    lv_arc_set_bg_angles(active_time_progress, 0, 360);
    lv_arc_set_bg_end_angle(active_time_progress, 360);
    lv_obj_remove_style(active_time_progress, NULL, LV_PART_KNOB);
    lv_obj_remove_flag(active_time_progress, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(active_time_progress, 65, 65);
    lv_obj_center(active_time_progress);
    lv_arc_set_value(active_time_progress, 10);
    lv_obj_set_align(active_time_progress, LV_ALIGN_TOP_MID);
    lv_obj_set_pos(active_time_progress, 0, 5);
    lv_obj_set_style_arc_width(active_time_progress, 4, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(active_time_progress, 4, LV_PART_MAIN);
    lv_obj_set_style_arc_color(active_time_progress, lv_palette_main(LV_PALETTE_LIGHT_BLUE), LV_PART_INDICATOR);

    lv_obj_t *active_time_count = lv_label_create(watchface_digital_0);
    lv_obj_set_style_text_font(active_time_count, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(active_time_count, "28");
    lv_obj_align_to(active_time_count, active_time_progress, LV_ALIGN_CENTER, 0, 9);

    lv_obj_t *active_time_icon = lv_image_create(watchface_digital_0);
    lv_image_set_src(active_time_icon, &img_active_time);
    lv_image_set_scale(active_time_icon, 100);
    lv_obj_align_to(active_time_icon, active_time_progress, LV_ALIGN_CENTER, 0, -11);
}

static void hr_observer(lv_observer_t *observer, lv_subject_t *subject) {
    lv_obj_t *target = lv_observer_get_target_obj(observer);
    char buf[4];
    sprintf(buf, "%02d", lv_subject_get_int(subject));
    lv_label_set_text(target, lv_subject_get_int(&gui_data.hr_val) == 1 ? buf : "--");
}

static void spo2_observer(lv_observer_t *observer, lv_subject_t *subject) {
    lv_obj_t *target = lv_observer_get_target_obj(observer);
    char buf[3];
    sprintf(buf, "%02d%%", lv_subject_get_int(subject));
    lv_label_set_text(target, lv_subject_get_int(&gui_data.spo2_val) == 1 ? buf : "--");
}
