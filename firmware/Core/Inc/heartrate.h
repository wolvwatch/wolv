/*
 * heartrate.h
 *
 *  Created on: Mar 17, 2025
 *      Author: ryankaelle
 */

#ifndef INC_HEARTRATE_H_
#define INC_HEARTRATE_H_

#include "stm32l4xx_hal.h"
#include "stdint.h"
#include "stdio.h"

void MAX30102_Init(void);
uint32_t MAX30102_ReadFIFO(void);
void Process_HeartRate(void);
void Send_HeartRate_UART(void);

extern uint32_t ir_buffer[];
extern uint8_t buffer_index;
extern uint32_t last_peak_time;
extern float bpm;

#endif /* INC_HEARTRATE_H_ */
