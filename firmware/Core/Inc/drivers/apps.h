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
    //APP_MEDCARD,
    //APP_METRONOME,
    APP_SYSINFO,
    APP_FLASHLIGHT,
    APP_COUNT
} app_id_t;

typedef enum {
    BTN_UP,
    BTN_DOWN,
    BTN_SEL
} button_t;

typedef struct {
    void (*init)   (void);
    void (*update) (void);
    void (*draw)   (void);
    void (*input)  (button_t btn);
} app_t;

// External declarations for all app instances


void init_apps(void);


#endif //APPS_H
