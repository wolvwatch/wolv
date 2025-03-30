#include "gui.h"

void init_gui_data(gui_data_t *data) {
    lv_subject_init_int(&data->ctime_h, 0);
    lv_subject_init_int(&data->ctime_m, 0);
    lv_subject_init_int(&data->ctime_s, 0);
    lv_subject_init_int(&data->ctime_ampm, 0);
}