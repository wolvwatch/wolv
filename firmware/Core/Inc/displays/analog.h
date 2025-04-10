#ifndef ANALOG_H
#define ANALOG_H

#include <stdint.h>

/**
 * @brief Initialize the analog watch face.
 */
void analog_init(void);

/**
 * @brief Update the analog watch face given the current time.
 *
 * @param hours   The current hour (0-23).
 * @param minutes The current minute (0-59).
 * @param seconds The current second (0-59).
 */
void analog_update(uint8_t hours, uint8_t minutes, uint8_t seconds);

void draw_watch_face(void);
void draw_hour_markers(void);
void draw_watch_hands(uint8_t hours, uint8_t minutes, uint8_t seconds);
void draw_biometric_data(uint8_t heart_rate, uint8_t spo2);
void draw_center_dot(void);

#endif /* ANALOG_H */