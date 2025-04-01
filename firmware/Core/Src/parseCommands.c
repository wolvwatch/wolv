/*
 * Commands.c
 *
 *  Created on: Mar 31, 2025
 *      Author: ryankaelle
 */


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "lvgl.h"
#include "gui.h"
#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern gui_data_t gui_data;

static void setTimeFromString(const char* timestr)
{
    // format: "MM/DD/YYYY,HH:MM:SS"

    int month, day, year;
    int hours, minutes, seconds;


    char datePart[11];   // "MM/DD/YYYY" => 10 chars + null
    char timePart[9];    // "HH:MM:SS"   => 8 chars + null

    memset(datePart, 0, sizeof(datePart));
    memset(timePart, 0, sizeof(timePart));

    const char *commaPos = strchr(timestr, ',');
    if (!commaPos) {
        printf("Invalid time format (no comma): %s\n", timestr);
        return;
    }

    // left side (MM/DD/YYYY)
    int dateLen = commaPos - timestr;
    if (dateLen >= sizeof(datePart)) {
        printf("Date part too long.\n");
        return;
    }
    strncpy(datePart, timestr, dateLen);
    datePart[dateLen] = '\0';

    // right side (HH:MM:SS)
    strncpy(timePart, commaPos + 1, sizeof(timePart) - 1);
    timePart[sizeof(timePart) - 1] = '\0';

    if (3 != sscanf(datePart, "%d/%d/%d", &month, &day, &year)) {
        printf("Invalid date format: %s\n", datePart);
        return;
    }
    if (3 != sscanf(timePart, "%d:%d:%d", &hours, &minutes, &seconds)) {
        printf("Invalid time format: %s\n", timePart);
        return;
    }

    // not sure if this is right
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    sTime.Hours = hours;
    sTime.Minutes = minutes;
    sTime.Seconds = seconds;
    sTime.SubSeconds = 0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        printf("Failed to set time.\n");
    }


    sDate.Year = (year >= 2000) ? (year - 2000) : (year - 1900); // depends how your RTC is configured
    sDate.Month = month;
    sDate.Date = day;
    sDate.WeekDay = 0;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK) {
        printf("Failed to set date.\n");
    }
}

void parseBluetoothCommand(const char *cmd)
{
    //
    // CMD:1 Toggle heart rate display
    // CMD:2:#RRGGBB Change background color
    // CMD:3 Toggle notifications (your code's meaning)
    // CMD:4:UP or CMD:4:DOWN Adjust brightness
    // CMD:5 -> Toggle clock display
    // CMD:6:MM/DD/YYYY,HH:MM:SS Set time

    // CMD:1 Toggle Heart Rate Display
    if (strncmp(cmd, "CMD:1", 5) == 0) {
        static bool hrVisible = false;
        hrVisible = !hrVisible;
        if (hrVisible) {
            lv_subject_set_int(&gui_data.hr, 1); // "1" meaning "HR on"
        } else {
            lv_subject_set_int(&gui_data.hr, 0); // "0" meaning "HR off"
        }
    }

    // 2) CMD:2:<hexColor>
    else if (strncmp(cmd, "CMD:2:", 6) == 0) {
        const char *colorStr = cmd + 6; // skip CMD:2:
        if (*colorStr == '#') colorStr++;

        // Convert hex string to int
        uint32_t colorVal = (uint32_t)strtoul(colorStr, NULL, 16);

        // Convert to lv_color_t
        lv_color_t newColor = lv_color_hex(colorVal);
        lv_style_set_bg_color(&main_style, newColor);
    }

    // CMD:3 Toggle Notifications
    else if (strncmp(cmd, "CMD:3", 5) == 0) {
        static bool notificationsEnabled = false;
        notificationsEnabled = !notificationsEnabled;
        if (notificationsEnabled) {
        } else {
        }
    }

    // CMD:4:UP or CMD:4:DOWN
    else if (strncmp(cmd, "CMD:4:", 6) == 0) {
        const char *dir = cmd + 6; // "UP" or "DOWN"
        if (strncmp(dir, "UP", 2) == 0) {
            // Increase brightness
            // need to implement setBrightness(current + 10)
        } else if (strncmp(dir, "DOWN", 4) == 0) {
            // Decrease brightness
        }
    }

    // 5) CMD:5 Toggle Clock Display
    else if (strncmp(cmd, "CMD:5", 5) == 0) {
        // If watchface is analog switch to digital and vice versa
        static bool clockOn = true;
        clockOn = !clockOn;
        if (clockOn) {
            load_screen(SCREEN_watchface_digital_0);
        } else {
            load_screen(SCREEN_settings);
        }
    }

    // 6) CMD:6:MM/DD/YYYY,HH:MM:SS
    else if (strncmp(cmd, "CMD:6:", 6) == 0) {
        const char *timeString = cmd + 6; // skip "CMD:6:"
        setTimeFromString(timeString);

        RTC_TimeTypeDef time;
        RTC_DateTypeDef date;
        HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

        lv_subject_set_int(&gui_data.ctime_h, time.Hours);
        lv_subject_set_int(&gui_data.ctime_m, time.Minutes);
        lv_subject_set_int(&gui_data.ctime_s, time.Seconds);
    }

    // unknown commands
    else {
        printf("Unknown command received: %s\r\n", cmd);
    }
}
