/*!
 * \file parameters-api.c
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Hardware-agnostic API to manage the steering wheel tunable parameters.
 */

#include <string.h>

#include "parameters-api.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ParametersHandler handler;

/*!
 * \brief Adjust a numeric parameter by a signed delta, clamping to [0, MAX].
 *
 * \param parameter_id The parameter to adjust.
 * \param delta The signed amount to apply.
 *
 * \return The new value after clamping.
 */
EAGLETRT_STATIC uint8_t prv_parameters_adjust_numeric(
    enum InputsSharedParameterID parameter_id,
    int16_t delta) {
    int16_t next = (int16_t)handler.values[parameter_id] + delta;
    next = EAGLETRT_API_CLAMP(next, 0, (int16_t)INPUTS_SHARED_PARAMETER_NUMERIC_MAX);
    handler.values[parameter_id] = (uint8_t)next;
    return (uint8_t)next;
}

/*!
 * \brief Toggle a boolean parameter between 0 and 1.
 *
 * \param parameter_id The parameter to toggle.
 *
 * \return The new value after toggling.
 */
EAGLETRT_STATIC uint8_t prv_parameters_toggle(
    enum InputsSharedParameterID parameter_id) {
    handler.values[parameter_id] = handler.values[parameter_id] ? 0U : 1U;
    return handler.values[parameter_id];
}

/*!
 * \brief Build and push a parameter change event to CM7.
 *
 * \param parameter_id The parameter that changed.
 * \param value The new value of the parameter.
 *
 * \retval INPUTS_RC_OK if the event was notified successfully.
 * \retval INPUTS_RC_NOTIFY_ERROR if the notify callback failed.
 */
EAGLETRT_STATIC enum InputsReturnCode prv_parameters_notify(
    enum InputsSharedParameterID parameter_id,
    uint8_t value) {
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_PARAMETER_CHANGE,
        .parameter.parameter_id = parameter_id,
        .parameter.value = value,
    };
    if (!handler.notify_callback(event)) {
        return INPUTS_RC_NOTIFY_ERROR;
    }
    return INPUTS_RC_OK;
}

enum ParametersReturnCode parameters_api_init(inputs_notify_callback notify_callback) {
    if (notify_callback == NULL) {
        return PARAMETERS_RC_ERROR;
    }

    memset(&handler, 0, sizeof(handler));
    handler.notify_callback = notify_callback;

    return PARAMETERS_RC_OK;
}

enum InputsReturnCode parameters_api_handle_input(struct InputsSharedEvent event) {
    switch (event.type) {
        case INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION: {
            enum InputsSharedParameterID parameter_id;
            switch (event.knob.knob_id) {
                case INPUTS_SHARED_KNOB_ID_FRONT_LEFT:
                    parameter_id = INPUTS_SHARED_PARAMETER_ID_POWER;
                    break;
                case INPUTS_SHARED_KNOB_ID_FRONT_RIGHT:
                    parameter_id = INPUTS_SHARED_PARAMETER_ID_REGEN;
                    break;
                case INPUTS_SHARED_KNOB_ID_SIDE_LEFT:
                    parameter_id = INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING;
                    break;
                default:
                    return INPUTS_RC_OK;
            }
            uint8_t value = prv_parameters_adjust_numeric(parameter_id, event.knob.delta);
            return prv_parameters_notify(parameter_id, value);
        }
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS: {
            enum InputsSharedParameterID parameter_id;
            switch (event.button.button_id) {
                case INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT:
                    parameter_id = INPUTS_SHARED_PARAMETER_ID_TRACTION_CONTROL;
                    break;
                case INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT:
                    parameter_id = INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL;
                    break;
                default:
                    return INPUTS_RC_OK;
            }
            uint8_t value = prv_parameters_toggle(parameter_id);
            return prv_parameters_notify(parameter_id, value);
        }
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE:
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS:
        case INPUTS_SHARED_EVENT_TYPE_PARAMETER_CHANGE:
        default:
            return INPUTS_RC_OK;
    }
}

uint8_t parameters_api_get_value(enum InputsSharedParameterID parameter_id) {
    if (parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return 0U;
    }
    return handler.values[parameter_id];
}
