#include "sense/accel.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "displays/data.h"
#include "drivers/adxl362.h"

#define SAMPLING_RATE_HZ 50.0f
#define SAMPLING_INTERVAL_MS (1000.0f / SAMPLING_RATE_HZ)
#define MAG_FILTER_SIZE 8
#define BASELINE_FILTER_SIZE 200
#define STEP_PEAK_THRESHOLD 125.0f
#define MIN_STEP_INTERVAL_MS 200.0f
#define MAX_STEP_INTERVAL_MS 2000.0f
#define REG_MODE_COUNT 4
#define STEP_DETECTION_TIMEOUT_MS (MAX_STEP_INTERVAL_MS * 2)

static float mag_filter_buf[MAG_FILTER_SIZE];
static float baseline_filter_buf[BASELINE_FILTER_SIZE];
static uint8_t mag_filter_idx = 0;
static uint8_t baseline_filter_idx = 0;
static float mag_filter_sum = 0.0f;
static float baseline_filter_sum = 0.0f;
static bool mag_filter_filled = false;
static bool baseline_filter_filled = false;
static float last_filtered_mag = 0.0f;
static bool is_potential_peak = false;
static uint32_t last_step_timestamp_ms = 0;
static uint32_t last_peak_timestamp_ms = 0;
static uint16_t consecutive_valid_steps = 0;
static bool in_regulation_mode = false;
static uint16_t total_steps = 0;

extern app_data_t g_app_data;

float update_sma(float new_value, float *buffer, uint8_t *index, float *sum, bool *filled, const uint16_t size) {
    uint16_t current_idx = *index;

    if (*filled) {
        *sum -= buffer[current_idx];
    }

    buffer[current_idx] = new_value;
    *sum += new_value;

    *index = (current_idx + 1) % size;

    if (!(*filled) && (*index == 0) && (current_idx == size - 1)) {
        *filled = true;
    }

    if (*filled) {
        return *sum / (float) size;
    } else {
        uint16_t count = (*index == 0) ? size : *index;
        return (count == 0) ? new_value : (*sum / (float) count);
    }
}

void init_step_counter_robust(void) {
    memset(mag_filter_buf, 0, sizeof(mag_filter_buf));
    memset(baseline_filter_buf, 0, sizeof(baseline_filter_buf));
    mag_filter_idx = 0;
    baseline_filter_idx = 0;
    mag_filter_sum = 0.0f;
    baseline_filter_sum = 0.0f;
    mag_filter_filled = false;
    baseline_filter_filled = false;

    last_filtered_mag = 0.0f;
    is_potential_peak = false;
    last_step_timestamp_ms = 0;
    last_peak_timestamp_ms = 0;
    consecutive_valid_steps = 0;
    in_regulation_mode = false;
    total_steps = 0;
    g_app_data.biometrics.steps = 0;

    float idle_guess = 980.0f;
    for (int i = 0; i < BASELINE_FILTER_SIZE; ++i) baseline_filter_buf[i] = idle_guess;
    baseline_filter_sum = idle_guess * BASELINE_FILTER_SIZE;
    baseline_filter_filled = true;
    last_filtered_mag = idle_guess;
}

// Call this for every new accelerometer sample (x, y, z) at 50Hz
void update_steps_robust(const int16_t x, const int16_t y, const int16_t z) {
    float current_mag = (float) abs(x) + (float) abs(y) + (float) abs(z);
    float filtered_mag = update_sma(current_mag, mag_filter_buf, &mag_filter_idx, &mag_filter_sum, &mag_filter_filled,
                                    MAG_FILTER_SIZE);
    float baseline_mag = update_sma(filtered_mag, baseline_filter_buf, &baseline_filter_idx, &baseline_filter_sum,
                                    &baseline_filter_filled, BASELINE_FILTER_SIZE);
    if (!mag_filter_filled || !baseline_filter_filled) {
        last_filtered_mag = filtered_mag;
        return;
    }

    uint32_t current_time_ms = HAL_GetTick();

    if (filtered_mag < last_filtered_mag) {
        // Signal is now falling
        if (is_potential_peak) {
            // And it was rising just before -> Peak was at last_filtered_mag
            float peak_value = last_filtered_mag;
            if (peak_value > baseline_mag + STEP_PEAK_THRESHOLD) {
                uint32_t time_diff_ms = current_time_ms - last_peak_timestamp_ms;
                // Handle timestamp wraparound
                if (current_time_ms < last_peak_timestamp_ms) {
                    time_diff_ms = (0xFFFFFFFF - last_peak_timestamp_ms) + current_time_ms + 1;
                }
                if (last_peak_timestamp_ms == 0) {
                    time_diff_ms = MIN_STEP_INTERVAL_MS;
                }


                if (time_diff_ms >= MIN_STEP_INTERVAL_MS && time_diff_ms <= MAX_STEP_INTERVAL_MS) {
                    last_step_timestamp_ms = current_time_ms;
                    consecutive_valid_steps++;

                    if (!in_regulation_mode) {
                        if (consecutive_valid_steps >= REG_MODE_COUNT) {
                            total_steps += consecutive_valid_steps;
                            in_regulation_mode = true;
                        }
                    } else {
                        total_steps++;
                    }
                    g_app_data.biometrics.steps = total_steps;
                } else {
                    // Reset consecutive count if timing fails, might miss steps but avoids false positives from bursts
                    consecutive_valid_steps = 0;
                }

                last_peak_timestamp_ms = current_time_ms - (uint32_t) SAMPLING_INTERVAL_MS;
                if (current_time_ms < (uint32_t) SAMPLING_INTERVAL_MS) {
                    // Handle wraparound near 0
                    last_peak_timestamp_ms = 0;
                }
            }
        }
        is_potential_peak = false; // Reset flag as signal is falling or flat
    } else if (filtered_mag > last_filtered_mag) {
        is_potential_peak = true; // Signal is rising
    }

    last_filtered_mag = filtered_mag;

    // Check time since the last *valid step detection* time stamp
    if (last_step_timestamp_ms > 0 &&
        (current_time_ms - last_step_timestamp_ms > STEP_DETECTION_TIMEOUT_MS)) {
        if (current_time_ms >= last_step_timestamp_ms) {
            consecutive_valid_steps = 0;
            in_regulation_mode = false;
            last_step_timestamp_ms = 0;
            last_peak_timestamp_ms = 0;
        }
    }


#if STEPS_DEBUG
    static uint32_t last_print_time = 0;
    if (HAL_GetTick() - last_print_time > 500) {
         printf("Steps:%u|Reg:%d|Cons:%u|Mag:%.1f|Filt:%.1f|Base:%.1f|PeakThr:%.1f\n",
                total_steps, in_regulation_mode, consecutive_valid_steps,
                current_mag, filtered_mag, baseline_mag, baseline_mag + STEP_PEAK_THRESHOLD);
         last_print_time = HAL_GetTick();
    }
#endif
}

void update_accel_data() {
    static int16_t x, y, z;
    ADXL362_ReadXYZ(&x, &y, &z);
    update_steps_robust(x, y, z);
}
