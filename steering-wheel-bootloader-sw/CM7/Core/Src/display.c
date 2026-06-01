#include "eagletrt-api.h"
#include "display.h"
#include "drawing-api.h"
#include "box-api.h"
#include "label-api.h"
#include "raster-api.h"
#include "raster-fonts.h"
#include <stddef.h>

EAGLETRT_STATIC struct RasterHandler raster_handler;
EAGLETRT_STATIC struct Label label;
EAGLETRT_STATIC struct Box interface;

enum RasterReturnCode display_init() {
    if (label_api_init(&label, "Booting...", 400, 220, &font_konexy, 40, FONT_ALIGN_CENTER, (struct Color){ .argb = 0xFFFFFFFF }) != RASTER_RC_OK) {
        return RASTER_RC_ERROR;
    }
    if (box_api_init(&interface, 0, (struct BoxRectangle){ .x = 0, .y = 0, .width = 800, .height = 480 }, (struct Color){ .argb = 0xFF000000 }, &label) != RASTER_RC_OK) {
        return RASTER_RC_ERROR;
    }
    if (raster_api_init(&raster_handler, &interface, 1, draw_rectangle_callback, NULL) != RASTER_RC_OK) {
        return RASTER_RC_ERROR;
    }
    return RASTER_RC_OK;
}

void display_draw() {
    EAGLETRT_API_UNUSED(raster_api_render(&raster_handler));
}
