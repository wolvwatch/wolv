#ifndef DIGITAL_H
#define DIGITAL_H

#include "drivers/apps.h"


void watchface_digital_init(void);
void watchface_digital_update(void);
void watchface_digital_draw(void);
void watchface_digital_input(button_t button);

extern const app_t digital_app;

#endif //DIGITAL_H
