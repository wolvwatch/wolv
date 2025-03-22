#include "heartrate.h"
#include "max30102_for_stm32_hal.h"

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

#define MAX30102_ADDR  0xAE  // 7-bit address shifted left

static max30102_t gMaxSensor;

uint32_t red_buffer[50];
uint8_t  buffer_index     = 0;
float    bpm             = 0.0f;
uint32_t last_peak_time   = 0;


void MAX30102_Init() {
	// 1) Initialize the max30102 driver struct with our I2C handle
	    max30102_init(&gMaxSensor, &hi2c1);

	    // 2) Reset the sensor
	    max30102_reset(&gMaxSensor);
	    HAL_Delay(50);  // small delay after reset

	    // 3) Choose Heart-Rate Only mode
	    //    (the “driver” enumerates HR mode as 0x02)
	    max30102_set_mode(&gMaxSensor, max30102_heart_rate);

	    // 4) Configure sampling rate = 100 Hz
	    max30102_set_sampling_rate(&gMaxSensor, max30102_sr_100);

	    // 5) Set pulse width to 18-bit (411 µs):
	    max30102_set_led_pulse_width(&gMaxSensor, max30102_pw_18_bit);

	    // 6) Set ADC range to 4096 nA
	    max30102_set_adc_resolution(&gMaxSensor, max30102_adc_4096);

	    // 7) Set LED current to ~7 mA for Red LED (LED #2 in this driver’s naming)
	    //    If you want IR off, set it to 0 or skip entirely.
	    max30102_set_led_current_2(&gMaxSensor, 7.0f);  // Red
	    max30102_set_led_current_1(&gMaxSensor, 0.0f);  // IR off (optional for pure HR)

	    // 8) Configure FIFO averaging, rollover, etc
	    //    e.g. average 4 samples, allow rollover, A_FULL = 17 empty samples left
	    max30102_set_fifo_config(&gMaxSensor, max30102_smp_ave_4, 1, 0x0F);

	    // 9) Clear the FIFO pointers so it starts fresh
	    max30102_clear_fifo(&gMaxSensor);
}

//uint32_t MAX30102_ReadFIFO() {
//    uint8_t reg = 0x07;
//    uint8_t data[6];
//
//    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, &reg, 1, 100);
//    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR, data, 6, 100);
//
//    uint32_t ir_value = (data[0] << 16) | (data[1] << 8) | data[2];  // IR LED Data
//    return ir_value;
//}

uint32_t MAX30102_ReadFIFO(void)
{
    uint8_t reg = 0x07;
    uint8_t data[6];

    HAL_I2C_Master_Transmit(&hi2c1, MAX30102_ADDR, &reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, MAX30102_ADDR, data, 3, 100);
    uint32_t red_value = ((uint32_t)data[0] << 16) |
                         ((uint32_t)data[1] << 8)  |
                         ((uint32_t)data[2]);
    return red_value;
}
//
//void Process_HeartRate(void)
//{
//    static uint32_t last_beat_time = 0;    // ms timestamp of last detected beat
//    static float    filtered_bpm = 0.0f;   // smoothed BPM, if desired
//
//    uint32_t current_time = HAL_GetTick();  // system time in ms
//    uint32_t ir_value     = MAX30102_ReadFIFO();
//
//    // Store IR in a small ring buffer (optionally useful for debugging)
//    ir_buffer[buffer_index++] = ir_value;
//    if (buffer_index >= 50)
//        buffer_index = 0;
//
//    // Very simplistic beat detection threshold
//    // "peak" if IR > threshold AND at least 300 ms since last beat
//    if (ir_value > 50000 && (current_time - last_beat_time) > 300)
//    {
//        // Time between beats (ms):
//        uint32_t delta = current_time - last_beat_time;
//        last_beat_time = current_time;
//
//        // Instant BPM = 60,000 ms/min / delta (ms/beat)
//        float instantaneous_bpm = 60000.0f / (float)delta;
//
//        // Optionally smooth the instantaneous BPM reading:
//        // e.g. a simple exponential smoothing
//        // filtered_bpm = alpha * instantaneous_bpm + (1 - alpha) * filtered_bpm;
//        // pick alpha ~ 0.3 for moderate smoothing
//        float alpha = 0.3f;
//        filtered_bpm = alpha * instantaneous_bpm + (1.0f - alpha) * filtered_bpm;
//
//        // For demonstration, let's store BPM as filtered reading
//        bpm = filtered_bpm;
//    }
//
//    if ((current_time - last_beat_time) > 2000)
//        {
//            bpm = 0.0f;
//            filtered_bpm = 0.0f;  // optional, so we reset the filter too
//        }
//}

void Process_HeartRate(void)
{
    static float filtered_bpm = 0.0f;

    // Read new samples
    max30102_read_fifo(&gMaxSensor);

    // first new sample
    uint32_t red_value = gMaxSensor._red_samples[0];

    // Reset that index for next time
    gMaxSensor._red_samples[0] = 0;
    uint32_t current_time = HAL_GetTick();
    if (red_value > 50000 && (current_time - last_peak_time > 300))
    {
        uint32_t delta = current_time - last_peak_time;
        last_peak_time = current_time;

        float inst_bpm = 60000.0f / (float)delta;
        float alpha = 0.3f;
        filtered_bpm = alpha * inst_bpm + (1.0f - alpha) * filtered_bpm;
        bpm = filtered_bpm;
    }

    if ((current_time - last_peak_time) > 2000)
    {
        bpm = 0.0f;
        filtered_bpm = 0.0f;
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM5)
    {
        Process_HeartRate();
    }
}

void Send_HeartRate_UART() {
	printf("Heart Rate: %.1f BPM\r\n", bpm);
}
