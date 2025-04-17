/*
* bluetooth.h
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */

#ifndef INC_BLUETOOTH_H_
#define INC_BLUETOOTH_H_

#include "../../../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"
#include <stdio.h>

#define RX_BUFFER_SIZE 256

/* External variable declared elsewhere (e.g., in main.c) */
extern uint8_t rxData;

extern volatile char rxBuffer[RX_BUFFER_SIZE];
extern volatile uint16_t rxIndex;
extern UART_HandleTypeDef huart3;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void parseBluetoothCommand(char *cmd);
void send_sensor_data(void);
void send_ack_request(void);
void check_ack_timeout(void);


// commands left to change:
/*
 * batt
 * face
 * app:list,launch,close
 * set:timezone,hr_interval
 * sens: hr, spo2, accel
 * time: get, set, date
 * notif: call icon
 *
 */



#endif /* INC_BLUETOOTH_H_ */
