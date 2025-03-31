/*
 * accel.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */


#include "main.h"
#include <stdio.h>
#include "stm32l4xx_hal.h"

// For example, define the chip select pin:
#define CS_ACC_GPIO_Port   GPIOD
#define CS_ACC_Pin         GPIO_PIN_1

extern SPI_HandleTypeDef hspi1;


#define ADXL362_REG_SOFT_RESET    0x1F
#define ADXL362_REG_FILTER_CTL    0x2C
#define ADXL362_REG_POWER_CTL     0x2D

#define ADXL362_CMD_WRITE_REG     0x0A
#define ADXL362_CMD_READ_REG      0x0B

// For convenience, chip-select helpers:
static inline void ADXL362_Select(void)
{
    HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, GPIO_PIN_RESET);
}
static inline void ADXL362_Unselect(void)
{
    HAL_GPIO_WritePin(CS_ACC_GPIO_Port, CS_ACC_Pin, GPIO_PIN_SET);
}

/**
  * @brief  Send a single-byte write to an ADXL362 register.
  * @param  regAddr: 8-bit register address
  * @param  data:    byte to write
  */
void ADXL362_WriteReg(uint8_t regAddr, uint8_t data)
{
    uint8_t txBuf[3];
    txBuf[0] = ADXL362_CMD_WRITE_REG; // 0x0A
    txBuf[1] = regAddr;
    txBuf[2] = data;

    ADXL362_Select();
    HAL_SPI_Transmit(&hspi1, txBuf, 3, HAL_MAX_DELAY);
    ADXL362_Unselect();
}

/**
  * @brief  Read a single byte from an ADXL362 register.
  * @param  regAddr: 8-bit register address
  * @return The register's byte value
  */
uint8_t ADXL362_ReadReg(uint8_t regAddr)
{
    uint8_t txBuf[2];
    uint8_t rxBuf[1];

    txBuf[0] = ADXL362_CMD_READ_REG;  // 0x0B
    txBuf[1] = regAddr;

    ADXL362_Select();
    // Send [READ_CMD, RegisterAddress]
    HAL_SPI_Transmit(&hspi1, txBuf, 2, HAL_MAX_DELAY);
    // Now read back one byte
    HAL_SPI_Receive(&hspi1, rxBuf, 1, HAL_MAX_DELAY);
    ADXL362_Unselect();

    return rxBuf[0];
}

/**
  * @brief  Perform a soft reset of the ADXL362 and wait for it.
  */
void ADXL362_SoftReset(void)
{
    // Per datasheet, write 0x52 to SOFT_RESET (0x1F)
    ADXL362_WriteReg(ADXL362_REG_SOFT_RESET, 0x52);
    // Wait ~10 ms for the device to reset
    HAL_Delay(10);
}

/**
  * @brief  Initialize ADXL362 for ±2g range, 100Hz ODR, measurement mode.
  *         Adjust to your preferences as needed.
  */
void ADXL362_Init(void)
{
    // 1) Soft reset
    ADXL362_SoftReset();

    // 2) FILTER_CTL (0x2C):
    //    Bits: [7:6] = range (00 => ±2g),
    //          [3]   = HALF_BW=1 => bandwidth is ODR/4, or 0 => ODR/2,
    //          [2:0] = ODR (011 => 100 Hz)
    //    So 0x13 => Range=±2g, HALF_BW=0, ODR=100Hz.
    //    Or 0x12 => Range=±2g, HALF_BW=1, ODR=100Hz
    ADXL362_WriteReg(ADXL362_REG_FILTER_CTL, 0x13);

    // 3) POWER_CTL (0x2D):
    //    Bits: [6:5] = low noise mode (00 => normal, 01 => low noise, etc.)
    //          [2]   = wake-up mode
    //          [1:0] = measurement mode (10 => measure)
    //    0x02 => normal noise, measurement mode
    //    (0b00000010)
    ADXL362_WriteReg(ADXL362_REG_POWER_CTL, 0x02);
}

/**
  * @brief  Read the 3-axis (12-bit) acceleration from ADXL362.
  * @param  xRaw, yRaw, zRaw: pointers to int16_t that receive the measurements
  */
void ADXL362_ReadXYZ(int16_t* xRaw, int16_t* yRaw, int16_t* zRaw)
{
    // The 12-bit data registers:
    // XDATA_L = 0x0E, XDATA_H = 0x0F
    // YDATA_L = 0x10, YDATA_H = 0x11
    // ZDATA_L = 0x12, ZDATA_H = 0x13
    // Read all 6 bytes in burst mode:
    uint8_t txBuf[2];
    uint8_t rxBuf[6];

    txBuf[0] = ADXL362_CMD_READ_REG;  // 0x0B
    txBuf[1] = 0x0E;                  // Start at XDATA_L

    ADXL362_Select();
    // Transmit the read command + start address
    HAL_SPI_Transmit(&hspi1, txBuf, 2, HAL_MAX_DELAY);
    // Now read 6 bytes
    HAL_SPI_Receive(&hspi1, rxBuf, 6, HAL_MAX_DELAY);
    ADXL362_Unselect();

    // Reassemble each axis (little-endian)
    *xRaw = (int16_t)((rxBuf[1] << 8) | rxBuf[0]);
    *yRaw = (int16_t)((rxBuf[3] << 8) | rxBuf[2]);
    *zRaw = (int16_t)((rxBuf[5] << 8) | rxBuf[4]);
}
