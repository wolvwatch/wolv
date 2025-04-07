#include <stdio.h>
#include <math.h>

#include "gui.h"
#include "lvgl.h"
#include "rasterizer.h"

#define CLOCK_CENTER_X 120
#define CLOCK_CENTER_Y 120
#define CLOCK_RADIUS 100
#define HOUR_HAND_LENGTH 55
#define MINUTE_HAND_LENGTH 75
#define SECOND_HAND_LENGTH 85

#define COLOR_WHITE 0xFFFF
#define COLOR_RED 0xF800
#define COLOR_GREEN 0x07E0
#define COLOR_BLUE 0x001F
#define COLOR_GRAY 0x8410

static lv_obj_t *canvas;
static uint8_t *canvas_buf;
static lv_timer_t *update_timer; 

static void update_clock(lv_timer_t *timer);
static void hr_observer(lv_observer_t *observer, lv_subject_t *subject);
static void spo2_observer(lv_observer_t *observer, lv_subject_t *subject);

CREATE_SCREEN(watchface_analog_0) {
    watchface_analog_0 = lv_obj_create(NULL);
    
    canvas_buf = lv_malloc(LV_CANVAS_BUF_SIZE_TRUE_COLOR(LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT));
    canvas = lv_canvas_create(watchface_analog_0);
    lv_canvas_set_buffer(canvas, canvas_buf, LCD_1IN28_WIDTH, LCD_1IN28_HEIGHT, LV_COLOR_FORMAT_NATIVE);
    lv_obj_center(canvas);
    
    // update the clock every second (1000ms)
    update_timer = lv_timer_create(update_clock, 1000, NULL);
    
    LV_IMAGE_DECLARE(img_steps);
    LV_IMAGE_DECLARE(img_heart);
    LV_IMAGE_DECLARE(img_spo2);
    LV_IMAGE_DECLARE(img_bluetooth);
    
    // hr
    lv_obj_t *hr_icon = lv_image_create(watchface_analog_0);
    lv_image_set_src(hr_icon, &img_heart);
    lv_image_set_scale(hr_icon, 100);
    lv_obj_set_pos(hr_icon, 25, 10);

    lv_obj_t *hr_val = lv_label_create(watchface_analog_0);
    lv_subject_add_observer_obj(&gui_data.hr, hr_observer, hr_val, NULL);
    lv_obj_set_width(hr_val, 20);
    lv_obj_align_to(hr_val, hr_icon, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_text_align(hr_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(hr_val, &lv_font_montserrat_12, LV_PART_MAIN);

    // sp02
    lv_obj_t *spo2_icon = lv_image_create(watchface_analog_0);
    lv_image_set_src(spo2_icon, &img_spo2);
    lv_image_set_scale(spo2_icon, 128);
    lv_obj_set_pos(spo2_icon, 154, 10);

    lv_obj_t *spo2_val = lv_label_create(watchface_analog_0);
    lv_subject_add_observer_obj(&gui_data.spo2, spo2_observer, spo2_val, NULL);
    lv_obj_set_width(spo2_val, 50);
    lv_obj_align_to(spo2_val, spo2_icon, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_text_align(spo2_val, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_font(spo2_val, &lv_font_montserrat_12, LV_PART_MAIN);
    
    // bluetooth
    lv_obj_t *bluetooth_icon = lv_image_create(watchface_analog_0);
    lv_image_set_src(bluetooth_icon, &img_bluetooth);
    lv_image_set_scale(bluetooth_icon, 100);
    lv_obj_set_pos(bluetooth_icon, 20, 150);
    
    update_clock(NULL);
}

static void update_clock(lv_timer_t *timer) {
    uint8_t hours = lv_subject_get_int(&gui_data.ctime_h);
    uint8_t minutes = lv_subject_get_int(&gui_data.ctime_m);
    uint8_t seconds = lv_subject_get_int(&gui_data.ctime_s);
    

    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    // clock face
    draw_arc(0, 360, CLOCK_CENTER_X, CLOCK_CENTER_Y, CLOCK_RADIUS, COLOR_GRAY, false, 2);
    
    // hour markers
    for (int i = 0; i < 12; i++) {
        float angle = i * 30.0f * M_PI / 180.0f;
        uint16_t x1 = CLOCK_CENTER_X + (CLOCK_RADIUS - 10) * cosf(angle);
        uint16_t y1 = CLOCK_CENTER_Y + (CLOCK_RADIUS - 10) * sinf(angle);
        uint16_t x2 = CLOCK_CENTER_X + CLOCK_RADIUS * cosf(angle);
        uint16_t y2 = CLOCK_CENTER_Y + CLOCK_RADIUS * sinf(angle);
        
        draw_line(x1, y1, x2, y2, COLOR_WHITE, 2);
    }
    
    // hour hand
    float hour_angle = ((hours % 12) * 30 + minutes * 0.5f) * M_PI / 180.0f;
    uint16_t hour_x = CLOCK_CENTER_X + HOUR_HAND_LENGTH * sinf(hour_angle);
    uint16_t hour_y = CLOCK_CENTER_Y - HOUR_HAND_LENGTH * cosf(hour_angle);
    draw_line(CLOCK_CENTER_X, CLOCK_CENTER_Y, hour_x, hour_y, COLOR_WHITE, 4);
    
    // minute hand
    float minute_angle = (minutes * 6) * M_PI / 180.0f;
    uint16_t minute_x = CLOCK_CENTER_X + MINUTE_HAND_LENGTH * sinf(minute_angle);
    uint16_t minute_y = CLOCK_CENTER_Y - MINUTE_HAND_LENGTH * cosf(minute_angle);
    draw_line(CLOCK_CENTER_X, CLOCK_CENTER_Y, minute_x, minute_y, COLOR_WHITE, 2);
    
    // second hand
    float second_angle = (seconds * 6) * M_PI / 180.0f;
    uint16_t second_x = CLOCK_CENTER_X + SECOND_HAND_LENGTH * sinf(second_angle);
    uint16_t second_y = CLOCK_CENTER_Y - SECOND_HAND_LENGTH * cosf(second_angle);
    draw_line(CLOCK_CENTER_X, CLOCK_CENTER_Y, second_x, second_y, COLOR_RED, 1);
    
    draw_arc(0, 360, CLOCK_CENTER_X, CLOCK_CENTER_Y, 5, COLOR_WHITE, true, 0);
    
    lv_obj_invalidate(canvas);
    
    extern void screen_render();
    screen_render();
}

static void hr_observer(lv_observer_t *observer, lv_subject_t *subject) {
    lv_obj_t *target = lv_observer_get_target_obj(observer);
    char buf[4];
    sprintf(buf, "%02d", lv_subject_get_int(subject));
    lv_label_set_text(target, lv_subject_get_int(&gui_data.hr_val) == 1 ? buf : "--");
}

static void spo2_observer(lv_observer_t *observer, lv_subject_t *subject) {
    lv_obj_t *target = lv_observer_get_target_obj(observer);
    char buf[5];
    sprintf(buf, "%02d%%", lv_subject_get_int(subject));
    lv_label_set_text(target, lv_subject_get_int(&gui_data.spo2_val) == 1 ? buf : "--");
} 