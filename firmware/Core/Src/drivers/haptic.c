#include "drivers/haptic.h"
#include "main.h"
#include "stm32l4xx_hal.h"

extern TIM_HandleTypeDef htim2;

void Haptic_Start(void)
{
    TIM2->CCR1 = 0.5 * 65535/100;
}

void Haptic_Stop(void)
{
    TIM2->CCR1 = 0;
}

void Haptic_Buzz(void)
{
    Haptic_Start();
    for (int i = 0; i < 600000; i++){}
    Haptic_Stop();
}