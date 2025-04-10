/*
* bluetooth.c
 *
 *  Created on: Mar 22, 2025
 *      Author: ryankaelle
 */
#include "drivers/bluetooth.h"
#include "stm32l4xx_hal.h"
#include "displays/screen.h"
#include "displays/notification.h"
#include "ux/display.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint8_t rxData;

#define RX_BUFFER_SIZE 256
volatile char rxBuffer[RX_BUFFER_SIZE];
volatile uint16_t rxIndex = 0;
extern display_t disp;


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART3) {
        rxBuffer[rxIndex++] = rxData;
        if (rxData == '\n' || rxIndex >= RX_BUFFER_SIZE - 1) {
            rxBuffer[rxIndex] = '\0';
            printf("Received string: %s", rxBuffer);
            parseBluetoothCommand((char *)rxBuffer);
            rxIndex = 0;
        }

        HAL_UART_Receive_IT(huart, &rxData, 1);
    }
}
void trim(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r')) {
        str[len-1] = '\0';
        len--;
    }
}

void parseBluetoothCommand(char *cmd) {
    char cmdCopy[256];
    strncpy(cmdCopy, cmd, sizeof(cmdCopy) - 1);
    cmdCopy[sizeof(cmdCopy)-1] = '\0';

    trim(cmdCopy);

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
                disp.brightness = bright;
            } else {
                printf("Error: BRIGHT command missing parameter\r\n");
            }
        } else if (strcmp(command, "FACE") == 0) {
            if (parameter) {
                int active = atoi(parameter);
                disp.active_screen = active;
            } else {
                printf("Error: FACE command missing parameter\r\n");
            }
        } else if (strcmp(command, "ON") == 0) {
            disp.on = true;
        } else if (strcmp(command, "OFF") == 0) {
            disp.on = false;
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
                    disp.metric = true;
                } else if (strcmp(parameter, "IMPERIAL") == 0) {
                    disp.metric = false;
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
            // send current time back over UART
        } else if (strcmp(command, "SET") == 0) {
            if (parameter) {
                int hour, minute, second;
                if (sscanf(parameter, "%d:%d:%d", &hour, &minute, &second) == 3) {
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