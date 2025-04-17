#ifndef HAPTIC_H
#define HAPTIC_H

#include "../../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"

void Haptic_Start(void);

void Haptic_Stop(void);

void Haptic_Buzz(uint32_t duration_ms);

#endif // HAPTIC_H