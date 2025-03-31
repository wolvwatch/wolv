/**
******************************************************************************
  * @file    accel.h
  * @brief   Header file for ADXL362 accelerometer driver (accel.c)
  ******************************************************************************
  * @attention
  *
  * (c) 2025 Your Company or Name
  *
  ******************************************************************************
  */
#ifndef __ACCEL_H
#define __ACCEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
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
    void ADXL362_ReadXYZ(int16_t* xRaw, int16_t* yRaw, int16_t* zRaw);

#ifdef __cplusplus
}
#endif

#endif /* __ACCEL_H */