#include "../Inc/drivers/max30102.h"
#include <stdio.h>
#include "string.h"
#include "../../Drivers/STM32L4xx_HAL_Driver/Inc/stm32l4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
#define MAX30102_ADDRESS 0x57

// Status Registers
static const uint8_t MAX30102_INTSTAT1 = 0x00;
static const uint8_t MAX30102_INTSTAT2 = 0x01;
static const uint8_t MAX30102_INTENABLE1 = 0x02;
static const uint8_t MAX30102_INTENABLE2 = 0x03;

// FIFO Registers
static const uint8_t MAX30102_FIFOWRITEPTR = 0x04;
static const uint8_t MAX30102_FIFOOVERFLOW = 0x05;
static const uint8_t MAX30102_FIFOREADPTR = 0x06;
static const uint8_t MAX30102_FIFODATA = 0x07;

// Configuration Registers
static const uint8_t MAX30102_FIFOCONFIG = 0x08;
static const uint8_t MAX30102_MODECONFIG = 0x09;
static const uint8_t MAX30102_PARTICLECONFIG = 0x0A; // Note, sometimes listed as "SPO2" config in datasheet (pg. 11)
static const uint8_t MAX30102_LED1_PULSEAMP = 0x0C;
static const uint8_t MAX30102_LED2_PULSEAMP = 0x0D;
static const uint8_t MAX30102_LED3_PULSEAMP = 0x0E;
static const uint8_t MAX30102_LED_PROX_AMP = 0x10;
static const uint8_t MAX30102_MULTILEDCONFIG1 = 0x11;
static const uint8_t MAX30102_MULTILEDCONFIG2 = 0x12;

// Die Temperature Registers
static const uint8_t MAX30102_DIETEMPINT = 0x1F;
static const uint8_t MAX30102_DIETEMPFRAC = 0x20;
static const uint8_t MAX30102_DIETEMPCONFIG = 0x21;

// Proximity Function Registers
static const uint8_t MAX30102_PROXINTTHRESH = 0x30;

// Part ID Registers
static const uint8_t MAX30102_REVISIONID = 0xFE;
static const uint8_t MAX30102_PARTID = 0xFF; // Should always be 0x15. Identical to MAX30102.

// MAX30102 Commands
// Interrupt configuration (pg 13, 14)
static const uint8_t MAX30102_INT_A_FULL_MASK = (uint8_t) ~0b10000000;
static const uint8_t MAX30102_INT_A_FULL_ENABLE = 0x80;
static const uint8_t MAX30102_INT_A_FULL_DISABLE = 0x00;

static const uint8_t MAX30102_INT_DATA_RDY_MASK = (uint8_t) ~0b01000000;
static const uint8_t MAX30102_INT_DATA_RDY_ENABLE = 0x40;
static const uint8_t MAX30102_INT_DATA_RDY_DISABLE = 0x00;

static const uint8_t MAX30102_INT_ALC_OVF_MASK = (uint8_t) ~0b00100000;
static const uint8_t MAX30102_INT_ALC_OVF_ENABLE = 0x20;
static const uint8_t MAX30102_INT_ALC_OVF_DISABLE = 0x00;

static const uint8_t MAX30102_INT_PROX_INT_MASK = (uint8_t) ~0b00010000;
static const uint8_t MAX30102_INT_PROX_INT_ENABLE = 0x10;
static const uint8_t MAX30102_INT_PROX_INT_DISABLE = 0x00;

static const uint8_t MAX30102_INT_DIE_TEMP_RDY_MASK = (uint8_t) ~0b00000010;
static const uint8_t MAX30102_INT_DIE_TEMP_RDY_ENABLE = 0x02;
static const uint8_t MAX30102_INT_DIE_TEMP_RDY_DISABLE = 0x00;

static const uint8_t MAX30102_SAMPLEAVG_MASK = (uint8_t) ~0b11100000;
static const uint8_t MAX30102_SAMPLEAVG_1 = 0x00;
static const uint8_t MAX30102_SAMPLEAVG_2 = 0x20;
static const uint8_t MAX30102_SAMPLEAVG_4 = 0x40;
static const uint8_t MAX30102_SAMPLEAVG_8 = 0x60;
static const uint8_t MAX30102_SAMPLEAVG_16 = 0x80;
static const uint8_t MAX30102_SAMPLEAVG_32 = 0xA0;

static const uint8_t MAX30102_ROLLOVER_MASK = 0xEF;
static const uint8_t MAX30102_ROLLOVER_ENABLE = 0x10;
static const uint8_t MAX30102_ROLLOVER_DISABLE = 0x00;

static const uint8_t MAX30102_A_FULL_MASK = 0xF0;

static const uint8_t MAX30102_SHUTDOWN_MASK = 0x7F;
static const uint8_t MAX30102_SHUTDOWN = 0x80;
static const uint8_t MAX30102_WAKEUP = 0x00;

static const uint8_t MAX30102_RESET_MASK = 0xBF;
static const uint8_t MAX30102_RESET = 0x40;

static const uint8_t MAX30102_MODE_MASK = 0xF8;
static const uint8_t MAX30102_MODE_REDONLY = 0x02;
static const uint8_t MAX30102_MODE_REDIRONLY = 0x03;
static const uint8_t MAX30102_MODE_MULTILED = 0x07;

static const uint8_t MAX30102_ADCRANGE_MASK = 0x9F;
static const uint8_t MAX30102_ADCRANGE_2048 = 0x00;
static const uint8_t MAX30102_ADCRANGE_4096 = 0x20;
static const uint8_t MAX30102_ADCRANGE_8192 = 0x40;
static const uint8_t MAX30102_ADCRANGE_16384 = 0x60;

static const uint8_t MAX30102_SAMPLERATE_MASK = 0xE3;
static const uint8_t MAX30102_SAMPLERATE_50 = 0x00;
static const uint8_t MAX30102_SAMPLERATE_100 = 0x04;
static const uint8_t MAX30102_SAMPLERATE_200 = 0x08;
static const uint8_t MAX30102_SAMPLERATE_400 = 0x0C;
static const uint8_t MAX30102_SAMPLERATE_800 = 0x10;
static const uint8_t MAX30102_SAMPLERATE_1000 = 0x14;
static const uint8_t MAX30102_SAMPLERATE_1600 = 0x18;
static const uint8_t MAX30102_SAMPLERATE_3200 = 0x1C;

static const uint8_t MAX30102_PULSEWIDTH_MASK = 0xFC;
static const uint8_t MAX30102_PULSEWIDTH_69 = 0x00;
static const uint8_t MAX30102_PULSEWIDTH_118 = 0x01;
static const uint8_t MAX30102_PULSEWIDTH_215 = 0x02;
static const uint8_t MAX30102_PULSEWIDTH_411 = 0x03;

static const uint8_t MAX30102_SLOT1_MASK = 0xF8;
static const uint8_t MAX30102_SLOT2_MASK = 0x8F;
static const uint8_t MAX30102_SLOT3_MASK = 0xF8;
static const uint8_t MAX30102_SLOT4_MASK = 0x8F;

static const uint8_t SLOT_NONE = 0x00;
static const uint8_t SLOT_RED_LED = 0x01;
static const uint8_t SLOT_IR_LED = 0x02;
static const uint8_t SLOT_GREEN_LED = 0x03;
static const uint8_t SLOT_NONE_PILOT = 0x04;
static const uint8_t SLOT_RED_PILOT = 0x05;
static const uint8_t SLOT_IR_PILOT = 0x06;
static const uint8_t SLOT_GREEN_PILOT = 0x07;

static const uint8_t MAX_30105_EXPECTEDPARTID = 0x15;

max_struct_t max30102_sensor = {};

void read_byte(uint8_t reg, uint8_t *buf) {
    HAL_StatusTypeDef result = HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDRESS << 1, &reg, 1, HAL_MAX_DELAY);
    if (result != HAL_OK) printf("I2C MAX30102 ERROR: reg %u result %u", reg, HAL_I2C_GetError(&hi2c1));

    result = HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDRESS << 1, buf, 1, HAL_MAX_DELAY);
    if (result != HAL_OK) printf("I2C MAX30102 ERROR: reg %u result %u", reg, HAL_I2C_GetError(&hi2c1));
}

void write_byte(uint8_t reg, uint8_t val) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = val;
    HAL_StatusTypeDef result = HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDRESS << 1, buf, 2, HAL_MAX_DELAY);
    if (result != HAL_OK) printf("I2C MAX30102 ERROR: reg %u result %u", reg, HAL_I2C_GetError(&hi2c1));
}

void bit_mask(uint8_t reg, uint8_t mask, uint8_t val) {
    uint8_t current_val;
    read_byte(reg, &current_val);
    current_val = current_val & mask;
    write_byte(reg, current_val | val);
}

void soft_reset() {
    bit_mask(MAX30102_MODECONFIG, MAX30102_RESET_MASK, MAX30102_RESET);
    uint8_t response;
    do {
        read_byte(MAX30102_MODECONFIG, &response);
        HAL_Delay(100);
    } while ((response & MAX30102_RESET) != 0);
}

void max30102_init() {
    soft_reset();

    bit_mask(MAX30102_FIFOCONFIG, MAX30102_SAMPLEAVG_MASK, MAX30102_SAMPLEAVG_8);
    bit_mask(MAX30102_FIFOCONFIG, MAX30102_ROLLOVER_MASK, MAX30102_ROLLOVER_DISABLE);
    bit_mask(MAX30102_FIFOCONFIG, MAX30102_A_FULL_MASK, 0x5);
    bit_mask(MAX30102_MODECONFIG, MAX30102_MODE_MASK, MAX30102_MODE_MULTILED);
    bit_mask(MAX30102_PARTICLECONFIG, MAX30102_ADCRANGE_MASK, MAX30102_ADCRANGE_4096);
    bit_mask(MAX30102_PARTICLECONFIG, MAX30102_SAMPLERATE_MASK, MAX30102_SAMPLERATE_200);
    bit_mask(MAX30102_PARTICLECONFIG, MAX30102_PULSEWIDTH_MASK, MAX30102_PULSEWIDTH_411);

    uint8_t power_lvl = 0x1F;
    write_byte(MAX30102_LED1_PULSEAMP, power_lvl);
    write_byte(MAX30102_LED2_PULSEAMP, power_lvl);

    bit_mask(MAX30102_MULTILEDCONFIG1, MAX30102_SLOT1_MASK, SLOT_RED_LED);
    bit_mask(MAX30102_MULTILEDCONFIG1, MAX30102_SLOT2_MASK, SLOT_IR_LED << 4);

    write_byte(MAX30102_FIFOWRITEPTR, 0);
    write_byte(MAX30102_FIFOOVERFLOW, 0);
    write_byte(MAX30102_FIFOREADPTR, 0);

    bit_mask(MAX30102_INTENABLE1, MAX30102_INT_A_FULL_MASK, MAX30102_INT_A_FULL_ENABLE);
    max30102_clear_int();
}

int max30102_read_data() {
    uint8_t read_ptr, write_ptr;
    read_byte(MAX30102_FIFOREADPTR, &read_ptr);
    read_byte(MAX30102_FIFOWRITEPTR, &write_ptr);

    int new_samps = 0;
    if (read_ptr != write_ptr) {
        new_samps = write_ptr - read_ptr;
        if (new_samps < 0) new_samps += 32;
        uint16_t bytes_left = new_samps * 6;

        while (bytes_left > 0) {
            uint16_t get = bytes_left;
            if (get > 32) get = 32 - (32 % 6);
            bytes_left -= get;

            uint8_t data = MAX30102_FIFODATA;
            HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDRESS << 1, &data, 1, HAL_MAX_DELAY);
            uint8_t buf[new_samps * 6];
            HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDRESS << 1, buf, get, HAL_MAX_DELAY);

            uint16_t i = 0;
            while (get > 0) {
                max30102_sensor.head++;
                max30102_sensor.head %= 32;

                uint32_t hr, spo2;
                uint8_t temp[4];

                temp[3] = 0;
                temp[2] = buf[i * 6];
                temp[1] = buf[i * 6 + 1];
                temp[0] = buf[i * 6 + 2];
                memcpy(&hr, temp, 4);
                hr &= 0x3FFFF;

                temp[3] = 0;
                temp[2] = buf[i * 6 + 3];
                temp[1] = buf[i * 6 + 4];
                temp[0] = buf[i * 6 + 5];
                memcpy(&spo2, temp, 4);
                spo2 &= 0x3FFFF;

                temp[3] = 0;
                max30102_sensor.red[max30102_sensor.head] = hr;
                max30102_sensor.IR[max30102_sensor.head] = spo2;
#if RAW_HR_DEBUG && RAW_SPO2_DEBUG
                printf("raw_hr:%d raw_spo2:%d\n", hr, spo2);
#elif RAW_HR_DEBUG
                printf("raw_hr:%d\n", hr);
#elif SPO2_DEBUG
                printf("raw_spo2:%d\n", spo2);
#endif
                i++;
                get -= 6;
            }
        }
    }

    return new_samps;
}

void max30102_clear_int() {
    uint8_t buf[1];
    read_byte(MAX30102_INTSTAT1, buf);
    read_byte(MAX30102_INTSTAT2, buf);
}
