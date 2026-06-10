#include <stdio.h>
#include "fsm.h"
#include "raster.h"
#include "post.h"
#include "tigr.h"

Tigr *screen;

enum RasterReturnCode draw_rectangle_callback(uint16_t x, uint16_t y, uint16_t width, uint16_t height, struct Color color) {
    for (uint16_t row = y; row < y + height; row++) {
        for (uint16_t col = x; col < x + width; col++) {
            TPixel *dst = &screen->pix[(row * screen->w) + col];
            uint8_t a = color.a;
            dst->r = ((color.r * a) + (dst->r * (255 - a))) / 255;
            dst->g = ((color.g * a) + (dst->g * (255 - a))) / 255;
            dst->b = ((color.b * a) + (dst->b * (255 - a))) / 255;
            dst->a = 255;
        }
    }
    return RASTER_RC_OK;
}

int main() {
    screen = tigrWindow(800, 480, "FSM Example", 0);

    fsm_state_t state = FSM_STATE_INIT;

    uint32_t tick = 0;

    struct PostInitData post_init_data = {
        .draw_rectangle = draw_rectangle_callback
    };

    state = fsm_run_state(state, &post_init_data);

    struct FsmData fsm_data = {
        .tick = tick
    };

    while (!tigrClosed(screen)) {
        fsm_data.tick = tick++;
        state = fsm_run_state(state, &fsm_data);
        tigrUpdate(screen);
    }
    tigrFree(screen);
    return 0;
}
