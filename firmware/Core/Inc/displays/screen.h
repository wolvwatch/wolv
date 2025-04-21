#ifndef SCREEN_H
#define SCREEN_H

#include "stdint.h"
#include "stdbool.h"

typedef enum {
  SCREEN_LAUNCHER = 0, // not done
  WATCHFACE_DIGITAL, // done
  WATCHFACE_ANALOG, // done
  SCREEN_WEATHER, // not done
  SCREEN_GAMES, // flappy bird
  SCREEN_TIMER, // done
  SCREEN_MEDCARD, //not done
  SCREEN_METRONOME, // not done
  SCREEN_SYSMON, // done
  SCREEN_FLASHLIGHT, // done
  SCREEN_COUNT // not done
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
