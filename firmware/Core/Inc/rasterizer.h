#ifndef CORE_INC_RASTERIZER_H_
#define CORE_INC_RASTERIZER_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint16_t color_t;

/**
* Draw an arc onto the screen.
* Params:
* - start_angle: the angle to start the draw from. 0deg is the rightmost point on the circle, with the degrees
*   incrementing counter-clockwise (i.e. straight up is 90deg, leftmost point is 180deg, bottommost point is 270deg)
* - end_angle: the angle to end the draw from, if this is 360 and start_angle is 0, this should result in a full circle
*   if this is 180deg and start_angle is 0deg, this should result in a half-arc
* - x & y: the coordinates of the arc's center
* - radius: radius of the arc from center
* - color: the color of the arc
* - fill: if true, the arc should be filled in with color, if false should just be outlined with stroke
* - stroke: the width of the outline stroke, ignored if fill is true
*/
void draw_arc(uint16_t start_angle, uint16_t end_angle, uint16_t x, uint16_t y, uint16_t radius, color_t color, bool fill, uint16_t stroke);

/**
* Draw a line on the screen.
* Params:
* - x0 & y0: the starting point
* - x1 & y0: the ending point
* - color: the color of the stroke
* - stroke: the width of the stroke
*/
void draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, color_t color, uint16_t stroke);

#endif /* CORE_INC_RASTERIZER_H_ */ 