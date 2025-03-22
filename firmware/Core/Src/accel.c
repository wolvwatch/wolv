/*
 * accel.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */


#include "main.h"
#include <stdio.h>

// For example, define the chip select pin:
#define ACCEL_CS_GPIO_Port   GPIOB
#define ACCEL_CS_Pin         GPIO_PIN_5

extern SPI_HandleTypeDef hspi1;

// Transmit a single byte over SPI, ignoring response
void ADXL362_SPI_WriteByte(uint8_t byte)
{
    HAL_SPI_Transmit(&hspi1, &byte, 1, HAL_MAX_DELAY);
}

// Read a single byte from SPI
uint8_t ADXL362_SPI_ReadByte(void)
{
    uint8_t rxByte = 0;
    HAL_SPI_Receive(&hspi1, &rxByte, 1, HAL_MAX_DELAY);
    return rxByte;
}

// Write one register on the ADXL362
// command 0x0A = WRITE REGISTER
//  byte0 = 0x0A
//  byte1 = regAddress
//  byte2 = value
void ADXL362_WriteRegister(uint8_t regAddress, uint8_t value)
{
    // CS low
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_RESET);

    // Send write command
    uint8_t cmd[3];
    cmd[0] = 0x0A;        // ADXL362 "Write register" command
    cmd[1] = regAddress;  // which register
    cmd[2] = value;       // data to write
    HAL_SPI_Transmit(&hspi1, cmd, 3, HAL_MAX_DELAY);

    // CS high
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_SET);
}

// Read one register on the ADXL362
// command 0x0B = READ REGISTER
//  byte0 = 0x0B
//  byte1 = regAddress
uint8_t ADXL362_ReadRegister(uint8_t regAddress)
{
    uint8_t value;
    // CS low
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_RESET);

    // Send read command + address
    uint8_t cmd[2];
    cmd[0] = 0x0B;
    cmd[1] = regAddress;
    HAL_SPI_Transmit(&hspi1, cmd, 2, HAL_MAX_DELAY);

    // Read returned byte
    HAL_SPI_Receive(&hspi1, &value, 1, HAL_MAX_DELAY);

    // CS high
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_SET);
    return value;
}

void ADXL362_Init(void)
{
    // 1) Soft reset (write 0x52 to SOFT_RESET register at 0x1F)
    ADXL362_WriteRegister(0x1F, 0x52);
    HAL_Delay(10); // small delay after reset

    // 2) Example: set range = +/- 2g, ODR = 100 Hz
    // FILTER_CTL (0x2C) defaults to 0x13 => +/-2g, 1/2 bandwidth, ODR=100 Hz
    // but we can explicitly write if you want:
    // ADXL362_WriteRegister(0x2C, 0x13);

    // 3) Put device into measurement mode
    // POWER_CTL (0x2D):
    // Bits [2:0] = 0b010 => Measurement Mode
    // For normal noise mode, ensure LOW_NOISE bits are 0:
    // [6:5] = 0 => normal. So final = 0x02
    ADXL362_WriteRegister(0x2D, 0x02);

    HAL_Delay(10);
}


void ADXL362_ReadXYZ(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t cmd[2];
    uint8_t rawData[6];

    // CS low
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_RESET);

    // Send read command and start address
    cmd[0] = 0x0B;    // READ REGISTER command
    cmd[1] = 0x0E;    // starting at XDATA_L
    HAL_SPI_Transmit(&hspi1, cmd, 2, HAL_MAX_DELAY);

    // Read 6 bytes (X_L, X_H, Y_L, Y_H, Z_L, Z_H)
    HAL_SPI_Receive(&hspi1, rawData, 6, HAL_MAX_DELAY);

    // CS high
    HAL_GPIO_WritePin(ACCEL_CS_GPIO_Port, ACCEL_CS_Pin, GPIO_PIN_SET);

    // Combine bytes
    // Each axis is 12-bit, sign-extended in a 16-bit container
    *x = (int16_t)((rawData[1] << 8) | rawData[0]);
    *y = (int16_t)((rawData[3] << 8) | rawData[2]);
    *z = (int16_t)((rawData[5] << 8) | rawData[4]);

    // Now each is a signed value in LSBs. If ±2g range, 1 LSB = 1 mg
    // That means acceleration in mg = (*x), or in g = (*x) / 1000.0f
}
