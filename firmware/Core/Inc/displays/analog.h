#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

/**
 * @brief Initialize the analog watch face.
 */
void analog_init(void);


void analog_update(uint8_t hours, uint8_t minutes, uint8_t seconds);

void draw_watch_face(void);
void draw_hour_markers(void);
void draw_watch_hands(uint8_t hours, uint8_t minutes, uint8_t seconds);
void draw_biometric_data(void);
void draw_center_dot(void);
void watchface_analog_draw(void);

#endif /* ANALOG_H */