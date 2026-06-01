/*!
 * \file popup-api.c
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief API for the parameter-change popup overlay.
 */

#include "popup-api.h"
#include "font.h"
#include "label-api.h"
#include "box-api.h"
#include "screen.h"
#include "eagletrt-api.h"
#include "raster-fonts.h"
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
 *
 * \param parameter_id The parameter to check.
 *
 * \retval true if the parameter is a toggle
 * \retval false if the parameter is a numeric value
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
EAGLETRT_STATIC void prv_popup_format_value(struct PopupHandler *handler, const enum InputsSharedParameterID parameter_id, uint8_t value) {
    if (prv_popup_is_toggle(parameter_id)) {
        EAGLETRT_API_UNUSED(snprintf(handler->value_buffer, POPUP_VALUE_BUFFER_SIZE, "%s", value ? "ON" : "OFF"));
    } else {
        EAGLETRT_API_UNUSED(snprintf(handler->value_buffer, POPUP_VALUE_BUFFER_SIZE, "%u", (unsigned)value));
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

    if (label_api_init(&handler->labels[0], prv_parameter_names[INPUTS_SHARED_PARAMETER_ID_POWER], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, &font_konexy, 48, FONT_ALIGN_CENTER, (struct Color){ .argb = 0xFFFFFFFF }) != RASTER_RC_OK) {
        return POPUP_RC_ERROR;
    }
    if (label_api_init(&handler->labels[1], handler->value_buffer, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, &font_konexy, 96, FONT_ALIGN_CENTER, (struct Color){ .argb = 0xFFFFFFFF }) != RASTER_RC_OK) {
        return POPUP_RC_ERROR;
    }
    if (box_api_init(&handler->boxes[0], 0, (struct BoxRectangle){ .x = 0, .y = 0, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT / 2 }, (struct Color){ .argb = 0xFF101010 }, &handler->labels[0]) != RASTER_RC_OK) {
        return POPUP_RC_ERROR;
    }
    if (box_api_init(&handler->boxes[1], 1, (struct BoxRectangle){ .x = 0, .y = SCREEN_HEIGHT / 2, .width = SCREEN_WIDTH, .height = SCREEN_HEIGHT / 2 }, (struct Color){ .argb = 0xFF1E1E1E }, &handler->labels[1]) != RASTER_RC_OK) {
        return POPUP_RC_ERROR;
    }
    return POPUP_RC_OK;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
enum PopupReturnCode popup_api_show(struct PopupHandler *handler, enum InputsSharedParameterID parameter_id, uint8_t value, uint32_t tick) {
    if (handler == NULL || parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return POPUP_RC_ERROR;
    }

    handler->active = true;
    handler->last_event_tick = tick;
    handler->current_parameter = parameter_id;

    handler->labels[0].text = (char *)prv_parameter_names[parameter_id];

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
