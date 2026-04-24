#include "screen-api.h"
#include "fontutils.h"
#include "inputs-shared.h"
#include "eagletrt-api.h"
#include "raster-api.h"

EAGLETRT_STATIC struct ScreenHandler screen_handler;

EAGLETRT_STATIC struct RasterLabel main_interface_labels[8] = {
    { .type = LABEL_DATA_STRING, .data.text = "Box 1", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 35 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 2", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 95 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 3", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 155 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 4", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 215 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 5", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 275 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 6", .format.string_fmt = { .max_length = 0 }, .pos = { .x = 110, .y = 335 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 7", .format = { .string_fmt = { .max_length = 0 } }, .pos = { .x = 110, .y = 395 }, .font = FONT_KONEXY, .size = 16, .color = { .argb = 0xFFFFFFFF }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Main Area", .format = { .string_fmt = { .max_length = 0 } }, .pos = { .x = 535, .y = 215 }, .font = FONT_KONEXY, .size = 24, .color = { .argb = 0xFF000000 }, .align = FONT_ALIGN_CENTER }
};

// example interface with 8 boxes, each with a different color and position (needs to define the final interface)
EAGLETRT_STATIC struct RasterBox main_interface[8] = {
    { .updated = true, .id = 0, .rect = { .x = 10, .y = 10, .w = 200, .h = 50 }, .color = { .argb = 0xFF0000FF }, .label = &main_interface_labels[0] },
    { .updated = true, .id = 1, .rect = { .x = 10, .y = 70, .w = 200, .h = 50 }, .color = { .argb = 0xFF00FF00 }, .label = &main_interface_labels[1] },
    { .updated = true, .id = 2, .rect = { .x = 10, .y = 130, .w = 200, .h = 50 }, .color = { .argb = 0xFFFF0000 }, .label = &main_interface_labels[2] },
    { .updated = true, .id = 3, .rect = { .x = 10, .y = 190, .w = 200, .h = 50 }, .color = { .argb = 0xFFFFFF00 }, .label = &main_interface_labels[3] },
    { .updated = true, .id = 4, .rect = { .x = 10, .y = 250, .w = 200, .h = 50 }, .color = { .argb = 0xFFFF00FF }, .label = &main_interface_labels[4] },
    { .updated = true, .id = 5, .rect = { .x = 10, .y = 310, .w = 200, .h = 50 }, .color = { .argb = 0xFF00FFFF }, .label = &main_interface_labels[5] },
    { .updated = true, .id = 6, .rect = { .x = 10, .y = 370, .w = 200, .h = 50 }, .color = { .argb = 0xFFFFFFFF }, .label = &main_interface_labels[6] },
    { .updated = true, .id = 7, .rect = { .x = 220, .y = 10, .w = 570, .h = 410 }, .color = { .argb = 0xFFCCCCCC }, .label = &main_interface_labels[7] }
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

enum ScreenReturnCode screen_update(uint32_t tick) {
    raster_api_render(&screen_handler.raster);
    return SCREEN_RC_OK;
}
