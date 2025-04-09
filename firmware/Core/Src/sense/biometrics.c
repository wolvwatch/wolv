#include "sense/biometrics.h"

#include <stdlib.h>

#include "drivers/max30102.h"
#include "sense/filter.h"

#define HR_WINDOW 750

biometric_callback_func_t *callbacks;
uint16_t callbacks_size = 0;

extern max_struct_t max30102_sensor;

void update_biometrics() {
    BWBandPass *filter = create_bw_band_pass_filter(4, 25, 0.5, 3);
    PeakDetector detector;
    PeakDetector_Init(&detector, 10, 10);
    uint16_t count = 0;
    uint16_t peaks = 0;

    uint16_t new_samples = max30102_read_data();
    for (uint16_t i = 0; i < new_samples; i++) {
        float hr_reading = max30102_sensor.red[max30102_sensor.tail++];
        max30102_sensor.tail %= 32;
        if (hr_reading < 80000) continue; // not touching skin
        count++;

        hr_reading = bw_band_pass(filter, hr_reading);
        peaks += PeakDetector_Process(&detector, hr_reading);

        if (count == HR_WINDOW) {
            float bpm = peaks * (HR_WINDOW / 1500.0);
            for (uint16_t j = 0; j < callbacks_size; j++) callbacks[j]((uint16_t) bpm, 0);
            peaks = 0;
            count = 0;
        }
    }
}

void add_biometric_callback(biometric_callback_func_t func) {
    if (callbacks_size++ == 0) {
        callbacks = malloc(sizeof(biometric_callback_func_t));
        callbacks[0] = func;
        return;
    }

    callbacks = realloc(callbacks, ++callbacks_size * sizeof(biometric_callback_func_t));
    callbacks[callbacks_size - 1] = func;
}
