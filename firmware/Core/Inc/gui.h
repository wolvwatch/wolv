#ifndef GUI_H
#define GUI_H

#include "lvgl.h"

typedef struct {
  lv_subject_t ctime_h, ctime_m, ctime_s, ctime_ampm;
} gui_data_t;

void init_gui_data(gui_data_t *data);

#endif //GUI_H
