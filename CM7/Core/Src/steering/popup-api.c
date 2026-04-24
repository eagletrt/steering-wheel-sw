/*!
 * \file popup-api.c
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief API for the parameter-change popup overlay.
 */

#include "popup-api.h"
#include "screen.h"
#include "eagletrt.h"
#include <stddef.h>
#include <stdio.h>

/*!
 * \brief Human-readable names for each parameter, indexed by InputsSharedParameterID.
 */
EAGLETRT_STATIC const char *prv_parameter_names[INPUTS_SHARED_PARAMETER_ID_COUNT] = {
    [INPUTS_SHARED_PARAMETER_ID_POWER] = "POWER",
    [INPUTS_SHARED_PARAMETER_ID_REGEN] = "REGEN",
    [INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING] = "TORQUE",
    [INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = "TELEMETRY LOG",
    [INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL] = "LAUNCH CONTROL",
};

/*!
 * \brief Returns true if the parameter uses an ON/OFF toggle semantic.
 */
EAGLETRT_STATIC bool prv_popup_is_toggle(enum InputsSharedParameterID parameter_id) {
    return parameter_id == INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG ||
           parameter_id == INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL;
}

/*!
 * \brief Format a parameter value into the popup buffer.
 *
 * \param handler Pointer to the popup handler owning the buffer.
 * \param parameter_id The parameter being displayed.
 * \param value The new parameter value.
 */
EAGLETRT_STATIC void prv_popup_format_value(
    struct PopupHandler *handler,
    enum InputsSharedParameterID parameter_id,
    uint8_t value) {
    if (prv_popup_is_toggle(parameter_id)) {
        snprintf(handler->value_buffer, POPUP_VALUE_BUFFER_SIZE, "%s", value ? "ON" : "OFF");
    } else {
        snprintf(handler->value_buffer, POPUP_VALUE_BUFFER_SIZE, "%u", (unsigned)value);
    }
}

enum PopupReturnCode popup_api_init(struct PopupHandler *handler) {
    if (handler == NULL) {
        return POPUP_RC_ERROR;
    }

    handler->active = false;
    handler->last_event_tick = 0U;
    handler->current_parameter = INPUTS_SHARED_PARAMETER_ID_POWER;
    handler->value_buffer[0] = '\0';

    handler->labels[0] = (struct RasterLabel){
        .type = LABEL_DATA_STRING,
        .data.text = (char *)prv_parameter_names[INPUTS_SHARED_PARAMETER_ID_POWER],
        .format.string_fmt = { .max_length = 0 },
        .pos = { .x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 4 },
        .font = FONT_KONEXY,
        .size = 48,
        .color = { .argb = 0xFFFFFFFF },
        .align = FONT_ALIGN_CENTER,
    };
    handler->labels[1] = (struct RasterLabel){
        .type = LABEL_DATA_STRING,
        .data.text = handler->value_buffer,
        .format.string_fmt = { .max_length = 0 },
        .pos = { .x = SCREEN_WIDTH / 2, .y = SCREEN_HEIGHT / 4 },
        .font = FONT_KONEXY,
        .size = 96,
        .color = { .argb = 0xFFFFFFFF },
        .align = FONT_ALIGN_CENTER,
    };

    handler->boxes[0] = (struct RasterBox){
        .updated = true,
        .id = 0,
        .rect = { .x = 0, .y = 0, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT / 2 },
        .color = { .argb = 0xFF101010 },
        .label = &handler->labels[0],
    };
    handler->boxes[1] = (struct RasterBox){
        .updated = true,
        .id = 1,
        .rect = { .x = 0, .y = SCREEN_HEIGHT / 2, .w = SCREEN_WIDTH, .h = SCREEN_HEIGHT / 2 },
        .color = { .argb = 0xFF1E1E1E },
        .label = &handler->labels[1],
    };

    return POPUP_RC_OK;
}

enum PopupReturnCode popup_api_show(
    struct PopupHandler *handler,
    enum InputsSharedParameterID parameter_id,
    uint8_t value,
    uint32_t tick) {
    if (handler == NULL || parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return POPUP_RC_ERROR;
    }

    handler->active = true;
    handler->last_event_tick = tick;
    handler->current_parameter = parameter_id;

    handler->labels[0].data.text = (char *)prv_parameter_names[parameter_id];

    prv_popup_format_value(handler, parameter_id, value);

    return POPUP_RC_OK;
}

void popup_api_dismiss(struct PopupHandler *handler) {
    if (handler == NULL) {
        return;
    }
    handler->active = false;
}

bool popup_api_is_active(struct PopupHandler *handler, uint32_t tick) {
    if (handler == NULL || !handler->active) {
        return false;
    }
    if (tick - handler->last_event_tick >= SCREEN_POPUP_TIMEOUT_MS) {
        handler->active = false;
        return false;
    }
    return true;
}
