//
// Created by Ryan Kaelle on 4/21/25.
//

#ifndef APPS_H
#define APPS_H

typedef enum {
    APP_WATCHFACE_ANALOG,
    APP_WATCHFACE_DIGITAL,
    APP_LAUNCHER,          // the apps screen
    APP_WEATHER,
    APP_GAMES,
    APP_TIMER,
    APP_MEDCARD,
    APP_METRONOME,
    APP_SYSINFO,
    APP_FLASHLIGHT,
    APP_COUNT
} app_id_t;

typedef struct {
    void (*init)   (void);            // allocate buffers, reset state
    void (*update) (void);            // advance timers, poll sensors
    void (*draw)   (void);            // push UI to the frame‑buffer
    // void (*input)(std::string btn);    // Up / Select / Down
} app_t;

extern const app_t apps[APP_COUNT];   // built in per‑app .c modules

static app_id_t current = APP_WATCHFACE_ANALOG;


#endif //APPS_H
