#include "gui.h"

void init_gui_data(gui_data_t *data) {
    lv_subject_init_int(&data->ctime_h, 0);
    lv_subject_init_int(&data->ctime_m, 0);
    lv_subject_init_int(&data->ctime_s, 0);
    lv_subject_init_int(&data->ctime_ampm, 0);

    lv_subject_init_int(&data->hr, 0);
    lv_subject_init_int(&data->hr_val, 0);
    lv_subject_init_int(&data->spo2, 0);
    lv_subject_init_int(&data->spo2_val, 0);
}