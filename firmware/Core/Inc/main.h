/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN1_Pin GPIO_PIN_13
#define BTN1_GPIO_Port GPIOC
#define BTN2_Pin GPIO_PIN_14
#define BTN2_GPIO_Port GPIOC
#define BTN3_Pin GPIO_PIN_15
#define BTN3_GPIO_Port GPIOC
#define Misc1_Pin GPIO_PIN_0
#define Misc1_GPIO_Port GPIOC
#define Misc2_Pin GPIO_PIN_1
#define Misc2_GPIO_Port GPIOC
#define Misc3_Pin GPIO_PIN_2
#define Misc3_GPIO_Port GPIOC
#define TIM2_CH1_Haptic_Pin GPIO_PIN_0
#define TIM2_CH1_Haptic_GPIO_Port GPIOA
#define CS_MEM_Pin GPIO_PIN_2
#define CS_MEM_GPIO_Port GPIOA
#define BT_STATE_Pin GPIO_PIN_4
#define BT_STATE_GPIO_Port GPIOA
#define BT_EN_Pin GPIO_PIN_5
#define BT_EN_GPIO_Port GPIOA
#define WP_MEM_Pin GPIO_PIN_0
#define WP_MEM_GPIO_Port GPIOB
#define H_RST_MEM_Pin GPIO_PIN_1
#define H_RST_MEM_GPIO_Port GPIOB
#define EXTI15_HR_INT1_Pin GPIO_PIN_15
#define EXTI15_HR_INT1_GPIO_Port GPIOB
#define TIM1_CH1_BL_Screen_Pin GPIO_PIN_8
#define TIM1_CH1_BL_Screen_GPIO_Port GPIOA
#define SCRN_RST_Pin GPIO_PIN_9
#define SCRN_RST_GPIO_Port GPIOA
#define SCRN_DC_Pin GPIO_PIN_10
#define SCRN_DC_GPIO_Port GPIOA
#define SCRN_CS_Pin GPIO_PIN_11
#define SCRN_CS_GPIO_Port GPIOA
#define Flashing_SWDIO_Pin GPIO_PIN_13
#define Flashing_SWDIO_GPIO_Port GPIOA
#define Flashing_SWCLK_Pin GPIO_PIN_14
#define Flashing_SWCLK_GPIO_Port GPIOA
#define CS_ACC_Pin GPIO_PIN_4
#define CS_ACC_GPIO_Port GPIOB
#define EXTI15_ACC_INT2_Pin GPIO_PIN_5
#define EXTI15_ACC_INT2_GPIO_Port GPIOB
#define EXTI6_ACC_INT1_Pin GPIO_PIN_6
#define EXTI6_ACC_INT1_GPIO_Port GPIOB
#define ACC_3_3V_Pin GPIO_PIN_7
#define ACC_3_3V_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
