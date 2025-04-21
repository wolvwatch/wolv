// // displays/launcher.c
//
// #include "launcher.h"
// #include "ux/display.h"
// #include "ux/rasterizer.h"
// #include "drivers/lcd.h"
// #include "displays/data.h"
//
// #include "displays/analog.h"
// #include "displays/digital.h"
// #include "displays/sysmon.h"
// #include "displays/flappy.h"
// #include "displays/splashscreen.h"  // if you have medical card or timer here use their headers
// #include "displays/notification.h"  // placeholder for “medical card”
// #include "displays/battery.h"       // repurpose for flashlight or metronome
// #include "displays/metronome.h"     // your metronome header
// #include "displays/timer.h"         // your pomodoro timer header
// #include "displays/medical.h"       // your medical card header
//
// // Simple array of all apps in menu order.  Must match the data.h enum.
// static const struct {
//     const char *name;
//     const app_t *app;
// } s_apps[SCREEN_COUNT] = {
//     [SCREEN_LAUNCHER]  = {"Launcher",   &launcher_app},  // not actually used
//     [SCREEN_DIGITAL]   = {"Digital",    &digital_app},
//     [SCREEN_ANALOG]    = {"Analog",     &analog_app},
//     [SCREEN_WEATHER]   = {"Weather",    &weather_app},
//     [SCREEN_GAMES]     = {"Games",      &flappy_app},
//     [SCREEN_TIMER]     = {"Timer",      &timer_app},
//     [SCREEN_MEDCARD]   = {"Med Card",   &medical_app},
//     [SCREEN_METRONOME] = {"Metronome",  &metronome_app},
//     [SCREEN_SYSMON]    = {"SysMon",     &sysmon_app},
//     [SCREEN_FLASHLIGHT]= {"Flashlight", &flash_app},
// };
//
// static uint8_t s_sel;  // index of currently highlighted app
//
// void launcher_init(void) {
//     s_sel = 0;
// }
//
// void launcher_update(void) {
//     // nothing to animate here
// }
//
// void launcher_draw(void) {
//     screen_clear();
//
//     // Title
//     draw_text("Select App",
//               CENTER_X,  15,
//               &montserrat_reg, COLOR_WHITE, 0.8f, true);
//
//     // List each app name; highlight the selected one
//     for (int i = 0; i < SCREEN_COUNT; i++) {
//         uint8_t y = 40 + i * 18;
//         bool  sel = (i == s_sel);
//         if (sel) {
//             // draw a little bar behind it
//             draw_rectangle(10, y - 2, 220, 16, COLOR_WHITE);
//             draw_text(s_apps[i].name,
//                       CENTER_X, y + 6,
//                       &montserrat_reg, 0b000, 0.6f, true);
//         } else {
//             draw_text(s_apps[i].name,
//                       CENTER_X, y + 6,
//                       &montserrat_reg, COLOR_WHITE, 0.6f, true);
//         }
//     }
//
//     screen_render();
// }
//
// void launcher_input(button_t b) {
//     switch (b) {
//         case BTN_UP:
//             s_sel = (s_sel + SCREEN_COUNT - 1) % SCREEN_COUNT;
//             break;
//         case BTN_DOWN:
//             s_sel = (s_sel + 1) % SCREEN_COUNT;
//             break;
//         case BTN_SEL:
//             // switch to chosen app
//             g_app_data.display.active_screen = (screen_t)s_sel;
//             // call its init so it can reset state
//             s_apps[s_sel].app->init();
//             break;
//         default:
//             break;
//     }
// }
//
// const app_t launcher_app = {
//     .init   = launcher_init,
//     .update = launcher_update,
//     .draw   = launcher_draw,
//     .input  = launcher_input
// };