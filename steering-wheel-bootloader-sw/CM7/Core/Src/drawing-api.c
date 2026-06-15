#include "drawing-api.h"

#define DRAWING_API_FRAMEBUFFER_WIDTH (800U)
#define DRAWING_API_FRAMEBUFFER_HEIGHT (480U)

uint32_t framebuffer[DRAWING_API_FRAMEBUFFER_WIDTH * DRAWING_API_FRAMEBUFFER_HEIGHT]
    __attribute__((section(".framebuffer"), aligned(32)));

enum RasterReturnCode draw_rectangle_callback(uint16_t x, uint16_t y, uint16_t width, uint16_t height, struct Color color) {
    if (x >= DRAWING_API_FRAMEBUFFER_WIDTH || y >= DRAWING_API_FRAMEBUFFER_HEIGHT) {
        return RASTER_RC_ERROR;
    }
    for (uint16_t j = 0; j < height; ++j) {
        for (uint16_t i = 0; i < width; ++i) {
            if (x + i >= DRAWING_API_FRAMEBUFFER_WIDTH || y + j >= DRAWING_API_FRAMEBUFFER_HEIGHT) {
                break;
            }
            framebuffer[(y * DRAWING_API_FRAMEBUFFER_HEIGHT) + (x + i)] = color.argb;
        }
    }
    return RASTER_RC_OK;
}
