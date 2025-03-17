I2C_HandleTypeDef hi2c1;


// init i2c (need to do this through IOC file)
void MX_I2C1_Init(void) {
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 400000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }
}



#define MAX30102_ADDR  0xAE  // 7-bit address shifted left

void MAX30102_Init() {
    uint8_t mode_config[2] = {0x09, 0x03};  // SpO2 mode
    uint8_t spo2_config[2] = {0x0A, 0x27};  // 100 samples/sec, 411us pulse width
    uint8_t led_config[2] = {0x0C, 0x24};   // LED pulse amplitude

    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, mode_config, 2, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, spo2_config, 2, HAL_MAX_DELAY);
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, led_config, 2, HAL_MAX_DELAY);
}


// read data coming off of hr

uint32_t MAX30102_ReadFIFO() {
    uint8_t reg = 0x07;
    uint8_t data[6];
    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR, data, 6, HAL_MAX_DELAY);

    uint32_t ir_value = (data[0] << 16) | (data[1] << 8) | data[2];  // IR LED Data
    return ir_value;
}

#define BUFFER_SIZE 50
#define THRESHOLD 50000  // Adjust this threshold based on testing

uint32_t ir_buffer[BUFFER_SIZE];
uint8_t buffer_index = 0;
uint32_t last_peak_time = 0;
uint8_t bpm = 0;

void Process_HeartRate() {
    static uint8_t beat_count = 0;
    static uint32_t last_time = 0;
    uint32_t current_time = HAL_GetTick();  // system time in ms

    uint32_t ir_value = MAX30102_ReadFIFO();
    ir_buffer[buffer_index++] = ir_value;

    if (buffer_index >= BUFFER_SIZE) {
        buffer_index = 0;
    }

    // peak detection (this might not be needed)
    if (ir_value > THRESHOLD && (current_time - last_time) > 300) {
        beat_count++;
        last_time = current_time;
    }

    // BPM every 10 seconds
    if ((current_time - last_peak_time) > 10000) {
        bpm = beat_count * 6;  // scale up to beats per minute
        beat_count = 0;
        last_peak_time = current_time;
    }
}


// send to our putty terminal
void Send_HeartRate_UART() {
    char msg[50];
    sprintf(msg, "Heart Rate: %d BPM\r\n", bpm);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

// put this into the main while loop:

while (1) {
    Process_HeartRate();
    Send_HeartRate_UART();
    HAL_Delay(1000);  // Send BPM every 1 second
}


