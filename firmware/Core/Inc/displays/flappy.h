//
// Created by Ryan Kaelle on 4/21/25.
//

#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "drivers/apps.h"

void flappy_init(void);
void flappy_update(void);
void flappy_draw(void);
//void flappy_input(button_t btn);

extern const app_t flappy_app;

#endif //FLAPPYBIRD_H
