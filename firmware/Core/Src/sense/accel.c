#include "sense/accel.h" // Assuming this declares update_accel_data etc.

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <math.h>   // For fabs
#include "main.h"
#include "displays/data.h" // Assumes this declares g_app_data

#include "drivers/adxl362.h" // Assumes this declares ADXL362_ReadXYZ

// --- Robust Step Counting Algorithm ---

// --- Configuration Constants (TUNABLE) ---
#define SAMPLING_RATE_HZ 50.0f
#define SAMPLING_INTERVAL_MS (1000.0f / SAMPLING_RATE_HZ)

// Filter for smoothing the magnitude signal (Simple Moving Average)
#define MAG_FILTER_SIZE 8

// Filter for estimating the slowly changing baseline/average magnitude (SMA)
// INCREASED SIZE to slow down baseline adaptation further
#define BASELINE_FILTER_SIZE 200

// Threshold: A peak must be this much *above* the adaptive baseline to be considered.
// LOWERED SIGNIFICANTLY based on log analysis showing peaks often < Base + 250
#define STEP_PEAK_THRESHOLD 125.0f

// Timing constraints for valid steps (in milliseconds)
#define MIN_STEP_INTERVAL_MS 200.0f
#define MAX_STEP_INTERVAL_MS 2000.0f

// How many consecutive valid steps are needed to confirm walking/running
#define REG_MODE_COUNT 4

// Timeout: If no valid step peak detected for this duration (ms), reset consecutive count.
#define STEP_DETECTION_TIMEOUT_MS (MAX_STEP_INTERVAL_MS * 2) // e.g., 4 seconds


// --- Static Variables ---
// Buffers for filters
static float mag_filter_buf[MAG_FILTER_SIZE];
// *** IMPORTANT: Update baseline buffer size ***
static float baseline_filter_buf[BASELINE_FILTER_SIZE]; // Size updated
static uint8_t mag_filter_idx = 0;
// *** Baseline index needs potentially larger type if size > 255 ***
static uint8_t baseline_filter_idx = 0; // Use uint16_t for safety
static float mag_filter_sum = 0.0f;
static float baseline_filter_sum = 0.0f;
static bool mag_filter_filled = false;
static bool baseline_filter_filled = false;

// Peak detection state
static float last_filtered_mag = 0.0f;
static bool is_potential_peak = false;

// Timing and step counting state
static uint32_t last_step_timestamp_ms = 0;
static uint32_t last_peak_timestamp_ms = 0;
static uint16_t consecutive_valid_steps = 0;
static bool in_regulation_mode = false;
static uint16_t total_steps = 0;

extern app_data_t g_app_data;


// --- Helper Functions ---

// Simple Moving Average Update (using uint16_t for index/size)
float update_sma(float new_value, float* buffer, uint8_t* index, float* sum, bool* filled, const uint16_t size) {
    uint16_t current_idx = *index;

    if (*filled) {
        *sum -= buffer[current_idx];
    }

    buffer[current_idx] = new_value;
    *sum += new_value;

    *index = (current_idx + 1) % size;

    // Check if buffer just became full *after* index increment
    if (!(*filled) && (*index == 0) && (current_idx == size - 1)) {
         *filled = true;
    }

    if (*filled) {
        return *sum / (float)size;
    } else {
        // Correct average calculation when not filled: sum / number_of_elements_added
        // If index is 0 after incrementing, it means we just added the last element (size-1 index)
        // or we just added the first element (index was 0, now 1).
        uint16_t count = (*index == 0) ? size : *index; // Number of elements currently used
        return (count == 0) ? new_value : (*sum / (float)count); // Avoid division by zero if called before first element added
    }
}


// --- Main Step Counter Logic ---

// Call this once at startup
void init_step_counter_robust(void) {
    memset(mag_filter_buf, 0, sizeof(mag_filter_buf));
    memset(baseline_filter_buf, 0, sizeof(baseline_filter_buf)); // Use updated size
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

    // Pre-fill baseline with estimate from idle log data (~980)
    float idle_guess = 980.0f;
    for(int i=0; i<BASELINE_FILTER_SIZE; ++i) baseline_filter_buf[i] = idle_guess;
    baseline_filter_sum = idle_guess * BASELINE_FILTER_SIZE;
    baseline_filter_filled = true; // Assume pre-filled
    last_filtered_mag = idle_guess; // Also init last_filtered_mag to avoid initial false peak
}

// Call this for every new accelerometer sample (x, y, z) at 50Hz
void update_steps_robust(const int16_t x, const int16_t y, const int16_t z) {
    // 1. Calculate scalar magnitude
    float current_mag = (float)abs(x) + (float)abs(y) + (float)abs(z);

    // 2. Update short-term filter
    float filtered_mag = update_sma(current_mag, mag_filter_buf, &mag_filter_idx, &mag_filter_sum, &mag_filter_filled, MAG_FILTER_SIZE);

    // 3. Update long-term baseline filter
    float baseline_mag = update_sma(filtered_mag, baseline_filter_buf, &baseline_filter_idx, &baseline_filter_sum, &baseline_filter_filled, BASELINE_FILTER_SIZE);

    // Filters need to be filled
    if (!mag_filter_filled || !baseline_filter_filled) {
        last_filtered_mag = filtered_mag;
        return;
    }

    uint32_t current_time_ms = HAL_GetTick();

    // 4. Simple Peak Detection Logic
    if (filtered_mag < last_filtered_mag) { // Signal is now falling
        if (is_potential_peak) {          // And it was rising just before -> Peak was at last_filtered_mag
            float peak_value = last_filtered_mag;

            // 5. Qualify the Peak
            if (peak_value > baseline_mag + STEP_PEAK_THRESHOLD) {
                uint32_t time_diff_ms = current_time_ms - last_peak_timestamp_ms;
                // Handle timestamp wraparound
                if (current_time_ms < last_peak_timestamp_ms) {
                    time_diff_ms = (0xFFFFFFFF - last_peak_timestamp_ms) + current_time_ms + 1;
                }

                // Treat first qualified peak specially for timing check
                if (last_peak_timestamp_ms == 0) {
                     time_diff_ms = MIN_STEP_INTERVAL_MS; // Allow first peak
                }


                if (time_diff_ms >= MIN_STEP_INTERVAL_MS && time_diff_ms <= MAX_STEP_INTERVAL_MS) {
                    // Timing valid!
                    last_step_timestamp_ms = current_time_ms; // Use current time for step timeout check
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

                } else { // Timing invalid
                    // Reset consecutive count if timing fails, might miss steps but avoids false positives from bursts
                    consecutive_valid_steps = 0;
                    // Option: Should we exit regulation mode on a single timing failure? Maybe not immediately.
                    // in_regulation_mode = false;
                }
                // Record the time of this *qualified* peak for the *next* interval check
                 // Use approximate time of the actual peak (one sample ago)
                last_peak_timestamp_ms = current_time_ms - (uint32_t)SAMPLING_INTERVAL_MS;
                 if (current_time_ms < (uint32_t)SAMPLING_INTERVAL_MS) { // Handle wraparound near 0
                     last_peak_timestamp_ms = 0; // Or estimate differently
                 }


            }
             // else: Peak below threshold
         }
         is_potential_peak = false; // Reset flag as signal is falling or flat
    } else if (filtered_mag > last_filtered_mag) {
        is_potential_peak = true; // Signal is rising
    }
    // else: signal is flat, is_potential_peak remains unchanged

    last_filtered_mag = filtered_mag;

    // 6. Handle Step Detection Timeout (Reset consecutive count if no valid steps detected)
     // Check time since the last *valid step detection* time stamp
     if (last_step_timestamp_ms > 0 &&
        (current_time_ms - last_step_timestamp_ms > STEP_DETECTION_TIMEOUT_MS))
    {
         // Basic wrap check for timeout detection
         if (current_time_ms >= last_step_timestamp_ms) {
             consecutive_valid_steps = 0;
             in_regulation_mode = false;
             // Reset last step timestamp to prevent continuous timeout triggering after a pause
             last_step_timestamp_ms = 0;
             // Also reset peak timestamp so next peak starts fresh timing check
             last_peak_timestamp_ms = 0;
         }
    }


#if STEPS_DEBUG // Keep the debug print
    static uint32_t last_print_time = 0;
    if (HAL_GetTick() - last_print_time > 500) {
         printf("Steps:%u|Reg:%d|Cons:%u|Mag:%.1f|Filt:%.1f|Base:%.1f|PeakThr:%.1f\n",
                total_steps, in_regulation_mode, consecutive_valid_steps,
                current_mag, filtered_mag, baseline_mag, baseline_mag + STEP_PEAK_THRESHOLD);
         last_print_time = HAL_GetTick();
    }
#endif

}

// --- Main Update Function (Called periodically @ 50Hz) ---

void update_accel_data() {
    static int16_t x, y, z;
    ADXL362_ReadXYZ(&x, &y, &z);
    update_steps_robust(x, y, z);
    // update_raise_to_wake(x, y, z);
}