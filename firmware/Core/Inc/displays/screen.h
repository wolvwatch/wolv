#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"
#include "stdbool.h"

typedef enum {
  WATCHFACE_DIGITAL, WATCHFACE_ANALOG
} screen_t;

typedef struct {
  screen_t active_screen;
  uint8_t brightness;
  bool on;
  bool metric;
  bool show_heart;
  bool show_steps;
  bool show_spo2;
} display_t;

#endif //SCREEN_H
