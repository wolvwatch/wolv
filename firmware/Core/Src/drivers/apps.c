#include "drivers/apps.h"
#include "displays/analog.h"
#include "displays/digital.h"
#include "displays/launcher.h"
#include "displays/weather.h"
#include "displays/flappy.h"
#include "displays/timerApp.h"
//#include "displays/medcard.h"
//#include "displays/metronome.h"
#include "displays/sysmon.h"
#include "displays/flashlight.h"

app_t apps[APP_COUNT];

void init_apps(void) {
    apps[APP_WATCHFACE_ANALOG] = analog_app;
    apps[APP_WATCHFACE_DIGITAL] = digital_app;
    apps[APP_LAUNCHER] = launcher_app;
    apps[APP_WEATHER] = weather_app,
    apps[APP_GAMES] = flappy_app;
    apps[APP_TIMER] = timer_app;
    //[APP_MEDCARD] = medcard_app,
    //[APP_METRONOME] = metronome_app,
    apps[APP_SYSINFO] = sysmon_app;
    apps[APP_FLASHLIGHT] = flash_app;
} 