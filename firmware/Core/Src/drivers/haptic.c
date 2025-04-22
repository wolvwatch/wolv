/*
The MIT License (MIT)

Copyright (c) 2025 Sandro Petrovski, Austin Sierco, Ryan Kaelle, and Tenzin Sherab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "drivers/haptic.h"
#include <stdbool.h>
#include "main.h"
#include "stm32l4xx_hal.h"

#define BUZZ_DELAY 200

extern TIM_HandleTypeDef htim2;

static uint16_t called = 0;
static bool enabled = 0;

void Haptic_Start(void) {
    enabled = 1;
    called = HAL_GetTick();
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period / 1.2);
}

void Haptic_Stop(void) {
    if (enabled && (HAL_GetTick() - called) >= BUZZ_DELAY) {
        TIM2->CCR1 = 0;
        enabled = 0;
    }
}

void Haptic_Buzz(void) {
    Haptic_Start();
}
