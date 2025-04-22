//
// Created by Ryan Kaelle on 4/21/25.
//

#ifndef SYSMON_H
#define SYSMON_H


#include "drivers/apps.h"

void sysmon_init(void);

void sysmon_update(void);

void sysmon_draw(void);

void sysmon_input(button_t button);

#endif //SYSMON_H
