#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "drivers/apps.h"

void launcher_init(void);
void launcher_update(void);
void launcher_draw(void);
void launcher_input(button_t btn);

extern const app_t launcher_app;

#endif // LAUNCHER_H 