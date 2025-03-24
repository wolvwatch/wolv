#include "max30102.h"
#include "stm32l4xx_hal.h"

extern I2C_HandleTypeDef hi2c1;
#define MAX30102_ADDR  0xAE

#define IR_STATUS_1 0x00
#define A_FULL 0b1 << 7
#define PPG_RDY 0b1 << 6
#define ALC_OVF 0b1 << 5
#define PWR_RDY 0b1

#define IR_STATUS_2 0x01
#define IR_DIE_TMP_RDY 0b1 << 1

#define IR_EN_1 0x02
#define IR_EN_2 0x03
#define FIFO_WR 0x04
#define FIFO_OVF 0x05
#define FIFO_RD 0x06
#define FIFO_DATA 0x07

#define FIFO_CFG 0x08
#define SMP_AVE_1 0b000 << 5
#define SMP_AVE_2 0b001 << 5
#define SMP_AVE_4 0b010 << 5
#define SMP_AVE_8 0b011 << 5
#define SMP_AVE_16 0b100 << 5
#define SMP_AVE_32 0b101 << 5
#define FIFO_ROLLOVER_EN 0b1 << 4

#define MODE_CFG 0x09
#define SHDN 0b1 << 7
#define RESET 0b1 << 6
#define MODE_HR 0b010
#define MODE_SPO2 0b011
#define MODE_MULTI 0b111

#define SPO2_CFG 0x0A
#define LED_PULSE_AMP 0x0C
#define LED_PULSE_AMP_HR 0x24
#define LED_PULSE_AMP_SPO2 0x00

#define MULTI_LED_CFG 0x11

void tx_byte(const uint8_t sub_addr, const uint8_t data) {
    uint8_t buf[2] = {sub_addr, data};
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, buf, 2, HAL_MAX_DELAY);
}

void max30102_init() {
    tx_byte(MODE_CFG, RESET | MODE_HR);
    HAL_Delay(50); // wait for reset
    tx_byte(MODE_CFG, RESET | MODE_HR);
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