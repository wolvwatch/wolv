#ifndef ACCEL_H
#define ACCEL_H

#include "stdint.h"

// TODO SET STEPS IN GLOBAL STRUCT
void update_steps(const uint16_t x, const uint16_t y, const uint16_t z);
void update_raise_to_wake(const uint16_t x, const uint16_t y, const uint16_t z);

#endif //ACCEL_H
