/*
* bluetooth.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */
#include "drivers/bluetooth.h"
#include "stm32l4xx_hal.h"
#include "displays/screen.h"
#include "displays/data.h"
#include "displays/notification.h"
#include "ux/display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t rxData;

#define RX_BUFFER_SIZE 256

extern UART_HandleTypeDef huart3;
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;
extern app_data_t g_app_data;
volatile uint32_t last_ack_time = 0;
volatile bool waiting_for_ack = false;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        rxBuffer[rxIndex++] = rxData;
        if (rxData == '\n' || rxIndex >= RX_BUFFER_SIZE - 1) {
            rxBuffer[rxIndex] = '\0';
            printf("Received string: %s", rxBuffer);
            parseBluetoothCommand((char *)rxBuffer);
            rxIndex = 0;
        }
    }

    HAL_UART_Receive_IT(huart, &rxData, 1);
}
void trim(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = '\0';
        len--;
    }
}

void send_ack_request(void) {
    const char* ack_cmd = "ACK:CONN\n";
    if (!waiting_for_ack) {
        HAL_UART_Transmit(&huart3, (uint8_t*)ack_cmd, strlen(ack_cmd), 1000);
        waiting_for_ack = true;
        last_ack_time = HAL_GetTick();
    }
}

void check_ack_timeout(void) {
    if (waiting_for_ack && (HAL_GetTick() - last_ack_time > 30000)) {
        g_app_data.settings.bluetooth = false;
        waiting_for_ack = false;
    }
}

void parseBluetoothCommand(char *cmd) {
    char cmdCopy[256];
    strncpy(cmdCopy, cmd, sizeof(cmdCopy) - 1);
    cmdCopy[sizeof(cmdCopy)-1] = '\0';

    trim(cmdCopy);


    if (strcmp(cmdCopy, "ACK:OK\n") == 0) {
        if (waiting_for_ack) {
            g_app_data.settings.bluetooth = true;
            waiting_for_ack = false;
        }
        return;
    }

    char *token = strtok(cmdCopy, ":");
    if (!token) {
        printf("Error: Command is empty or malformed.\r\n");
        return;
    }

    char *module = token;

    token = strtok(NULL, ":");
    if (!token) {
        printf("Error: No command specified in %s.\r\n", module);
        return;
    }
    char *command = token;

    char *parameter = strtok(NULL, ":");

    if (strcmp(module, "SYS") == 0) {
        if (strcmp(command, "INFO") == 0) {
            const char* info = "STM32L476RG";
            HAL_UART_Transmit(&huart3, (uint8_t*)info, strlen(info),1000);

        } else if (strcmp(command, "BATT") == 0) {
            const char* info = "BATTERY VALUE";
            HAL_UART_Transmit(&huart3, (uint8_t*)info, strlen(info),1000);

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
            } else {
                printf("Error: BRIGHT command missing parameter\r\n");
            }
        } else if (strcmp(command, "FACE") == 0) {
            if (parameter) {
                int active = atoi(parameter);
                g_app_data.display.active_screen = active;
            } else {
                printf("Error: FACE command missing parameter\r\n");
            }
        } else if (strcmp(command, "ON") == 0) {
            g_app_data.display.active_screen = 1;
        } else if (strcmp(command, "OFF") == 0) {
            g_app_data.display.active_screen = 0;
        } else {
            printf("Unknown SCR command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "APP") == 0) {
        if (strcmp(command, "LIST") == 0) {
            // send app list back
        } else if (strcmp(command, "LAUNCH") == 0) {
            // Here, parameter might be used to identify the app to launch
            if (parameter) {
                // Process launch command with parameter value
            } else {
                printf("Error: LAUNCH command missing parameter\r\n");
            }
        } else if (strcmp(command, "CLOSE") == 0) {
            // close current app
        } else {
            printf("Unknown APP command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "SET") == 0) {
        if (strcmp(command, "TIMEZONE") == 0) {
            if (parameter) {
                int timezone = atoi(parameter);
                // rtc set timezone accordingly
            } else {
                printf("Error: TIMEZONE command missing parameter\r\n");
            }
        } else if (strcmp(command, "UNITS") == 0) {
            if (parameter) {
                if (strcmp(parameter, "METRIC") == 0) {
                    g_app_data.settings.metric = true;
                } else if (strcmp(parameter, "IMPERIAL") == 0) {
                    g_app_data.settings.metric = true;
                } else {
                    printf("Error: Unknown UNITS parameter: %s\r\n", parameter);
                }
            } else {
                printf("Error: UNITS command missing parameter\r\n");
            }
        } else if (strcmp(command, "HR_INTERVAL") == 0) {
            if (parameter) {
                int interval = atoi(parameter);
                // set heart rate polling interval
            } else {
                printf("Error: HR_INTERVAL command missing parameter\r\n");
            }
        } else {
            printf("Unknown SET command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "SENS") == 0) {
        if (strcmp(command, "HR") == 0) {
            // send heart rate data back over UART
        } else if (strcmp(command, "SPO2") == 0) {
            // send SpO2 data back over UART
        } else if (strcmp(command, "ACCEL") == 0) {
            // send accelerometer data back over UART
        } else {
            printf("Unknown SENS command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "TIME") == 0) {
        if (strcmp(command, "GET") == 0) {
            char timeStr[16];
            char dateStr[16];

            snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d",
                     g_app_data.timeVal.hour,
                     g_app_data.timeVal.minute,
                     g_app_data.timeVal.second);

            snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
                     g_app_data.timeVal.day,
                     g_app_data.timeVal.month,
                     g_app_data.timeVal.year);
            HAL_UART_Transmit(&huart3, (uint8_t*)timeStr, strlen(timeStr),1000);
            // send current time back over UART
        } else if (strcmp(command, "SET") == 0) {
            if (parameter) {
                int hour, minute, second;
                if (sscanf(parameter, "%d:%d:%d", &hour, &minute, &second) == 3) {
                    g_app_data.timeVal.hour = hour;
                    g_app_data.timeVal.minute = minute;
                    g_app_data.timeVal.second = second;
                    // set time on watch
                } else {
                    printf("Error: Invalid time format\r\n");
                }
            } else {
                printf("Error: TIME SET command missing parameter\r\n");
            }
        } else if (strcmp(command, "DATE") == 0) {
            if (parameter) {
                int year, month, day;
                if (sscanf(parameter, "%d-%d-%d", &year, &month, &day) == 3) {
                     g_app_data.timeVal.day = day;
                     g_app_data.timeVal.month = month;
                     g_app_data.timeVal.year = year;
                    // set date on watch
                } else {
                    // Handle parsing error
                    printf("Error: Invalid date format.\n");
                }
            } else {
                printf("Error: DATE command missing parameter\r\n");
            }
        } else {
            printf("Unknown TIME command: %s\r\n", command);
        }
    }
    else if (strcmp(module, "NOTIF") == 0) {
        if (strcmp(command, "TEXT") == 0) {
            if (parameter) {
                display_notification(parameter);
            } else {
                printf("Error: TEXT command missing parameter\r\n");
            }
        } else if (strcmp(command, "CALL") == 0) {
            if (parameter) {
                char msg[50] = "Call from: ";
                strcat(msg, parameter);
                display_notification(msg);
            } else {
                printf("Error: CALL command missing parameter\r\n");
            }
        } else {
            printf("Unknown NOTIF command: %s\r\n", command);
        }
    } else {
        printf("Unknown module: %s\r\n", module);
    }
}

void send_sensor_data() {
    char data_str[128];
    extern app_data_t g_app_data;
    
    // Format: "DATA:HR={heart_rate},SPO2={spo2},STEPS={steps}\n"
    snprintf(data_str, sizeof(data_str), "DATA:HR=%.1f,SPO2=%.1f,STEPS=%d\n", 
             g_app_data.biometrics.heart_rate,
             g_app_data.biometrics.spo2,
             g_app_data.biometrics.steps);
             
    HAL_UART_Transmit(&huart3, (uint8_t*)data_str, strlen(data_str), 1000);
}