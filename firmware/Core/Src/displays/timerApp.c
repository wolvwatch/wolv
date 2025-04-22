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

#include "displays/timerApp.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "displays/data.h"
#include "displays/launcher.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

extern app_data_t g_app_data;

typedef enum {
    TMR_SETTING,
    TMR_RUNNING,
    TMR_PAUSED,
    TMR_FINISHED
} timer_state_t;

static timer_state_t state = TMR_SETTING;
static uint8_t sel_min = 0;
static uint8_t sel_sec = 0;
static uint32_t duration_ms = 0;
static uint32_t remaining_ms = 0;
static uint32_t last_tick = 0;

void timer_init(void) {
    state = TMR_SETTING;
    sel_min = 0;
    sel_sec = 0;
    duration_ms = 0;
    remaining_ms = 0;
}

void timer_update(void) {
    if (state == TMR_RUNNING) {
        uint32_t now = HAL_GetTick();
        uint32_t elapsed = now - last_tick;
        last_tick = now;

        if (elapsed < remaining_ms) {
            remaining_ms -= elapsed;
        } else {
            remaining_ms = 0;
            state = TMR_FINISHED;
        }
    }
}

void timer_draw(void) {
    char buf[32];
    uint32_t mins, secs;

    if (state == TMR_SETTING) {
        mins = sel_min;
        secs = sel_sec;
    } else {
        mins = remaining_ms / 60000;
        secs = (remaining_ms % 60000) / 1000;
    }

    snprintf(buf, sizeof(buf), "%02lu:%02lu", mins, secs);
    draw_text(buf, CENTER_X, CENTER_Y, &montserrat_reg, COLOR_WHITE, 1.5f, true);

    switch (state) {
        case TMR_SETTING:
            draw_text("Set Timer", CENTER_X, CENTER_Y - 40, &montserrat_reg, COLOR_WHITE, 0.7f, true);
            draw_text("UP/DOWN: Adjust", CENTER_X, CENTER_Y + 40, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            draw_text("SEL: Start", CENTER_X, CENTER_Y + 60, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            break;

        case TMR_RUNNING:
            draw_text("Running", CENTER_X, CENTER_Y - 40, &montserrat_reg, COLOR_WHITE, 0.7f, true);
            draw_text("SEL: Pause", CENTER_X, CENTER_Y + 40, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            break;

        case TMR_PAUSED:
            draw_text("Paused", CENTER_X, CENTER_Y - 40, &montserrat_reg, COLOR_WHITE, 0.7f, true);
            draw_text("SEL: Resume", CENTER_X, CENTER_Y + 40, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            draw_text("UP: Reset", CENTER_X, CENTER_Y + 60, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            break;

        case TMR_FINISHED:
            draw_text("Time's Up!", CENTER_X, CENTER_Y - 40, &montserrat_reg, COLOR_WHITE, 0.7f, true);
            draw_text("Press any button", CENTER_X, CENTER_Y + 40, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            draw_text("to reset", CENTER_X, CENTER_Y + 60, &montserrat_reg, COLOR_WHITE, 0.5f, true);
            break;
    }
}

void timer_input(button_t btn) {
    switch (state) {
        case TMR_SETTING:
            if (btn == BTN_UP) {
                if (sel_sec < 59) sel_sec++;
                else {
                    sel_sec = 0;
                    if (sel_min < 99) sel_min++;
                }
            } else if (btn == BTN_DOWN) {
                if (sel_sec == 0 && sel_min == 0) {
                    g_app_data.display.active_screen = SCREEN_LAUNCHER;
                    launcher_init();
                }
                if (sel_sec > 0) sel_sec--;
                else if (sel_min > 0) {
                    sel_min--;
                    sel_sec = 59;
                }
            } else if (btn == BTN_SEL) {
                duration_ms = (uint32_t) sel_min * 60000u + (uint32_t) sel_sec * 1000u;
                remaining_ms = duration_ms;
                last_tick = HAL_GetTick();
                if (duration_ms > 0) state = TMR_RUNNING;
            }
            break;

        case TMR_RUNNING:
            if (btn == BTN_SEL) {
                state = TMR_PAUSED;
            }
            break;

        case TMR_PAUSED:
            if (btn == BTN_SEL) {
                state = TMR_RUNNING;
                last_tick = HAL_GetTick();
            } else if (btn == BTN_UP) {
                timer_init();
            }
            break;

        case TMR_FINISHED:
            timer_init();
            break;
    }
}

const app_t timer_app = {
    .init = timer_init,
    .update = timer_update,
    .draw = timer_draw,
    .input = timer_input
};
