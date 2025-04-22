//
// Created by Ryan Kaelle on 4/21/25.
//

#ifndef FLASHLIGHT_H
#define FLASHLIGHT_H
#include "drivers/apps.h"

void flash_init(void);
void flash_update(void);
void flash_draw(void);
void flash_input(button_t button);

#endif //FLASHLIGHT_H
