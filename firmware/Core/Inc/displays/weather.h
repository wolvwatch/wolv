#ifndef WEATHER_H
#define WEATHER_H

#include "drivers/apps.h"

void weather_init(void);
void weather_update(void);
void weather_draw(void);
void weather_input(button_t button);

extern const app_t weather_app;

#endif //WEATHER_H 