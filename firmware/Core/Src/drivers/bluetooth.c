#include "drivers/bluetooth.h"
#include "stm32l4xx_hal.h"
#include "displays/screen.h"
#include "displays/data.h"
#include "displays/notification.h"
#include "displays/launcher.h"
#include "drivers/apps.h"
#include "ux/display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern uint8_t rxData;

#define RX_BUFFER_SIZE 256

extern UART_HandleTypeDef huart3;
volatile char     rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex          = 0;

extern app_data_t g_app_data;

volatile uint32_t last_ack_time    = 0;
volatile bool     waiting_for_ack  = false;

// same order for now
static const char* app_names[APP_COUNT] = {
    "Analog Watch",    // APP_WATCHFACE_ANALOG
    "Digital Watch",   // APP_WATCHFACE_DIGITAL
    "App Launcher",    // APP_LAUNCHER
    "Weather",         // APP_WEATHER
    "Flappy Bird",     // APP_GAMES
    "Timer",          // APP_TIMER
    //"Medical Card",    // APP_MEDCARD
    //"Metronome",      // APP_METRONOME
    "System Info",     // APP_SYSINFO
    "Flashlight"      // APP_FLASHLIGHT
};

// app name to app_id
static app_id_t name_to_app_id(const char* name) {
    for (int i = 0; i < APP_COUNT; i++) {
        if (strcmp(name, app_names[i]) == 0) {
            return (app_id_t)i;
        }
    }
    return APP_WATCHFACE_ANALOG; // default analog watch
}

// convert app id to screen type
static screen_t app_id_to_screen(app_id_t app_id) {
    switch (app_id) {
        case APP_WATCHFACE_ANALOG: return WATCHFACE_ANALOG;
        case APP_WATCHFACE_DIGITAL: return WATCHFACE_DIGITAL;
        case APP_WEATHER: return SCREEN_WEATHER;
        case APP_LAUNCHER: return SCREEN_LAUNCHER;
        case APP_GAMES: return SCREEN_GAMES;
        case APP_TIMER: return SCREEN_TIMER;
        case APP_SYSINFO: return SCREEN_SYSMON;
        case APP_FLASHLIGHT: return SCREEN_FLASHLIGHT;
        default: return WATCHFACE_ANALOG;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3) {

        rxBuffer[rxIndex++] = rxData;

        if (rxData == '\n' || rxIndex >= RX_BUFFER_SIZE - 1) {
            rxBuffer[--rxIndex] = '\0';

            printf("Received string: %s\n", rxBuffer);
            parseBluetoothCommand((char *)rxBuffer);
            rxIndex = 0;
        }
    }

    HAL_UART_Receive_IT(huart, &rxData, 1);
}

static void trim(char *str)
{
    size_t len = strlen(str);
    while (len && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

void send_ack_request(void)
{
    const char *ack_cmd = "ACK:CONN\n";

    if (!waiting_for_ack) {
        HAL_UART_Transmit(&huart3, (uint8_t *)ack_cmd, strlen(ack_cmd), 1000);
        waiting_for_ack = true;
        last_ack_time   = HAL_GetTick();
    }
}

void check_ack_timeout(void)
{
    if (waiting_for_ack && (HAL_GetTick() - last_ack_time > 30000)) {
        g_app_data.settings.bluetooth = false;
        waiting_for_ack = false;
    }
}

void parseBluetoothCommand(char *cmd)
{
    char cmdCopy[256];
    strncpy(cmdCopy, cmd, sizeof(cmdCopy) - 1);
    cmdCopy[sizeof(cmdCopy) - 1] = '\0';
    trim(cmdCopy);

    if (strcmp(cmdCopy, "ACK:CONN") == 0) {
        const char *ok = "ACK:OK\n";
        HAL_UART_Transmit(&huart3, (uint8_t *)ok, strlen(ok), 1000);
        g_app_data.settings.bluetooth = true;
        waiting_for_ack = false;
        return;
    }

    if (strcmp(cmdCopy, "ACK:OK") == 0) {
        g_app_data.settings.bluetooth = true;
        waiting_for_ack = false;
        return;
    }

    char *token   = strtok(cmdCopy, ":");
    if (!token) { printf("Error: Empty command.\r\n"); return; }
    char *module  = token;

    token = strtok(NULL, ":");
    if (!token) { printf("Error: No command in %s.\r\n", module); return; }
    char *command = token;

    char *parameter = strtok(NULL, ":");

    if (strcmp(module, "SYS") == 0) {
        if (strcmp(command, "INFO") == 0) {
            const char *info = "STM32L496RTGx";
            HAL_UART_Transmit(&huart3, (uint8_t *)info, strlen(info), 1000);

        } else if (strcmp(command, "BATT") == 0) {
            const char *info = "BATTERY VALUE";
            HAL_UART_Transmit(&huart3, (uint8_t *)info, strlen(info), 1000);

        } else if (strcmp(command, "REBOOT") == 0) {
            HAL_NVIC_SystemReset();
        } else {
            printf("Unknown SYS command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "SCR") == 0) {
        if (strcmp(command, "BRIGHT") == 0) {
            if (parameter) {
                int bright = atoi(parameter);
                g_app_data.settings.brightness = bright;
                set_brightness(bright);
            } else {
                printf("Error: BRIGHT missing parameter\r\n");
            }
        } else if (strcmp(command, "FACE") == 0) {
            if (parameter) {
                int face = atoi(parameter);
                switch (face) {
                    case 0:
                        apps[APP_WATCHFACE_DIGITAL].init();
                        g_app_data.display.active_screen = WATCHFACE_DIGITAL;
                        break;
                    case 1:
                        apps[APP_WATCHFACE_ANALOG].init();
                        g_app_data.display.active_screen = WATCHFACE_ANALOG;
                        break;
                    default:
                        printf("Error: Invalid face value (use 0 for digital, 1 for analog)\r\n");
                        break;
                }
            } else {
                printf("Error: FACE missing parameter (0=digital, 1=analog)\r\n");
            }
        } else if (strcmp(command, "ON") == 0)  { 
            g_app_data.display.on = true; 
        }
        else if (strcmp(command, "OFF") == 0) { 
            g_app_data.display.on = false; 
        }
        else { 
            printf("Unknown SCR command: %s\r\n", command); 
        }
    }
    else if (strcmp(module, "APP") == 0) {
        if (strcmp(command, "LIST") == 0) {
            // the list of available apps
            char appList[512] = "";
            for (int i = 0; i < APP_COUNT; i++) {
                if (i > 0) strcat(appList, ",");
                strcat(appList, app_names[i]);
            }
            strcat(appList, "\n");
            HAL_UART_Transmit(&huart3, (uint8_t *)appList, strlen(appList), 1000);
        }
        else if (strcmp(command, "LAUNCH") == 0) {
            if (!parameter) {
                printf("Error: LAUNCH missing parameter\r\n");
            } else {
                app_id_t app_id = name_to_app_id(parameter);
                screen_t screen = app_id_to_screen(app_id);
                
                // initialize the app
                apps[app_id].init();
                g_app_data.display.active_screen = screen;
                
                char response[64];
                snprintf(response, sizeof(response), "Launched: %s\n", parameter);
                HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), 1000);
            }
        }
        else if (strcmp(command, "CLOSE") == 0) {
            //  launcher
            launcher_init();
            g_app_data.display.active_screen = SCREEN_LAUNCHER;
            
            const char *response = "Returned to launcher\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), 1000);
        }
        else {
            printf("Unknown APP command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "SET") == 0) {
        if (strcmp(command, "TIMEZONE") == 0) {
            if (parameter) {
                (void)atoi(parameter);
            } else printf("Error: TIMEZONE missing parameter\r\n");
        } else if (strcmp(command, "UNITS") == 0) {
            if (!parameter) { printf("Error: UNITS missing parameter\r\n"); }
            else if (strcmp(parameter, "METRIC") == 0)   g_app_data.settings.metric = true;
            else if (strcmp(parameter, "IMPERIAL") == 0) g_app_data.settings.metric = false;
            else printf("Error: Unknown UNITS parameter: %s\r\n", parameter);
        } else if (strcmp(command, "HR_INTERVAL") == 0) {
            if (!parameter) printf("Error: HR_INTERVAL missing parameter\r\n");
        } else { printf("Unknown SET command: %s\r\n", command); }
    }
    else if (strcmp(module, "SENS") == 0) {
        /* send sensor snapshots over UART */
    }
    else if (strcmp(module, "TIME") == 0) {
        if (strcmp(command, "GET") == 0) {
            char timeStr[16];
            snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
                     g_app_data.timeVal.hour,
                     g_app_data.timeVal.minute,
                     g_app_data.timeVal.second);
            HAL_UART_Transmit(&huart3, (uint8_t *)timeStr, strlen(timeStr), 1000);
        } else if (strcmp(command, "SET") == 0 && parameter) {
            int h, m, s;
            if (sscanf(parameter, "%d:%d:%d", &h, &m, &s) == 3) {
                g_app_data.timeVal.hour = h;
                g_app_data.timeVal.minute = m;
                g_app_data.timeVal.second = s;
            } else printf("Error: Invalid time format\r\n");
        } else if (strcmp(command, "DATE") == 0 && parameter) {
            int y, mon, d;
            if (sscanf(parameter, "%d-%d-%d", &y, &mon, &d) == 3) {
                g_app_data.timeVal.year  = y;
                g_app_data.timeVal.month = mon;
                g_app_data.timeVal.day   = d;
            } else printf("Error: Invalid date format\r\n");
        } else { printf("Unknown TIME command: %s\r\n", command); }
    }
    else if (strcmp(module, "NOTIF") == 0) {
        if (strcmp(command, "TEXT") == 0 && parameter) {
            display_notification(parameter);
        } else if (strcmp(command, "CALL") == 0 && parameter) {
            char msg[64] = "Call from: ";
            strncat(msg, parameter, sizeof(msg) - strlen(msg) - 1);
            display_notification(msg);
        } else { printf("Unknown NOTIF command or missing param\r\n"); }
    }
    else if (strcmp(module, "WEATHER") == 0) {
        if (parameter) {
            // skip past WEATHER:
            if (strncmp(parameter, "WEATHER:", 8) == 0) {
                parameter += 8;
            }
            
            char* json_start = strchr(parameter, '{');
            if (!json_start) {
                printf("Error: Invalid weather data format (no JSON object)\r\n");
                return;
            }

            // temp
            char* temp = strstr(json_start, "temp_current");
            if (temp) {
                temp = strchr(temp, ':');
                if (temp) g_app_data.weather.temp_current = atof(temp + 1);
            }
            
            // high temperature
            temp = strstr(json_start, "temp_high");
            if (temp) {
                temp = strchr(temp, ':');
                if (temp) g_app_data.weather.temp_high = atof(temp + 1);
            }
            // low temperature
            temp = strstr(json_start, "temp_low");
            if (temp) {
                temp = strchr(temp, ':');
                if (temp) g_app_data.weather.temp_low = atof(temp + 1);
            }
            // humidity
            char* hum = strstr(json_start, "humidity");
            if (hum) {
                hum = strchr(hum, ':');
                if (hum) g_app_data.weather.humidity = (uint8_t)atoi(hum + 1);
            }
            // condition
            char* cond = strstr(json_start, "condition");
            if (cond) {
                cond = strchr(cond, ':');
                if (cond) g_app_data.weather.condition = (uint8_t)atoi(cond + 1);
            }
            // location (in quotes)
            char* loc = strstr(json_start, "location");
            if (loc) {
                loc = strchr(loc, ':');
                if (loc) {
                    loc = strchr(loc, '"');
                    if (loc) {
                        loc++;
                        char* end_quote = strchr(loc, '"');
                        if (end_quote) {
                            size_t len = end_quote - loc;
                            if (len > sizeof(g_app_data.weather.location) - 1)
                                len = sizeof(g_app_data.weather.location) - 1;
                            strncpy(g_app_data.weather.location, loc, len);
                            g_app_data.weather.location[len] = '\0';
                        }
                    }
                }
            }
            // desc
            char* desc = strstr(json_start, "description");
            if (desc) {
                desc = strchr(desc, ':');
                if (desc) {
                    desc = strchr(desc, '"');
                    if (desc) {
                        desc++;
                        char* end_quote = strchr(desc, '"');
                        if (end_quote) {
                            size_t len = end_quote - desc;
                            if (len > sizeof(g_app_data.weather.description) - 1)
                                len = sizeof(g_app_data.weather.description) - 1;
                            strncpy(g_app_data.weather.description, desc, len);
                            g_app_data.weather.description[len] = '\0';
                        }
                    }
                }
            }
            if (g_app_data.display.active_screen == SCREEN_WEATHER) {
                apps[APP_WEATHER].init();
            }
            const char *response = "Weather updated\n";
            HAL_UART_Transmit(&huart3, (uint8_t *)response, strlen(response), 1000);
        } else {
            printf("Error: Missing weather data\r\n");
        }
    }
    else {
        printf("Unknown module: %s\r\n", module);
    }
}

void send_sensor_data(void)
{
    char data_str[128];
    snprintf(data_str, sizeof(data_str),
             "DATA:HR=%.1f,SPO2=%.1f,STEPS=%d\n",
             g_app_data.biometrics.heart_rate,
             g_app_data.biometrics.spo2,
             g_app_data.biometrics.steps);

    HAL_UART_Transmit(&huart3, (uint8_t *)data_str, strlen(data_str), 1000);
}