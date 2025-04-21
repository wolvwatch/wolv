#include "displays/timer.h"
#include "ux/display.h"
#include "drivers/lcd.h"
#include "displays/data.h"
#include "ux/rasterizer.h"
#include "stm32l4xx_hal.h"
#include <stdio.h>

// Timer states
typedef enum {
  TMR_SETTING,
  TMR_RUNNING,
  TMR_PAUSED,
  TMR_FINISHED
} tmr_state_t;

static tmr_state_t state;
static uint8_t sel_min, sel_sec; // need to implement
static uint32_t duration_ms;
static uint32_t remaining_ms;
static uint32_t last_tick;


void timer_init(void) {
  state       = TMR_SETTING;
  sel_min     = 0;
  sel_sec     = 30;
  duration_ms = 0;
  remaining_ms= 0;
  last_tick   = HAL_GetTick();
}

void timer_update(void) {
  uint32_t now = HAL_GetTick();
  uint32_t dt  = now - last_tick;
  last_tick    = now;

  if (state == TMR_RUNNING) {
    if (dt >= remaining_ms) {
      remaining_ms = 0;
      state = TMR_FINISHED;
      // todo buzzer trigger
    } else {
      remaining_ms -= dt;
    }
  }
}

void timer_draw(void) {

  char buf[16];
  if (state == TMR_SETTING) {
    snprintf(buf, sizeof(buf), "%02u:%02u", sel_min, sel_sec);
    draw_text(buf, CENTER_X, CENTER_Y, &montserrat_reg, COLOR_WHITE, 1.2f, true);
    draw_text("UP/DOWN to set", CENTER_X, 210, &montserrat_reg, COLOR_WHITE, 0.4f, true);
    draw_text("SEL to start",  CENTER_X, 230, &montserrat_reg, COLOR_WHITE, 0.4f, true);
  }
  else {
    uint32_t secs = (remaining_ms + 500) / 1000;
    uint32_t m = secs/60, s = secs%60;
    snprintf(buf, sizeof(buf), "%02u:%02u", m, s);
    draw_text(buf, CENTER_X, CENTER_Y, &ultra, COLOR_WHITE, 1.2f, true);

    if (state == TMR_PAUSED) {
      draw_text("PAUSED", CENTER_X, 210, &montserrat_reg, COLOR_WHITE, 0.5f, true);
    } else if (state == TMR_FINISHED) {
      draw_text("DONE!",  CENTER_X, 210, &montserrat_reg, COLOR_WHITE, 0.5f, true);
    } else {
      draw_text("SEL to pause", CENTER_X, 210, &montserrat_reg, COLOR_WHITE, 0.4f, true);
    }
  }

  screen_render();
}
//
//static void timer_input(button_t b) {
//  switch (state) {
//    case TMR_SETTING:
//      if (b == BTN_UP) {
//        if (sel_sec < 59) sel_sec++;
//        else { sel_sec = 0; if (sel_min < 99) sel_min++; }
//      }
//      else if (b == BTN_DOWN) {
//        if (sel_sec > 0) sel_sec--;
//        else { sel_sec = 59; if (sel_min > 0) sel_min--; }
//      }
//      else if (b == BTN_SEL) {
//        duration_ms  = (uint32_t)sel_min * 60000u + (uint32_t)sel_sec * 1000u;
//        remaining_ms = duration_ms;
//        last_tick    = HAL_GetTick();
//        if (duration_ms > 0) state = TMR_RUNNING;
//      }
//      break;
//
//    case TMR_RUNNING:
//      if (b == BTN_SEL) {
//        state = TMR_PAUSED;
//      }
//      break;
//
//    case TMR_PAUSED:
//      if (b == BTN_SEL) {
//        // resume
//        last_tick = HAL_GetTick();
//        state     = TMR_RUNNING;
//      }
//      else if (b == BTN_DOWN) {
//        // reset to setting
//        state = TMR_SETTING;
//      }
//      break;
//
//    case TMR_FINISHED:
//      state = TMR_SETTING;
//      break;
//  }
//}
//
//const app_t timer_app = {
//  .init   = timer_init,
//  .update = timer_update,
//  .draw   = timer_draw,
//  .input  = timer_input
//};