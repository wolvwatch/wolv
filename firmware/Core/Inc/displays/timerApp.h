#ifndef TIMER_H
#define TIMER_H

#include "ux/display.h"
#include "drivers/apps.h"


void timer_draw(void);
void timer_update(void);
void timer_init(void);
void timer_input(button_t button);

#endif //TIMER_H
