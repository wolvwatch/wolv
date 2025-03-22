/*
 * accel.h
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */

#ifndef INC_ACCEL_H_
#define INC_ACCEL_H_

#include "main.h"
#include <stdint.h>

// Chip Select definitions for the ADXL362
#define ACCEL_CS_GPIO_Port   GPIOA
#define ACCEL_CS_Pin         GPIO_PIN_4

// External SPI handle (defined elsewhere, e.g., in main.c)
extern SPI_HandleTypeDef hspi1;

// Function prototypes
void ADXL362_SPI_WriteByte(uint8_t byte);
uint8_t ADXL362_SPI_ReadByte(void);
void ADXL362_WriteRegister(uint8_t regAddress, uint8_t value);
uint8_t ADXL362_ReadRegister(uint8_t regAddress);
void ADXL362_Init(void);
void ADXL362_ReadXYZ(int16_t *x, int16_t *y, int16_t *z);

#endif /* INC_ACCEL_H_ */
