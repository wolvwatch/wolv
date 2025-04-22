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
extern const app_t analog_app;
extern const app_t digital_app;
extern const app_t launcher_app;
extern const app_t weather_app;
extern const app_t flappy_app;
extern const app_t timer_app;
//extern const app_t medcard_app;
//extern const app_t metronome_app;
extern const app_t sysmon_app;
extern const app_t flash_app;

extern app_t apps[APP_COUNT];

void init_apps(void);

static app_id_t current = APP_WATCHFACE_ANALOG;

#endif //APPS_H
