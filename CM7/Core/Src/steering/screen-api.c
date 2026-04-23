#include "screen-api.h"
#include "inputs-shared.h"
#include "eagletrt-api.h"
#include "raster-api.h"

EAGLETRT_STATIC struct ScreenHandler screen_handler;

// example interface with 8 boxes, each with a different color and position (needs to define the final interface)
EAGLETRT_STATIC struct RasterBox main_interface[8] = {
    { .updated = true, .id = 0, .rect = { .x = 10, .y = 10, .w = 200, .h = 50 }, .color = { .argb = 0xFF0000FF }, .label = NULL },
    { .updated = true, .id = 1, .rect = { .x = 10, .y = 70, .w = 200, .h = 50 }, .color = { .argb = 0xFF00FF00 }, .label = NULL },
    { .updated = true, .id = 2, .rect = { .x = 10, .y = 130, .w = 200, .h = 50 }, .color = { .argb = 0xFFFF0000 }, .label = NULL },
    { .updated = true, .id = 3, .rect = { .x = 10, .y = 190, .w = 200, .h = 50 }, .color = { .argb = 0xFFFFFF00 }, .label = NULL },
    { .updated = true, .id = 4, .rect = { .x = 10, .y = 250, .w = 200, .h = 50 }, .color = { .argb = 0xFFFF00FF }, .label = NULL },
    { .updated = true, .id = 5, .rect = { .x = 10, .y = 310, .w = 200, .h = 50 }, .color = { .argb = 0xFF00FFFF }, .label = NULL },
    { .updated = true, .id = 6, .rect = { .x = 10, .y = 370, .w = 200, .h = 50 }, { .argb = 0xFFFFFFFF }, .label = NULL },
    { .updated = true, .id = 7, .rect = { .x = 220, .y = 10, .w = 570, .h = 410 }, { .argb = 0xFFCCCCCC }, .label = NULL }
};

enum InputEventsReturnCode mock_input_event_button_event_callback(enum InputsSharedButtonID button_id) {
    EAGLETRT_API_UNUSED(button_id);
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_button_long_press_callback(enum InputsSharedButtonID button_id) {
    EAGLETRT_API_UNUSED(button_id);
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_button_release_callback(enum InputsSharedButtonID button_id) {
    EAGLETRT_API_UNUSED(button_id);
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_knob_rotation_callback(enum InputsSharedKnobID knob_id, int8_t delta) {
    EAGLETRT_API_UNUSED(knob_id);
    EAGLETRT_API_UNUSED(delta);
    return INPUT_EVENTS_RC_OK;
}

enum ScreenReturnCode screen_init(font_draw_line_callback draw_line, raster_draw_rectangle_callback draw_rectangle) {
    uint16_t len = sizeof(main_interface) / sizeof(main_interface[0]);
    raster_api_init(&screen_handler.raster, main_interface, len, draw_line, draw_rectangle, NULL);
    return SCREEN_RC_OK;
}
