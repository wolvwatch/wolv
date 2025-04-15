#include "sense/accel.h"

#include <stdbool.h>
#include <stdlib.h>

/**
 * NOTE: this algorithm was implemented using the "Step Counting Using the ADXL367" resource from Analog Devices.
 * https://www.analog.com/en/resources/app-notes/an-2554.html
 */
#define SENSITIVITY 400.0
#define INIT_OFFSET_VALUE 4000

static uint32_t raw[4];
static bool raw_filled = false;
static float filtered[17];
static float thresh[4];

static uint16_t steps = 0, thresh_count = 0, max_min_sample_count = 0, step_to_step_count = 0, possible_steps = 0;
static uint8_t raw_idx = 0, filtered_idx = 0, min_idx = 0, max_idx = 0, thresh_idx = 0;
static float max_val = 0, min_val = 0, last_min_val = 0, last_max_val = 0, difference = 0, last_thresh = 0, thresh_val = 0, dyn_thresh = 0;
static bool detected_max = false, above_thresh = false, reg_mode = false;

void add_to_buf(const uint32_t val) {
    raw[raw_idx++] = val;
    if (!raw_filled && raw_idx == 4) raw_filled = true;
    raw_idx %= 4;
}

float get_buf_avg() {
    uint32_t sum = 0;
    for (int i = 0; i < 4; i++) sum += raw[i];
    return sum / 4.0;
}

void update_steps(const uint16_t x, const uint16_t y, const uint16_t z) {
    add_to_buf(abs(x) + abs(y) + abs(z));
    if (!raw_filled) return;
    filtered[filtered_idx++] = get_buf_avg();
    filtered_idx %= 4;

    max_val = filtered[0];
    max_idx = 0;
    for (int i = 1; i < 4; i++) {
        if (filtered[i] > max_val) {
            max_val = filtered[i];
            max_idx = i;
        }
    }

    min_val = filtered[0];
    min_idx = 0;
    for (int i = 1; i < 4; i++) {
        if (filtered[i] < min_val) {
            min_val = filtered[i];
            min_idx = i;
        }
    }

    // max value is in middle of window
    if (max_idx == (filtered_idx + 17/2) % 17) {
        if (!detected_max) {
            detected_max = true;
            last_max_val = max_val;
            max_min_sample_count = 0;
        } else {
            last_min_val = min_val;
            difference = last_max_val - last_min_val;
            detected_max = false;
            max_min_sample_count = 0;

            if (last_max_val > last_thresh + SENSITIVITY/2 && last_min_val < last_thresh - SENSITIVITY/2) {
                above_thresh = true;
                thresh_count = 0;
            } else {
                thresh_count++;
            }

            if (difference > SENSITIVITY) {
                thresh_val = (last_min_val + last_max_val) / 2;
                dyn_thresh = dyn_thresh - thresh[thresh_idx] + thresh_val;
                last_thresh = dyn_thresh / 4;
                thresh[thresh_idx] = thresh_val;
                thresh_idx++;
                thresh_idx %= 4;

                if (above_thresh) {
                    above_thresh = 0;
                    step_to_step_count = 0;
                    max_min_sample_count = 0;

                    if (reg_mode) {
                        steps++;
                    } else {
                        possible_steps++;
                        if (possible_steps == 8) {
                            steps += possible_steps;
                            possible_steps = 0;
                            reg_mode = true;
                        }
                    }
                }
            }

            min_val = 0;
            if (thresh_count > 1) {
                thresh_count = 0;
                max_min_sample_count = 0;
                max_val = 0;
                min_val = 0;
                reg_mode = false;
                possible_steps = 0;
                thresh_count = 0;
            }
        }
    } else if (detected_max) {
        max_min_sample_count++;
        if (max_min_sample_count == 100) {
            max_min_sample_count = 0;
            max_val = 0;
            min_val = 0;
            detected_max = false;
            possible_steps = 0;
        }
    }

    step_to_step_count++;
    if (step_to_step_count >= 100) {
        step_to_step_count = 0;
        possible_steps = 0;
        if (reg_mode) {
            reg_mode = false;
            thresh_val = 4000;
            dyn_thresh = 4000*4;
            thresh_idx = 0;
            thresh_count = 0;
            for (int i = 0; i < 4; i++) thresh[i] = 4000;
        }
        max_min_sample_count = 0;
    }
}

void update_raise_to_wake(const uint16_t x, const uint16_t y, const uint16_t z) {
// TODO implement
}