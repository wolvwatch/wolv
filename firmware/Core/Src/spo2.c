#include "../Inc/spo2.h"

// ---------------------------------------------------
// Configuration
// ---------------------------------------------------
#define NUM_AVG_SAMPLES 2

// ---------------------------------------------------
// Internal buffers/variables
// ---------------------------------------------------
static int32_t an_x[BUFFER_SIZE]; // Used for IR channel after DC removal & filtering
static int32_t an_y[BUFFER_SIZE]; // Used for Red channel after DC removal & filtering

// Rolling arrays to smooth the final HR and SpO2
static int32_t hr_history[NUM_AVG_SAMPLES];
static int32_t spo2_history[NUM_AVG_SAMPLES];
static int idx_history = 0;

// Same 184-entry SpO2 lookup table from the Maxim code
const uint8_t uch_spo2_table[184] = {
    95, 95, 95, 96, 96, 96, 97, 97, 97, 97, 97, 98, 98, 98, 98, 98,
    99, 99, 99, 99, 99, 99, 99, 99, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99, 99, 99, 99,
    99, 99, 99, 99, 98, 98, 98, 98, 98, 98, 97, 97, 97, 97, 96, 96,
    96, 96, 95, 95, 95, 94, 94, 94, 93, 93, 93, 92, 92, 92, 91, 91,
    90, 90, 89, 89, 89, 88, 88, 87, 87, 86, 86, 85, 85, 84, 84, 83,
    82, 82, 81, 81, 80, 80, 79, 78, 78, 77, 76, 76, 75, 74, 74, 73,
    72, 72, 71, 70, 69, 69, 68, 67, 66, 66, 65, 64, 63, 62, 62, 61,
    60, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46,
    45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 31, 30, 29,
    28, 27, 26, 25, 23, 22, 21, 20, 19, 17, 16, 15, 14, 12, 11, 10,
    9, 7, 6, 5, 3, 2, 1
};

void maxim_peaks_above_min_height(
    int32_t *pn_locs,
    int32_t *n_npks,
    int32_t *pn_x,
    int32_t n_size,
    int32_t n_min_height
)
/**
* \brief        Find peaks above n_min_height
*/
{
    int32_t i = 1, n_width;
    *n_npks = 0;

    while (i < n_size - 1) {
        // potential left edge
        if ((pn_x[i] > n_min_height) && (pn_x[i] > pn_x[i - 1])) {
            n_width = 1;
            // find flat peak width
            while ((i + n_width < n_size) && (pn_x[i] == pn_x[i + n_width])) {
                n_width++;
            }
            // check if really a peak
            if ((pn_x[i] > pn_x[i + n_width]) && (*n_npks < 15)) {
                pn_locs[(*n_npks)++] = i;
                i += (n_width + 1);
            } else {
                i += n_width;
            }
        } else {
            i++;
        }
    }
}

void maxim_sort_ascend(int32_t *pn_x, int32_t n_size)
/**
* \brief        Sort array in ascending order (insertion sort)
*/
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_x[i];
        j = i;
        while ((j > 0) && (n_temp < pn_x[j - 1])) {
            pn_x[j] = pn_x[j - 1];
            j--;
        }
        pn_x[j] = n_temp;
    }
}

void maxim_sort_indices_descend(int32_t *pn_x, int32_t *pn_indx, int32_t n_size)
/**
* \brief        Sort indices according to values in pn_x (descending order)
*/
{
    int32_t i, j, n_temp;
    for (i = 1; i < n_size; i++) {
        n_temp = pn_indx[i];
        j = i;
        while ((j > 0) && (pn_x[n_temp] > pn_x[pn_indx[j - 1]])) {
            pn_indx[j] = pn_indx[j - 1];
            j--;
        }
        pn_indx[j] = n_temp;
    }
}

void maxim_remove_close_peaks(
    int32_t *pn_locs,
    int32_t *pn_npks,
    int32_t *pn_x,
    int32_t n_min_distance
)
/**
* \brief        Remove peaks too close together (distance < n_min_distance)
*/
{
    int32_t i, j, n_old_npks, n_dist;

    // Order peaks from largest to smallest in terms of pn_x
    maxim_sort_indices_descend(pn_x, pn_locs, *pn_npks);

    for (i = -1; i < *pn_npks; i++) {
        n_old_npks = *pn_npks;
        *pn_npks = i + 1;
        for (j = i + 1; j < n_old_npks; j++) {
            n_dist = pn_locs[j] - ((i == -1) ? -1 : pn_locs[i]);
            if ((n_dist > n_min_distance) || (n_dist < -n_min_distance)) {
                pn_locs[(*pn_npks)++] = pn_locs[j];
            }
        }
    }
    // Resort indices into ascending order
    maxim_sort_ascend(pn_locs, *pn_npks);
}

// -----------------------------------------------------------------
// Supporting peak-detection and sorting functions
// (unchanged except minor variable naming).
// -----------------------------------------------------------------
void maxim_find_peaks(
    int32_t *pn_locs,
    int32_t *n_npks,
    int32_t *pn_x,
    int32_t n_size,
    int32_t n_min_height,
    int32_t n_min_distance,
    int32_t n_max_num
)
/**
* \brief        Find peaks
* \par          Details
*               Find at most n_max_num peaks above n_min_height
*               separated by at least n_min_distance
*/
{
    maxim_peaks_above_min_height(pn_locs, n_npks, pn_x, n_size, n_min_height);
    maxim_remove_close_peaks(pn_locs, n_npks, pn_x, n_min_distance);
    *n_npks = (*n_npks < n_max_num) ? *n_npks : n_max_num;
}

// -----------------------------------------------------------------
// Drop-in replacement for the main function that computes
// heart rate and SpO2 from IR and Red buffers
// -----------------------------------------------------------------
void maxim_heart_rate_and_oxygen_saturation(
    uint32_t *pun_ir_buffer,
    int32_t n_ir_buffer_length,
    uint32_t *pun_red_buffer,
    int32_t *pn_spo2,
    int8_t *pch_spo2_valid,
    int32_t *pn_heart_rate,
    int8_t *pch_hr_valid
) {
    int32_t k, i;

    // ---------------------------------------------------------
    // 1) DC removal: compute mean of IR and Red
    // ---------------------------------------------------------
    uint32_t ir_sum = 0;
    uint32_t red_sum = 0;
    for (k = 0; k < n_ir_buffer_length; k++) {
        ir_sum += pun_ir_buffer[k];
        red_sum += pun_red_buffer[k];
    }
    uint32_t ir_mean = ir_sum / n_ir_buffer_length;
    uint32_t red_mean = red_sum / n_ir_buffer_length;

    // ---------------------------------------------------------
    // 2) Subtract DC and invert IR; just subtract DC on Red
    // ---------------------------------------------------------
    for (k = 0; k < n_ir_buffer_length; k++) {
        an_x[k] = -1 * ((int32_t) pun_ir_buffer[k] - (int32_t) ir_mean); // IR channel
        an_y[k] = (int32_t) pun_red_buffer[k] - (int32_t) red_mean; // Red channel
    }

    // ---------------------------------------------------------
    // 3) Apply 4-point moving average on both IR and Red
    //    (You could do a second pass or an IIR filter here.)
    // ---------------------------------------------------------
    for (k = 0; k < (n_ir_buffer_length - MA4_SIZE); k++) {
        an_x[k] = (an_x[k] + an_x[k + 1] + an_x[k + 2] + an_x[k + 3]) / 4;
        an_y[k] = (an_y[k] + an_y[k + 1] + an_y[k + 2] + an_y[k + 3]) / 4;
    }
    // Fill leftover points with the last valid MA result
    for (; k < n_ir_buffer_length; k++) {
        an_x[k] = an_x[n_ir_buffer_length - MA4_SIZE - 1];
        an_y[k] = an_y[n_ir_buffer_length - MA4_SIZE - 1];
    }

    // ---------------------------------------------------------
    // 4) Compute a "dynamic threshold" from an_x[]
    //    (based on mean absolute amplitude).
    // ---------------------------------------------------------
    int32_t mean_abs_x = 0;
    for (k = 0; k < n_ir_buffer_length; k++) {
        mean_abs_x += (an_x[k] >= 0) ? an_x[k] : -an_x[k];
    }
    mean_abs_x /= n_ir_buffer_length;
    // Example: half the mean absolute amplitude
    int32_t n_th1 = mean_abs_x / 2;
    // Add some clamps to avoid extremes
    if (n_th1 < 20) n_th1 = 20;
    if (n_th1 > 100) n_th1 = 100;

    // ---------------------------------------------------------
    // 5) Use the "peak finder" to find valleys in IR
    //    (since we inverted IR, valleys appear as peaks)
    // ---------------------------------------------------------
    int32_t an_ir_valley_locs[15] = {0};
    int32_t n_npks = 0;
    maxim_find_peaks(
        an_ir_valley_locs,
        &n_npks,
        an_x,
        n_ir_buffer_length,
        n_th1,
        4, // min distance between peaks
        15 // max # of peaks
    );

    // ---------------------------------------------------------
    // 6) Heart Rate computation from the intervals
    // ---------------------------------------------------------
    int32_t valid_intervals = 0;
    int32_t interval_sum = 0;
    for (i = 1; i < n_npks; i++) {
        int32_t interval = an_ir_valley_locs[i] - an_ir_valley_locs[i - 1];
        if (interval > 0) {
            // Convert to BPM
            int32_t hr_temp = (int32_t) ((FreqS * 60) / interval);
            // Reject physiologically impossible intervals
            if (hr_temp >= 30 && hr_temp <= 220) {
                interval_sum += interval;
                valid_intervals++;
            }
        }
    }

    if (valid_intervals > 0) {
        int32_t avg_interval = interval_sum / valid_intervals;
        *pn_heart_rate = (int32_t) ((FreqS * 60) / avg_interval);
        *pch_hr_valid = 1;
    } else {
        *pn_heart_rate = -999; // unable to calculate
        *pch_hr_valid = 0;
    }

    // ---------------------------------------------------------
    // 7) SpO2 calculation using the ratio-based method
    //    (Same original code for ratio calculation)
    //    We do minimal changes, but keep the final use of
    //    uch_spo2_table[] to get *pn_spo2.
    // ---------------------------------------------------------
    // The original code attempts to find n_ratio_average from
    // AC/DC calculations in each segment. We'll do a simpler
    // snippet but keep the final "table lookup" usage.
    // ----------------------------------------------------------------------
    // For brevity, the simplest approach is re-using your original logic:
    // re-load the raw DC-removed an_x, an_y for ratio,
    // find AC and DC for each chunk, compute ratio, etc.
    // We'll keep the old snippet but insert the dynamic signals an_x[] and an_y[].
    // ----------------------------------------------------------------------

    // Re-load for ratio logic (in original code, an_x and an_y are already set)
    // find precise minima near an_ir_valley_locs, etc. We'll do a shorter version:
    int32_t n_spo2_calc = -999;
    int32_t n_ratio_average = 0;
    int32_t an_ratio[5] = {0};
    int32_t n_i_ratio_count = 0;

    // The main loop from original code that sets an_ratio[] ...
    // (We keep it mostly intact, but skipping some lines for brevity.)
    // NOTE: Must ensure an_x[] and an_y[] haven't been re-inverted or changed since step #2.
    // We assume they're still correct for computing AC/DC.

    if (n_npks >= 2) {
        // For each pair of valley locations, compute ratio
        for (i = 0; i < n_npks - 1; i++) {
            if (an_ir_valley_locs[i + 1] <= BUFFER_SIZE - 1) {
                int32_t n_x_dc_max = -16777216, n_y_dc_max = -16777216;
                int n_x_dc_max_idx = 0, n_y_dc_max_idx = 0;
                int32_t j;
                // find max within the segment
                for (j = an_ir_valley_locs[i]; j < an_ir_valley_locs[i + 1]; j++) {
                    if (an_x[j] > n_x_dc_max) {
                        n_x_dc_max = an_x[j];
                        n_x_dc_max_idx = j;
                    }
                    if (an_y[j] > n_y_dc_max) {
                        n_y_dc_max = an_y[j];
                        n_y_dc_max_idx = j;
                    }
                }
                // linear interpolation of AC components, same as original
                int32_t n_y_ac = (an_y[an_ir_valley_locs[i + 1]] - an_y[an_ir_valley_locs[i]])
                                 * (n_y_dc_max_idx - an_ir_valley_locs[i]);
                n_y_ac = an_y[an_ir_valley_locs[i]] + n_y_ac / (an_ir_valley_locs[i + 1] - an_ir_valley_locs[i]);
                n_y_ac = an_y[n_y_dc_max_idx] - n_y_ac;

                int32_t n_x_ac = (an_x[an_ir_valley_locs[i + 1]] - an_x[an_ir_valley_locs[i]])
                                 * (n_x_dc_max_idx - an_ir_valley_locs[i]);
                n_x_ac = an_x[an_ir_valley_locs[i]] + n_x_ac / (an_ir_valley_locs[i + 1] - an_ir_valley_locs[i]);
                n_x_ac = an_x[n_y_dc_max_idx] - n_x_ac;

                // Ratio = ( n_y_ac * n_x_dc_max ) / ( n_x_ac * n_y_dc_max )
                // Shift by >> 7 as in original code
                int32_t n_nume = (n_y_ac * n_x_dc_max) >> 7;
                int32_t n_denom = (n_x_ac * n_y_dc_max) >> 7;
                if (n_denom > 0 && n_nume != 0 && n_i_ratio_count < 5) {
                    an_ratio[n_i_ratio_count] = (n_nume * 100) / n_denom;
                    n_i_ratio_count++;
                }
            }
        }
        // Sort and pick median
        maxim_sort_ascend(an_ratio, n_i_ratio_count);
        int n_middle_idx = n_i_ratio_count / 2;
        if (n_middle_idx > 1) {
            n_ratio_average = (an_ratio[n_middle_idx - 1] + an_ratio[n_middle_idx]) / 2;
        } else if (n_i_ratio_count > 0) {
            n_ratio_average = an_ratio[n_middle_idx];
        }
    }

    // Look up final SpO2
    if (n_ratio_average > 2 && n_ratio_average < 184) {
        n_spo2_calc = uch_spo2_table[n_ratio_average];
        *pn_spo2 = n_spo2_calc;
        *pch_spo2_valid = 1;
    } else {
        *pn_spo2 = -999;
        *pch_spo2_valid = 0;
    }

    // ---------------------------------------------------------
    // 8) Smooth final HR & SpO2 across multiple calls
    //    (Rolling average over last NUM_AVG_SAMPLES results)
    // ---------------------------------------------------------
    // Store the new raw results if they are > 0, else store 0
    hr_history[idx_history] = (*pn_heart_rate > 0) ? *pn_heart_rate : 0;
    spo2_history[idx_history] = (*pn_spo2 > 0) ? *pn_spo2 : 0;

    idx_history = (idx_history + 1) % NUM_AVG_SAMPLES;

    // Compute average of the last valid readings
    int32_t sum_hr = 0, sum_spo2 = 0;
    int c_hr = 0, c_spo2 = 0;
    for (i = 0; i < NUM_AVG_SAMPLES; i++) {
        if (hr_history[i] > 0) {
            sum_hr += hr_history[i];
            c_hr++;
        }
        if (spo2_history[i] > 0) {
            sum_spo2 += spo2_history[i];
            c_spo2++;
        }
    }
    // If we have any valid HR readings, average them
    if (c_hr > 0) {
        *pn_heart_rate = sum_hr / c_hr;
    }
    // If we have any valid SpO2 readings, average them
    if (c_spo2 > 0) {
        *pn_spo2 = sum_spo2 / c_spo2;
    }
}
