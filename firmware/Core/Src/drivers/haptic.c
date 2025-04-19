#include "drivers/haptic.h"
#include "../Inc/main.h"

extern TIM_HandleTypeDef htim2;

void Haptic_Start(void)
{
    TIM2->CCR1 = 0.5 * 65535/100;
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, htim2.Init.Period / 2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

void Haptic_Stop(void)
{
    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
}

void Haptic_Buzz(uint32_t duration_ms)
{
    Haptic_Start();
    HAL_Delay(duration_ms);
    Haptic_Stop();
}