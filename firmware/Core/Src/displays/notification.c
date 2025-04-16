#include "displays/notification.h"
void display_notification(const char* message) {
    uint16_t start_x = 30;
    uint16_t start_y = 50;
    uint16_t width = 180;
    uint16_t height = 140;
    draw_rectangle(start_x, start_y, width, height, 0b111);
    draw_line(start_x, start_y, start_x + width - 1, start_y, 0b000, 1);
    draw_line(start_x, start_y + height - 1, start_x + width - 1, start_y + height - 1, 0b000, 1);
    draw_line(start_x, start_y, start_x, start_y + height - 1, 0b000, 1);
    draw_line(start_x + width - 1, start_y, start_x + width - 1, start_y + height - 1, 0b000, 1);
    uint16_t text_offset_x = 240/2;
    uint16_t text_offset_y = 240/2;
    draw_text(message, text_offset_x, text_offset_y, &montserrat_reg, 0b000, 1.0, false);
    screen_render();
}