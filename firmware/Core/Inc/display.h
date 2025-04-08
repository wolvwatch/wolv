#ifndef DISPLAY_H
#define DISPLAY_H

#include "lcd.h"         // For screen_init(), screen_set_pixel(), etc.
#include "rasterizer.h"  // For the definition of font_t
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
void display_main_text(uint16_t x, uint16_t y, const char *text, uint16_t color);

void display_time_text(uint16_t x, uint16_t y, const char *time_str, uint16_t color);

void display_bluetooth_icon(uint16_t x, uint16_t y, uint16_t color);

void display_heartrate_icon(uint16_t x, uint16_t y, uint16_t color);


void display_steps_text(uint16_t x, uint16_t y, const char *steps_str, uint16_t color);

#ifdef __cplusplus
}
#endif

#endif // DISPLAY_H