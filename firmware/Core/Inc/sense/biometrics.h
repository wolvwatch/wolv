#ifndef BIOMETRICS_H
#define BIOMETRICS_H

#include "stdint.h"

typedef void (*biometric_callback_func_t)(uint8_t new_hr, uint8_t new_spo2);

void update_biometrics();
void add_biometric_callback(biometric_callback_func_t func);

#endif //BIOMETRICS_H
