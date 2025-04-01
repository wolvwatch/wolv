/*
* bluetooth.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */
#include "bluetooth.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>
#include "lvgl.h"
#include "main.h"
#include "parseCommands.h"

extern uint8_t rx_buffer[1];
extern uint8_t rxData;
extern lv_indev_t *input;
extern UART_HandleTypeDef hlpuart1;
extern RTC_HandleTypeDef hrtc;

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

            parseBluetoothCommand(rxBuffer);
            rxIndex = 0;
        }
        HAL_UART_Receive_IT(huart, &rxData, 1);
    }

    if (huart->Instance == LPUART1) {
        HAL_UART_Receive_IT(&hlpuart1, rx_buffer, 1);
        lv_indev_read(input);
    }
}
