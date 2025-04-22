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

#ifndef __ACCEL_H
#define __ACCEL_H

#include "stdint.h"

#define ADXL362_REG_SOFT_RESET    0x1F
#define ADXL362_REG_FILTER_CTL    0x2C
#define ADXL362_REG_POWER_CTL     0x2D

#define ADXL362_CMD_WRITE_REG     0x0A
#define ADXL362_CMD_READ_REG      0x0B
#define ADXL362_FIFO_CTRL_REG     0x28
#define ADXL362_FIFO_SAMPLES_REG  0x29
#define ADXL362_INTMAP1_REG       0x2A
#define ADXL362_FIFO_ENTRIES_REG  0x0C
#define ADXL362_STATUS_REG        0x0B

void ADXL362_Select(void);

/* Initialize the ADXL362 for ±2g, 100 Hz ODR, normal noise mode */
void ADXL362_Init(void);

/* Write a single byte to the given ADXL362 register address */
void ADXL362_WriteReg(uint8_t regAddr, uint8_t data);

/* Read a single byte from the given ADXL362 register address */
uint8_t ADXL362_ReadReg(uint8_t regAddr);

/* Perform a soft-reset of the ADXL362 (write 0x52 to register 0x1F) */
void ADXL362_SoftReset(void);

/* Read the raw 12-bit X/Y/Z acceleration data into xRaw, yRaw, zRaw */
void ADXL362_ReadXYZ(int16_t *xRaw, int16_t *yRaw, int16_t *zRaw);

#endif /* __ACCEL_H */
