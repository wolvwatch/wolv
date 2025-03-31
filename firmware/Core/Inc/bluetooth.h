/*
* bluetooth.h
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "stm32l4xx_hal.h"
#include <stdio.h>

#define RX_BUFFER_SIZE 256

/* External variable declared elsewhere (e.g., in main.c) */
extern uint8_t rxData;

extern volatile char rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxIndex;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_BLUETOOTH_H_ */