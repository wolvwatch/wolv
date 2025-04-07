#ifndef CORE_INC_RASTERIZER_H_
#define CORE_INC_RASTERIZER_H_

#include <stdint.h>
#include <stdbool.h>
#include "lcd.h"

#define SET_PIXEL(x, y, color) screen_set_pixel(x, y, color);

/**
 * @brief Defines the color type (16-bit).
 */
typedef uint16_t color_t;

/**
 * @brief Initialize font rendering at a specific pixel height.
 *
 * @param font_pixel_height  The desired pixel height for the font.
 */
void font_init(int font_pixel_height);

/**
 * @brief Draw a single character to the screen.
 *
 * @param x      The x-coordinate (in pixels).
 * @param y      The y-coordinate (in pixels). Typically the baseline if you’re matching other text draws.
 * @param c      The character to draw.
 * @param color  The color (16-bit) used for drawing.
 */
void draw_char(int x, int y, char c, color_t color);

/**
 * @brief Draw a text string to the screen. Handles newlines (\n).
 *
 * @param x      The starting x-coordinate (in pixels).
 * @param y      The starting y-coordinate (in pixels). Typically the baseline for the first line.
 * @param text   Null-terminated string to draw.
 * @param color  The color (16-bit) used for drawing the text.
 */
void draw_text(int x, int y, const char *text, color_t color);

/**
 * @brief Draw a line on the screen using a specified stroke width (Bresenham’s algorithm).
 *
 * @param x0     The starting x-coordinate.
 * @param y0     The starting y-coordinate.
 * @param x1     The ending x-coordinate.
 * @param y1     The ending y-coordinate.
 * @param color  The 16-bit color of the line.
 * @param stroke The width of the line stroke (1 = single pixel).
 */
void draw_line(uint16_t x0, uint16_t y0,
               uint16_t x1, uint16_t y1,
               color_t color,
               uint16_t stroke);

/**
 * @brief Draw an arc (or partial circle) on the screen.
 *
 * Angles are interpreted as degrees, with 0° at the circle’s rightmost point,
 * increasing counter-clockwise (90° at top, 180° at left, 270° at bottom).
 *
 * @param start_angle  The starting angle in degrees.
 * @param end_angle    The ending angle in degrees (can exceed 360).
 * @param x            The x-coordinate of the arc’s center.
 * @param y            The y-coordinate of the arc’s center.
 * @param radius       The radius of the arc in pixels.
 * @param color        The 16-bit color of the arc.
 * @param fill         If true, fill the sector; if false, only draw the outline.
 * @param stroke       Outline thickness if fill == false (ignored if fill == true).
 */
void draw_arc(uint16_t start_angle,
              uint16_t end_angle,
              uint16_t x,
              uint16_t y,
              uint16_t radius,
              color_t color,
              bool fill,
              uint16_t stroke);

#endif /* CORE_INC_RASTERIZER_H_ */