#include "displays/analog.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include "ux/display.h"
#include "ux/font.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

// Watch face constants
#define CENTER_X 118
#define CENTER_Y 120
#define WATCH_RADIUS 118

// Hand lengths
#define HOUR_HAND_LENGTH   50
#define MINUTE_HAND_LENGTH 80
#define SECOND_HAND_LENGTH 90

// Color definitions (adjust these values based on your display's color format)
#ifndef COLOR_WHITE
#define COLOR_WHITE 0b111
#endif

#ifndef COLOR_BLACK
#define COLOR_BLACK 0b111
#endif

#ifndef COLOR_RED
#define COLOR_RED 0b110
#endif


//
// PUBLIC FUNCTIONS
//

void analog_init(void) {
    // Perform any required initialization.
    // For example, you might want to clear the display.
    // clear_screen(COLOR_WHITE); // Uncomment if you have a clear screen function.
}

void analog_update(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    // Optionally clear the display before drawing.
    // clear_screen(COLOR_WHITE);

    // Draw the static parts of the watch face.
    draw_watch_face();
    draw_hour_markers();

    // Draw the dynamic hands based on the current time.
    draw_watch_hands(hours, minutes, seconds);

    // Draw the center dot for aesthetics.
    draw_center_dot();

    // Format the digital time string and display it using the Montserrat font.
    char time_str[9]; // Format "HH:MM:SS"
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", hours, minutes, seconds);
    // Adjust the (x, y) position as needed.
    display_main_text(80, 200, time_str, COLOR_BLACK);
}

//
// INTERNAL HELPER FUNCTIONS
//

// Draws the outer circle of the watch face.
void draw_watch_face(void) {
    // Draw a full circle outline using draw_arc.
    // Note: Using 0 to 359 degrees and a stroke width of 3.
    draw_arc(0, 359, CENTER_X, CENTER_Y, WATCH_RADIUS, 0b001, false, 240);
}

// Draws hour markers (12 markers) around the watch's circumference.
void draw_hour_markers(void) {
    for (int i = 0; i < 12; i++) {
        // Each marker is 30 degrees apart.
        float angle = i * 30.0f;
        float rad = angle * (M_PI / 180.0f);

        // Compute the outer point on the circle.
        int x_outer = CENTER_X + (int)(WATCH_RADIUS * cosf(rad) + 0.5f);
        int y_outer = CENTER_Y + (int)(WATCH_RADIUS * sinf(rad) + 0.5f);
        // Compute the inner point; here we shorten the marker by 10 pixels.
        int x_inner = CENTER_X + (int)((WATCH_RADIUS - 10) * cosf(rad) + 0.5f);
        int y_inner = CENTER_Y + (int)((WATCH_RADIUS - 10) * sinf(rad) + 0.5f);

        // Draw the marker using a line with a stroke thickness of 2.
        draw_line(x_inner, y_inner, x_outer, y_outer, COLOR_BLACK, 2);
    }
}

// Draws the hour, minute, and second hands based on the provided time.
void draw_watch_hands(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    const float degToRad = M_PI / 180.0f;

    // Calculate hand angles in degrees.
    float hour_angle = ((hours % 12) + minutes / 60.0f) * 30.0f;
    float minute_angle = minutes * 6.0f;
    float second_angle = seconds * 6.0f;

    // Calculate and draw the hour hand.
    float hour_rad = hour_angle * degToRad;
    int hour_x = CENTER_X + (int)(HOUR_HAND_LENGTH * cosf(hour_rad) + 0.5f);
    int hour_y = CENTER_Y + (int)(HOUR_HAND_LENGTH * sinf(hour_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, hour_x, hour_y, COLOR_BLACK, 4);

    // Calculate and draw the minute hand.
    float minute_rad = minute_angle * degToRad;
    int minute_x = CENTER_X + (int)(MINUTE_HAND_LENGTH * cosf(minute_rad) + 0.5f);
    int minute_y = CENTER_Y + (int)(MINUTE_HAND_LENGTH * sinf(minute_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, minute_x, minute_y, COLOR_BLACK, 3);

    // Calculate and draw the second hand in red.
    float second_rad = second_angle * degToRad;
    int second_x = CENTER_X + (int)(SECOND_HAND_LENGTH * cosf(second_rad) + 0.5f);
    int second_y = CENTER_Y + (int)(SECOND_HAND_LENGTH * sinf(second_rad) + 0.5f);
    draw_line(CENTER_X, CENTER_Y, second_x, second_y, COLOR_RED, 1);
}

// Draws a small filled dot at the center of the watch.
void draw_center_dot(void) {
    // Draw a small square (5x5) centered on the watch center.
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            int x = CENTER_X + dx;
            int y = CENTER_Y + dy;
            screen_set_pixel((uint16_t)x, (uint16_t)y, COLOR_BLACK);
        }
    }
}