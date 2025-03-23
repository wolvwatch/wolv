#include "heartrate.h"

extern I2C_HandleTypeDef hi2c1;

#define FIFO_CFG 0b10010000
#define MODE_CFG 0b01000010

#define MAX30102_ADDR  0xAE  // 7-bit address shifted left

#define FIFO_CFG_REG 0x08
#define MODE_CFG_REG 0x09
#define FIFO_WR_PTR_REG 0x04
#define FIFO_RD_PTR_REG 0x06

void hr_tx(uint8_t sub_addr, uint8_t *data, uint16_t size) {
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR | 0b0, &sub_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR | 0b0, data, size, HAL_MAX_DELAY);
}

void hr_rx(uint8_t sub_addr, uint8_t *data, uint16_t size) {
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR | 0b0, &sub_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR | 0b1, data, size, HAL_MAX_DELAY);
}

void init_hr() {
    uint8_t fifo_cfg = FIFO_CFG;
    uint8_t mode_cfg = MODE_CFG;
    hr_tx(MODE_CFG_REG, &mode_cfg, 1);
    HAL_Delay(200);
    hr_tx(FIFO_CFG_REG, &fifo_cfg, 1);
}

uint8_t get_fifo_wr_ptr() {
    uint8_t ptr;
    hr_rx(FIFO_WR_PTR_REG, &ptr, 1);
    return ptr;
}

uint8_t get_fifo_rd_ptr() {
    uint8_t ptr;
    hr_rx(FIFO_RD_PTR_REG, &ptr, 1);
    return ptr;
}

void update_hr_data() {
    const uint8_t num_samples = get_fifo_wr_ptr() - get_fifo_rd_ptr();
    for (uint8_t i = 0; i < num_samples; i++) {
        uint8_t hr_sample =
    }
}