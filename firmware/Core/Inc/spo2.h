#ifndef SPO2_H
#define SPO2_H

#include "stdint.h"

#define FreqS 25    //sampling frequency
#define BUFFER_SIZE (FreqS * 10)
#define MA4_SIZE 4 // DONOT CHANGE
//#define min(x,y) ((x) < (y) ? (x) : (y)) //Defined in Arduino.h

void maxim_heart_rate_and_oxygen_saturation(
    uint32_t *pun_ir_buffer,
    int32_t   n_ir_buffer_length,
    uint32_t *pun_red_buffer,
    int32_t  *pn_spo2,
    int8_t   *pch_spo2_valid,
    int32_t  *pn_heart_rate,
    int8_t   *pch_hr_valid
);

#endif //SPO2_H
