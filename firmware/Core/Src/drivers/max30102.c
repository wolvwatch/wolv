#include "max30102.h"
#include "stm32l4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
#define MAX30102_ADDR  0xAE

#define FIFO_CFG_REG 0x08
#define MODE_CFG_REG 0x09
#define IR_EN_REG 0x02
#define LED_PULSE_AMP_LED1 0x0C
#define LED_PULSE_AMP_LED2 0x0C

#define FIFO_WR_PTR_REG 0x04
#define FIFO_OVF_PTR_REG 0x05
#define FIFO_RD_PTR_REG 0x06
#define FIFO_DATA_REG 0x07

void tx(uint8_t sub_addr, uint8_t *data, const uint16_t size) {
    uint8_t buf[size + 1];
    buf[0] = sub_addr;
    for (int i = 0; i < size; i++) buf[i + 1] = data[i];
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, buf, size + 1, HAL_MAX_DELAY);
}

void rx(uint8_t sub_addr, uint8_t *data, const uint16_t size) {
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, &sub_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR, data, size, HAL_MAX_DELAY);
}

static uint8_t get_fifo_wr_ptr() {
    uint8_t ptr;
    rx(FIFO_WR_PTR_REG, &ptr, 1);
    return ptr;
}

static uint8_t get_fifo_rd_ptr() {
    uint8_t ptr;
    rx(FIFO_RD_PTR_REG, &ptr, 1);
    return ptr;
}

#define M_RESET 0b1 << 6
#define M_HR 0b010
#define F_SMP_AVE 0b100 << 5
#define F_ROLLOVER_EN 0b0 << 4
#define IR_A_FULL 0b1 << 7

void max30102_init() {
    uint8_t fifo_cfg = F_SMP_AVE | F_ROLLOVER_EN;
    uint8_t mode_cfg = M_RESET | M_HR;
    uint8_t ir_cfg = IR_A_FULL;
    uint8_t led1_a = 0x2F;
    uint8_t led2_a = 0;

    tx(MODE_CFG_REG, &mode_cfg, 1);
    HAL_Delay(200);
    tx(FIFO_CFG_REG, &fifo_cfg, 1);
    tx(IR_EN_REG, &ir_cfg, 1);
    tx(LED_PULSE_AMP_LED1, &led1_a, 1);
    tx(LED_PULSE_AMP_LED2, &led2_a, 1);
}

static uint32_t red_spls[16] = {};
static uint32_t ir_spls[16] = {};

void max30102_reset_fifo() {
    uint8_t reset = 0;
    tx(FIFO_WR_PTR_REG, &reset, 1);
    tx(FIFO_OVF_PTR_REG, &reset, 1);
    tx(FIFO_RD_PTR_REG, &reset, 1);
}

void max30102_read_fifo() {
    const uint8_t num_samples = get_fifo_wr_ptr() - get_fifo_rd_ptr();
    uint32_t red_spl = 0, ir_spl = 0;
    uint8_t data;
    for (uint8_t i = 0; i < num_samples; i++) {
        rx(FIFO_DATA_REG, &data, 1);
        red_spl |= data << 16;
        rx(FIFO_DATA_REG, &data, 1);
        red_spl |= data << 8;
        rx(FIFO_DATA_REG, &data, 1);
        red_spl |= data;

        rx(FIFO_DATA_REG, &data, 1);
        ir_spl |= data << 16;
        rx(FIFO_DATA_REG, &data, 1);
        ir_spl |= data << 8;
        rx(FIFO_DATA_REG, &data, 1);
        ir_spl |= data;

        red_spls[i] = red_spl;
        ir_spls[i] = ir_spl;
    }
}