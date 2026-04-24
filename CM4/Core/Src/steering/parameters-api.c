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
#include "inputs-shared.h"

EAGLETRT_STATIC struct ParametersHandler handler;

/*!
 * \brief Whether a parameter is boolean (0/1) rather than numeric (0..MAX).
 */
EAGLETRT_STATIC bool prv_is_toggle(enum InputsSharedParameterID parameter_id) {
    return parameter_id == INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG ||
           parameter_id == INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL;
}

/*!
 * \brief Clamp a signed candidate value to the parameter's valid range.
 */
EAGLETRT_STATIC uint8_t prv_clamp(
    enum InputsSharedParameterID parameter_id,
    int16_t candidate) {
    int16_t max = prv_is_toggle(parameter_id) ? 1 : (int16_t)INPUTS_SHARED_PARAMETER_NUMERIC_MAX;
    int16_t clamped = EAGLETRT_API_CLAMP(candidate, 0, max);
    return (uint8_t)clamped;
}

/*!
 * \brief Apply an already-clamped value and notify the caller if it changed.
 *
 * \retval PARAMETERS_RC_OK if the value is unchanged or the on-change callback succeeded.
 * \retval PARAMETERS_RC_ERROR if the on-change callback returned false.
 */
EAGLETRT_STATIC enum ParametersReturnCode prv_apply(
    enum InputsSharedParameterID parameter_id,
    uint8_t new_value) {
    if (handler.values[parameter_id] == new_value) {
        return PARAMETERS_RC_OK;
    }
    handler.values[parameter_id] = new_value;
    if (!handler.on_change(parameter_id, new_value)) {
        return PARAMETERS_RC_ERROR;
    }
    return PARAMETERS_RC_OK;
}

enum ParametersReturnCode parameters_api_init(parameters_on_change_callback on_change) {
    if (on_change == NULL) {
        return PARAMETERS_RC_ERROR;
    }

    memset(&handler, 0, sizeof(handler));
    handler.on_change = on_change;

    return PARAMETERS_RC_OK;
}

uint8_t parameters_api_get(enum InputsSharedParameterID parameter_id) {
    if (parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return 0U;
    }
    return handler.values[parameter_id];
}

enum ParametersReturnCode parameters_api_set(
    enum InputsSharedParameterID parameter_id,
    uint8_t value) {
    if (parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return PARAMETERS_RC_ERROR;
    }
    return prv_apply(parameter_id, prv_clamp(parameter_id, (int16_t)value));
}

enum InputsReturnCode parameters_api_handle_button(enum InputsSharedButtonID button_id) {
    enum InputsSharedParameterID parameter_id;
    switch (button_id) {
        case INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT:
            parameter_id = INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG;
            break;
        case INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT:
            parameter_id = INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL;
            break;
        default:
            return INPUTS_RC_OK;
    }
    uint8_t next = handler.values[parameter_id] ? 0U : 1U;
    if (prv_apply(parameter_id, next) != PARAMETERS_RC_OK) {
        return INPUTS_RC_ERROR;
    }
    return INPUTS_RC_OK;
}

enum InputsReturnCode parameters_api_handle_knob(
    enum InputsSharedKnobID knob_id,
    int8_t delta) {
    enum InputsSharedParameterID parameter_id;
    switch (knob_id) {
        case INPUTS_SHARED_KNOB_ID_FRONT_RIGHT:
            parameter_id = INPUTS_SHARED_PARAMETER_ID_POWER;
            break;
        case INPUTS_SHARED_KNOB_ID_SIDE_RIGHT:
            parameter_id = INPUTS_SHARED_PARAMETER_ID_REGEN;
            break;
        case INPUTS_SHARED_KNOB_ID_SIDE_LEFT:
            parameter_id = INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING;
            break;
        default:
            return INPUTS_RC_OK;
    }
    int16_t candidate = (int16_t)handler.values[parameter_id] + (int16_t)delta;
    if (prv_apply(parameter_id, prv_clamp(parameter_id, candidate)) != PARAMETERS_RC_OK) {
        return INPUTS_RC_ERROR;
    }
    return INPUTS_RC_OK;
}
