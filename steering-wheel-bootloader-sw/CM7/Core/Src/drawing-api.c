#include "drawing-api.h"

uint32_t framebuffer[800 * 480]
    __attribute__((section(".framebuffer"), aligned(32)));

void draw_line_callback(uint16_t x, uint16_t y, uint16_t width, struct Color color) {
    if (x >= 800 || y >= 480)
        return;
    for (uint16_t i = 0; i < width; ++i) {
        if (x + i >= 800)
            break;
        framebuffer[y * 800 + (x + i)] = color.argb;
    }
}
