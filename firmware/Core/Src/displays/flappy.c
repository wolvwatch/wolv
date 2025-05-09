/*
The MIT License (MIT)

Copyright (c) 2025 Sandro Petrovski, Austin Sierco, Ryan Kaelle, and Tenzin Sherab

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#include "displays/flappy.h"
#include "ux/display.h"
#include "ux/rasterizer.h"
#include "displays/data.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "displays/launcher.h"

#define GRAVITY         0.8f
#define MAX_VEL_Y      12.0f
#define FLAP_V         -8.0f
#define PIPE_SPEED     3
#define GAP_HEIGHT     50
#define PIPE_WIDTH     20
#define BIRD_RADIUS    7
#define GROUND_Y       225
#define MAX_ROTATION   30.0f

extern app_data_t g_app_data;

static float velY;
static uint16_t birdY;
static int pipeX, gapY;
static int score;
static bool gameOver;

static void flappy_reset(void) {
    birdY = 120;
    velY = 0.0f;
    pipeX = 240;
    gapY = rand() % (GROUND_Y - 2 * GAP_HEIGHT) + GAP_HEIGHT;
    score = 0;
    gameOver = false;
}

void flappy_init(void) {
    srand((unsigned) time(NULL));
    flappy_reset();
}

static bool collides(void) {
    if (birdY - BIRD_RADIUS < 0 || birdY + BIRD_RADIUS > GROUND_Y)
        return true;

    if (pipeX < (CENTER_X - 40 + BIRD_RADIUS) &&
        pipeX + PIPE_WIDTH > (CENTER_X - 40 - BIRD_RADIUS)) {
        if (birdY - BIRD_RADIUS < gapY - GAP_HEIGHT / 2 ||
            birdY + BIRD_RADIUS > gapY + GAP_HEIGHT / 2)
            return true;
    }
    return false;
}

// gravity & scoring
void flappy_update(void) {
    if (gameOver) return;

    // gravity
    velY = fminf(velY + GRAVITY, MAX_VEL_Y);
    birdY += velY;

    // move pipe
    pipeX -= PIPE_SPEED;
    if (pipeX + PIPE_WIDTH < 0) {
        pipeX = 240;
        gapY = rand() % (GROUND_Y - 2 * GAP_HEIGHT) + GAP_HEIGHT;
        score++;
    }

    if (collides()) {
        gameOver = true;
    }
}

void flappy_draw(void) {
    // ground
    draw_rectangle(0, 0, 240, 240, COLOR_BLUE);
    draw_rectangle(0, GROUND_Y, 240, 240 - GROUND_Y, COLOR_GREEN);

    // pipes
    draw_rectangle(pipeX, 0, PIPE_WIDTH, gapY - GAP_HEIGHT / 2, COLOR_RED);
    draw_rectangle(pipeX, gapY + GAP_HEIGHT / 2, PIPE_WIDTH, GROUND_Y - (gapY + GAP_HEIGHT / 2),COLOR_RED);

    const int tail_height = 12;
    const int max_width = 7;
    for (int i = 0; i < tail_height; i++) {
        int width;
        if (i < tail_height / 2) {
            width = i + 1;
        } else {
            width = tail_height - i;
        }
        int x_start = CENTER_X - BIRD_RADIUS - 3 - (width / 2);
        draw_line(x_start, birdY - tail_height / 2 + i,
                  x_start + width, birdY - tail_height / 2 + i,COLOR_YELLOW, 1);
    }

    // bird body
    draw_arc(0, 358, CENTER_X, birdY, BIRD_RADIUS, COLOR_YELLOW, true, 2);

    // bird eye
    draw_arc(0, 358, CENTER_X + 3, birdY - 2, 1, COLOR_BLACK, true, 1);

    // bird wing (downward triangle)
    for (int i = 0; i <= 7; i++) {
        int width = 7 - i;
        int x_start = CENTER_X - 2 + i / 2;
        draw_line(x_start, birdY + 4 + i,
                  x_start + width, birdY + 4 + i,
                  COLOR_BLACK, 1);
    }

    // score
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", score);
    draw_text(buf, CENTER_X, 20, &montserrat_reg, COLOR_WHITE, 1.0f, true);

    if (gameOver) {
        draw_text("GAME OVER", CENTER_X, CENTER_Y, &montserrat_reg, COLOR_WHITE, 0.7f, true);
        draw_text("SEL to restart", CENTER_X, CENTER_Y + 20, &montserrat_reg, COLOR_WHITE, 0.5f, true);
    }
}

void flappy_input(button_t btn) {
    if (btn == BTN_UP || btn == BTN_DOWN) {
        // if button up or down, return to launcher
        g_app_data.display.active_screen = SCREEN_LAUNCHER;
        launcher_init();
        return;
    }

    if (btn == BTN_SEL) {
        if (gameOver) {
            flappy_reset();
        } else {
            velY = FLAP_V;
        }
    }
}

const app_t flappy_app = {
    .init = flappy_init,
    .update = flappy_update,
    .draw = flappy_draw,
    .input = flappy_input
};
