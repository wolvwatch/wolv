/*
The MIT License (MIT)

Copyright (c) 2025 Sandro Petrovski, Austin Sierco, Ryan Kaelle, and Tenzin Sherab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "sense/biometrics.h"
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
            g_app_data.biometrics.heart_rate = (peaks / HR_WINDOW) * 1500.0;
            peaks = 0;
            count = 0;
        }
    }
}
