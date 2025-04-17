#include "displays/splashscreen.h"
#include "ux/rasterizer.h"
#include "drivers/lcd.h"
#include <stdint.h>

#include "displays/data.h"


extern const tImage logo;

void splashscreen_draw(void) {
    uint16_t x = (LCD_1IN28_WIDTH - logo.width) / 2;
    uint16_t y = (LCD_1IN28_HEIGHT - logo.height) / 2;
    
    draw_image(&logo, x, y, COLOR_WHITE);
} 