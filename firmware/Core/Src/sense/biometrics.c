#include "sense/biometrics.h"

#include <stdio.h>
#include <stdlib.h>

#include "drivers/max30102.h"
#include "sense/filter.h"
#include "displays/data.h"

#define HR_WINDOW 300.0

extern max_struct_t max30102_sensor;
extern app_data_t g_app_data;

void update_biometrics(BWBandPass *filter, PeakDetector *detector) {
    static uint16_t count = 0;
    static uint16_t peaks = 0;

    uint16_t new_samples = max30102_read_data();
    for (uint16_t i = 0; i < new_samples; i++) {
        float hr_reading = max30102_sensor.red[max30102_sensor.tail++];
        max30102_sensor.tail %= 32;
        if (hr_reading < 80000) continue; // not touching skin
        count++;

        hr_reading = bw_band_pass(filter, hr_reading);
        peaks += PeakDetector_Process(detector, hr_reading);

        if (count == HR_WINDOW) {
            g_app_data.biometrics.heart_rate = (peaks/HR_WINDOW) * 1500.0;
            peaks = 0;
            count = 0;
        }
    }
}
