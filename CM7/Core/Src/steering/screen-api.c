#include "screen-api.h"
#include "eagletrt.h"
#include "fontutils.h"
#include "inputs-shared.h"
#include "popup-api.h"
#include "raster-api.h"

EAGLETRT_STATIC struct ScreenHandler screen_handler;

#define SCREEN_INTERFACE_BOX_COUNT (10U)
#define SCREEN_LABEL_X_POS (100U)
#define SCREEN_LABEL_Y_POS (50U)
#define SCREEN_LABEL_X_POS_L (400U)
#define SCREEN_LABEL_Y_POS_L (50U)
#define SCREEN_LABEL_COLOR (0xFFFFFFFFU)
#define SCREEN_LABEL_FONT_SIZE_M (22U)
#define SCREEN_LABEL_FONT_SIZE_L (30U)
#define SCREEN_BOX_WIDTH_M (200U)
#define SCREEN_BOX_HEIGHT_M (150U)

EAGLETRT_STATIC struct RasterLabel main_interface_labels[SCREEN_INTERFACE_BOX_COUNT] = {
    { .type = LABEL_DATA_STRING, .data.text = "Box 1", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 2", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 3", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 4", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 5", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 6", .format.string_fmt = { 0 }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 7", .format = { .string_fmt = { 0 } }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Box 8", .format = { .string_fmt = { 0 } }, .pos = { .x = SCREEN_LABEL_X_POS, .y = SCREEN_LABEL_Y_POS }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_M, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER },
    { .type = LABEL_DATA_STRING, .data.text = "Main Area", .format = { .string_fmt = { 0 } }, .pos = { .x = SCREEN_LABEL_X_POS_L, .y = SCREEN_LABEL_Y_POS_L }, .font = FONT_KONEXY, .size = SCREEN_LABEL_FONT_SIZE_L, .color = { .argb = SCREEN_LABEL_COLOR }, .align = FONT_ALIGN_CENTER }
};

// example interface with 8 boxes, each with a different color and position (needs to define the final interface)
EAGLETRT_STATIC struct RasterBox main_interface[SCREEN_INTERFACE_BOX_COUNT] = {
    { .updated = true, .id = 0, .rect = { .x = 0, .y = 0, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFF0000FF }, .label = &main_interface_labels[0] },
    { .updated = true, .id = 1, .rect = { .x = 200, .y = 0, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFF00FF00 }, .label = &main_interface_labels[1] },
    { .updated = true, .id = 2, .rect = { .x = 400, .y = 0, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFFFF0000 }, .label = &main_interface_labels[2] },
    { .updated = true, .id = 3, .rect = { .x = 600, .y = 0, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFFFFFF00 }, .label = &main_interface_labels[3] },
    { .updated = true, .id = 4, .rect = { .x = 0, .y = 150, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFFFF00FF }, .label = &main_interface_labels[4] },
    { .updated = true, .id = 5, .rect = { .x = 200, .y = 150, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFF00FFFF }, .label = &main_interface_labels[5] },
    { .updated = true, .id = 6, .rect = { .x = 400, .y = 150, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFFFFFFFF }, .label = &main_interface_labels[6] },
    { .updated = true, .id = 7, .rect = { .x = 600, .y = 150, .w = SCREEN_BOX_WIDTH_M, .h = SCREEN_BOX_HEIGHT_M }, .color = { .argb = 0xFFCCCCCC }, .label = &main_interface_labels[7] },
    { .updated = true, .id = 8, .rect = { .x = 0, .y = 300, .w = 180, .h = 800 }, .color = { .argb = 0xFFCCCCCC }, .label = &main_interface_labels[7] }
};

enum InputEventsReturnCode screen_on_parameter_change(const enum InputsSharedParameterID parameter_id, uint8_t value) {
    // Called from the HSEM ISR: only update the popup state here.
    // The raster interface swap is performed by screen_update in the main loop.
    if (popup_api_show(&screen_handler.popup, parameter_id, value, screen_handler.last_event_tick) != POPUP_RC_OK) {
        return INPUT_EVENTS_RC_ERROR;
    }
    return INPUT_EVENTS_RC_OK;
}

enum ScreenReturnCode screen_init(font_draw_line_callback draw_line, raster_draw_rectangle_callback draw_rectangle) {
    raster_api_init(&screen_handler.raster, main_interface, SCREEN_INTERFACE_BOX_COUNT, draw_line, draw_rectangle, NULL);

    if (popup_api_init(&screen_handler.popup) != POPUP_RC_OK) {
        return SCREEN_RC_ERROR;
    }
    screen_handler.popup_visible = false;
    screen_handler.last_event_tick = 0U;

    return SCREEN_RC_OK;
}

enum ScreenReturnCode screen_update(uint32_t tick) {
    screen_handler.last_event_tick = tick;

    bool popup_active = popup_api_is_active(&screen_handler.popup, tick);

    if (popup_active && !screen_handler.popup_visible) {
        raster_api_set_interface(&screen_handler.raster, screen_handler.popup.boxes, POPUP_BOX_COUNT);
        screen_handler.popup_visible = true;
    } else if (!popup_active && screen_handler.popup_visible) {
        raster_api_set_interface(&screen_handler.raster, main_interface, SCREEN_INTERFACE_BOX_COUNT);
        screen_handler.popup_visible = false;
    }

    raster_api_render(&screen_handler.raster);
    return SCREEN_RC_OK;
}
