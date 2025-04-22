#ifndef WEATHER_H
#define WEATHER_H

#include "drivers/apps.h"

void weather_init(void);
void weather_update(void);
void weather_draw(void);
void weather_input(button_t button);

#endif //WEATHER_H 