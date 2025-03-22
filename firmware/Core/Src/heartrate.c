#include "heartrate.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

#define MAX30102_ADDR  0xAE  // 7-bit address shifted left

// Define global variables ONLY in `heartrate.c`
uint32_t ir_buffer[50];
uint8_t  buffer_index     = 0;
uint32_t last_peak_time   = 0;
uint8_t  bpm             = 0;


void MAX30102_Init() {
    uint8_t mode_config[2] = {0x09, 0x03};  // SpO2 mode
    uint8_t spo2_config[2] = {0x0A, 0x27};  // 100 samples/sec, 411us pulse width
    uint8_t led_config[2] = {0x0C, 0x24};   // LED pulse amplitude

    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, mode_config, 2, 100);
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, spo2_config, 2, 100);
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, led_config, 2, 100);
}

uint32_t MAX30102_ReadFIFO() {
    uint8_t reg = 0x07;
    uint8_t data[6];

    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR, data, 6, 100);

    uint32_t ir_value = (data[0] << 16) | (data[1] << 8) | data[2];  // IR LED Data
    return ir_value;
}

void Process_HeartRate() {
    static uint8_t beat_count = 0;
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();  // system time in ms

    uint32_t ir_value = MAX30102_ReadFIFO();
    ir_buffer[buffer_index++] = ir_value;

    if (buffer_index >= 50) {
        buffer_index = 0;
    }

    if (ir_value > 50000 && (current_time - last_time) > 300) {
        beat_count++;
        last_time = current_time;
    }

    if ((current_time - last_peak_time) > 10000) {
        bpm = beat_count * 6;
        beat_count = 0;
        last_peak_time = current_time;
    }
}

void Send_HeartRate_UART() {
    printf("Heart Rate: %d BPM\r\n", bpm);
}
