#include "screen-api.h"
#include "box-api.h"
#include "eagletrt.h"
#include "raster-api.h"
#include "inputs-shared.h"
#include "popup-api.h"
#include "label-api.h"
#include "raster-fonts.h"

EAGLETRT_STATIC struct ScreenHandler screen_handler;

#define SCREEN_INTERFACE_BOX_COUNT (10U)
#define SCREEN_LABEL_X_POS (100U)
#define SCREEN_LABEL_Y_POS (50U)
#define SCREEN_LABEL_X_POS_L (400U)
#define SCREEN_LABEL_Y_POS_L (50U)
#define SCREEN_LABEL_COLOR ((struct Color){ .argb = 0xFFFFFFFFU })
#define SCREEN_LABEL_FONT_SIZE_M (22U)
#define SCREEN_LABEL_FONT_SIZE_L (30U)
#define SCREEN_BOX_WIDTH_M (200U)
#define SCREEN_BOX_HEIGHT_M (150U)

EAGLETRT_STATIC struct Label main_interface_labels[SCREEN_INTERFACE_BOX_COUNT];

// example interface with 8 boxes, each with a different color and position (needs to define the final interface)
EAGLETRT_STATIC struct Box main_interface[SCREEN_INTERFACE_BOX_COUNT];

EAGLETRT_STATIC enum RasterReturnCode prv_init_labels(void) {
    enum RasterReturnCode rc = RASTER_RC_OK;
    if (label_api_init(&main_interface_labels[0], "Box 1", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[1], "Box 2", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[2], "Box 3", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[3], "Box 4", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[4], "Box 5", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[5], "Box 6", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[6], "Box 7", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[7], "Box 8", SCREEN_LABEL_X_POS, SCREEN_LABEL_Y_POS, &font_konexy, SCREEN_LABEL_FONT_SIZE_M, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (label_api_init(&main_interface_labels[8], "Main Area", SCREEN_LABEL_X_POS_L, SCREEN_LABEL_Y_POS_L, &font_konexy, SCREEN_LABEL_FONT_SIZE_L, FONT_ALIGN_CENTER, SCREEN_LABEL_COLOR) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    return rc;
}

EAGLETRT_STATIC enum RasterReturnCode prv_init_interface(void) {
    enum RasterReturnCode rc = RASTER_RC_OK;
    if (box_api_init(&main_interface[0], 0, (struct BoxRectangle){ .x = 0, .y = 0, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFF0000FF }, &main_interface_labels[0]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[1], 1, (struct BoxRectangle){ .x = 200, .y = 0, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFF00FF00 }, &main_interface_labels[1]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[2], 2, (struct BoxRectangle){ .x = 400, .y = 0, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFFFF0000 }, &main_interface_labels[2]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[3], 3, (struct BoxRectangle){ .x = 600, .y = 0, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFFFFFF00 }, &main_interface_labels[3]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[4], 4, (struct BoxRectangle){ .x = 0, .y = 150, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFFFF00FF }, &main_interface_labels[4]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[5], 5, (struct BoxRectangle){ .x = 200, .y = 150, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFF00FFFF }, &main_interface_labels[5]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[6], 6, (struct BoxRectangle){ .x = 400, .y = 150, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFFFFFFFF }, &main_interface_labels[6]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[7], 7, (struct BoxRectangle){ .x = 600, .y = 150, .width = SCREEN_BOX_WIDTH_M, .height = SCREEN_BOX_HEIGHT_M }, (struct Color){ .argb = 0xFFCCCCCC }, &main_interface_labels[7]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    if (box_api_init(&main_interface[8], 8, (struct BoxRectangle){ .x = 0, .y = 300, .width = 180, .height = 800 }, (struct Color){ .argb = 0xFFCCCCCC }, &main_interface_labels[8]) != RASTER_RC_OK) {
        rc = RASTER_RC_ERROR;
    }
    return rc;
}

enum InputEventsReturnCode screen_on_parameter_change(const enum InputsSharedParameterID parameter_id, uint8_t value) {
    // Called from the HSEM ISR: only update the popup state here.
    // The raster interface swap is performed by screen_update in the main loop.
    if (popup_api_show(&screen_handler.popup, parameter_id, value, screen_handler.last_event_tick) != POPUP_RC_OK) {
        return INPUT_EVENTS_RC_ERROR;
    }
    return INPUT_EVENTS_RC_OK;
}

enum ScreenReturnCode screen_init(raster_draw_rectangle_callback draw_rectangle) {
    if (prv_init_labels() != RASTER_RC_OK) {
        return SCREEN_RC_ERROR;
    }
    if (prv_init_interface() != RASTER_RC_OK) {
        return SCREEN_RC_ERROR;
    }
    if (raster_api_init(&screen_handler.raster, main_interface, SCREEN_INTERFACE_BOX_COUNT, draw_rectangle, NULL) != RASTER_RC_OK) {
        return SCREEN_RC_ERROR;
    }

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
