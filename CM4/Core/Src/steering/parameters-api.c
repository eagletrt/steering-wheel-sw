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
#include "leds-api.h"
#include "eagletrt.h"
#include "eagletrt-api.h"

EAGLETRT_STATIC struct ParametersHandler handler;

/*!
 * \brief Static descriptor for each parameter.
 */
struct ParameterMeta {
    bool is_toggle; /*!< true for ON/OFF parameters, false for 0..NUMERIC_MAX */
    bool is_shared; /*!< true if the parameter is broadcast to CM7 */
};

EAGLETRT_STATIC const struct ParameterMeta prv_meta[INPUTS_SHARED_PARAMETER_ID_COUNT] = {
    [INPUTS_SHARED_PARAMETER_ID_POWER] = { .is_toggle = false, .is_shared = true },
    [INPUTS_SHARED_PARAMETER_ID_REGEN] = { .is_toggle = false, .is_shared = true },
    [INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING] = { .is_toggle = false, .is_shared = true },
    [INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = { .is_toggle = true, .is_shared = true },
    [INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL] = { .is_toggle = true, .is_shared = true },
    [INPUTS_SHARED_PARAMETER_ID_PTT] = { .is_toggle = true, .is_shared = false },
};

/*!
 * \brief Clamp a signed candidate value to the parameter's valid range.
 */
EAGLETRT_STATIC uint8_t prv_clamp(
    enum InputsSharedParameterID parameter_id,
    int16_t candidate) {
    int16_t max = prv_meta[parameter_id].is_toggle ? 1 : (int16_t)INPUTS_SHARED_PARAMETER_NUMERIC_MAX;
    int16_t clamped = EAGLETRT_API_CLAMP(candidate, 0, max);
    return (uint8_t)clamped;
}

/*!
 * \brief Run the local hardware side effect for a parameter that has one.
 *
 * \details PTT is the only parameter wired to local hardware today: it
 *     overlays the strip with the PTT pattern on activation and restores
 *     whatever was on it before on deactivation.
 */
EAGLETRT_STATIC void prv_apply_local_effect(
    enum InputsSharedParameterID parameter_id,
    uint8_t new_value) {

    switch (parameter_id) {
        case INPUTS_SHARED_PARAMETER_ID_PTT: {
            if (new_value) {
                leds_api_save_pattern();
                leds_api_set_ptt_pattern();
            } else {
                leds_api_restore_pattern();
            }
            leds_api_show();
        }
        default:
            break;
    }
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
    prv_apply_local_effect(parameter_id, new_value);
    if (!handler.on_change(parameter_id, new_value)) {
        return PARAMETERS_RC_ERROR;
    }
    return PARAMETERS_RC_OK;
}

/*!
 * \brief Recompute the PTT toggle from the current paddle hold flags.
 *
 * \details PTT is active while either top paddle is held; it only goes
 *     back to 0 once both are released.
 */
EAGLETRT_STATIC enum ParametersReturnCode prv_recompute_ptt(void) {
    uint8_t desired = (handler.ptt_top_left_held || handler.ptt_top_right_held) ? 1U : 0U;
    return prv_apply(INPUTS_SHARED_PARAMETER_ID_PTT, desired);
}

enum ParametersReturnCode parameters_api_init(parameters_on_change_callback on_change) {
    if (on_change == NULL) {
        return PARAMETERS_RC_ERROR;
    }

    memset(&handler, 0, sizeof(handler));
    handler.on_change = on_change;

    return PARAMETERS_RC_OK;
}

bool parameters_api_is_shared(enum InputsSharedParameterID parameter_id) {
    if (parameter_id >= INPUTS_SHARED_PARAMETER_ID_COUNT) {
        return false;
    }
    return prv_meta[parameter_id].is_shared;
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
    switch (button_id) {
        case INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT: {
            uint8_t next = handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] ? 0U : 1U;
            return prv_apply(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, next) == PARAMETERS_RC_OK
                       ? INPUTS_RC_OK
                       : INPUTS_RC_ERROR;
        }
        case INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT: {
            uint8_t next = handler.values[INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL] ? 0U : 1U;
            return prv_apply(INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL, next) == PARAMETERS_RC_OK
                       ? INPUTS_RC_OK
                       : INPUTS_RC_ERROR;
        }
        case INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT:
            handler.ptt_top_left_held = true;
            return prv_recompute_ptt() == PARAMETERS_RC_OK ? INPUTS_RC_OK : INPUTS_RC_ERROR;
        case INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT:
            handler.ptt_top_right_held = true;
            return prv_recompute_ptt() == PARAMETERS_RC_OK ? INPUTS_RC_OK : INPUTS_RC_ERROR;
        default:
            return INPUTS_RC_OK;
    }
}

enum InputsReturnCode parameters_api_handle_button_release(enum InputsSharedButtonID button_id) {
    switch (button_id) {
        case INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT:
            handler.ptt_top_left_held = false;
            return prv_recompute_ptt() == PARAMETERS_RC_OK ? INPUTS_RC_OK : INPUTS_RC_ERROR;
        case INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT:
            handler.ptt_top_right_held = false;
            return prv_recompute_ptt() == PARAMETERS_RC_OK ? INPUTS_RC_OK : INPUTS_RC_ERROR;
        default:
            return INPUTS_RC_OK;
    }
}

enum InputsReturnCode parameters_api_handle_knob(
    enum InputsSharedKnobID knob_id,
    int8_t delta) {
    enum InputsSharedParameterID parameter_id;
    switch (knob_id) {
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
    int16_t candidate = (int16_t)handler.values[parameter_id] + (int16_t)delta;
    if (prv_apply(parameter_id, prv_clamp(parameter_id, candidate)) != PARAMETERS_RC_OK) {
        return INPUTS_RC_ERROR;
    }
    return INPUTS_RC_OK;
}
