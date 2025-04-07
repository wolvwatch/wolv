#ifndef GUI_H
#define GUI_H

#include "lvgl.h"

#define CREATE_SCREEN(s)\
  extern gui_data_t gui_data;\
  lv_obj_t *s;\
  void init_##s()

typedef struct {
  lv_subject_t ctime_h, ctime_m, ctime_s, ctime_ampm;
  lv_subject_t hr, hr_val, spo2, spo2_val;
} gui_data_t;

typedef enum {
  SCREEN_watchface_digital_0,
  SCREEN_watchface_analog_0,
  SCREEN_settings,
  SCREEN_COUNT
} screen_t;

void init_gui();

void load_screen(screen_t screen);

#endif //GUI_H
