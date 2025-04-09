#include "../Inc/ux/rasterizer.h"
#include <math.h>
#include <stdlib.h>
#include "drivers/lcd.h"

/**
 * A very simple helper that sets a pixel while checking bounds, if desired.
 * Adjust if you have a different safe-pixel-set approach.
 */
static inline void put_pixel_safe(int x, int y, color_t color) {
    if (x >= 0 && x < 240 && y >= 0 && y < 240) {
        screen_set_pixel((uint16_t) x, (uint16_t) y, color);
    }
}


void draw_line(uint16_t x0, uint16_t y0,
               uint16_t x1, uint16_t y1,
               color_t color,
               uint16_t stroke) {
    int dx = (int) x1 - (int) x0;
    int dy = (int) y1 - (int) y0;

    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    float xInc = (float) dx / (float) steps;
    float yInc = (float) dy / (float) steps;

    float x = (float) x0;
    float y = (float) y0;

    int halfStroke = stroke / 2;

    for (int i = 0; i <= steps; i++) {
        if (stroke == 1) {
            put_pixel_safe((int) (x + 0.5f), (int) (y + 0.5f), color);
        } else {
            if (abs(dx) >= abs(dy)) {
                for (int offset = -halfStroke; offset <= halfStroke; offset++) {
                    put_pixel_safe((int) (x + 0.5f), (int) (y + 0.5f) + offset, color);
                }
            } else {
                for (int offset = -halfStroke; offset <= halfStroke; offset++) {
                    put_pixel_safe((int) (x + 0.5f) + offset, (int) (y + 0.5f), color);
                }
            }
        }

        x += xInc;
        y += yInc;
    }
}

void draw_arc(uint16_t startAngle,
              uint16_t endAngle,
              uint16_t cx,
              uint16_t cy,
              uint16_t radius,
              color_t color,
              bool fill,
              uint16_t stroke) {
    startAngle = startAngle % 360;
    endAngle = endAngle % 360;
    if (endAngle < startAngle) {
        endAngle += 360;
    }
    float step = 1.0f;
    float degToRad = (float) M_PI / 180.0f;
    int xPrev = cx + (int) (radius * cosf(startAngle * degToRad) + 0.5f);
    int yPrev = cy + (int) (radius * sinf(startAngle * degToRad) + 0.5f);

    for (float angle = (float) startAngle; angle <= (float) endAngle; angle += step) {
        float rad = angle * degToRad;
        int xArc = cx + (int) (radius * cosf(rad) + 0.5f);
        int yArc = cy + (int) (radius * sinf(rad) + 0.5f);

        if (fill) {
            draw_line(cx, cy, xArc, yArc, color, stroke);
            draw_line(xPrev, yPrev, xArc, yArc, color, stroke);
        } else {
            draw_line(xPrev, yPrev, xArc, yArc, color, stroke);
        }

        xPrev = xArc;
        yPrev = yArc;
    }
}
