/*
* bluetooth.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */
#include "../../Inc/drivers/bluetooth.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
extern uint8_t rxData;

#define RX_BUFFER_SIZE 256
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        rxBuffer[rxIndex++] = rxData;
        if (rxData == '\n' || rxIndex >= RX_BUFFER_SIZE - 1){
            rxBuffer[rxIndex] = '\0';
            printf("Received string: %s", rxBuffer);
            rxIndex = 0;
        }

        HAL_UART_Receive_IT(huart, &rxData, 1);
    }
}