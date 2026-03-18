/*!
 * \file inputs-api.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling implementation for the steering wheel.
 */

// needs it just because of memset
#include <string.h>

#include "inputs-api.h"
#include "eagletrt.h"

EAGLETRT_STATIC struct InputsHandler handler;

/*!
 * \brief Dispatch an event to both the notify and action callbacks.
 *
 * The notify callback is always called first.
 * If either fails the first non-OK code is returned, but both
 * are always attempted.
 */
EAGLETRT_STATIC enum InputsReturnCode prv_inputs_dispatch(
    struct InputsSharedEvent event) {
    bool notify_rc = handler.notify_callback(event, handler.critical_section_callback);
    enum InputsReturnCode action_rc = handler.action_callback(event);
    if (!notify_rc) {
        return INPUTS_RC_NOTIFY_ERROR;
    }
    return action_rc;
}

enum InputsReturnCode inputs_api_init(
    void (*critical_section_callback)(void),
    inputs_notify_callback notify_callback,
    inputs_action_callback action_callback) {
    if (notify_callback == NULL || action_callback == NULL) {
        return INPUTS_RC_ERROR;
    }

    memset(&handler, 0, sizeof(handler));

    handler.notify_callback = notify_callback;
    handler.action_callback = action_callback;
    handler.critical_section_callback = critical_section_callback;

    for (size_t i = 0; i < INPUTS_SHARED_BUTTON_ID_COUNT; i++) {
        handler.buttons[i].enabled = true;
        handler.buttons[i].state = INPUTS_BUTTON_STATE_IDLE;
    }

    for (size_t i = 0; i < INPUTS_SHARED_KNOB_ID_COUNT; i++) {
        handler.knobs[i].enabled = true;
        handler.knobs[i].last_position = 0;
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_api_update_button(
    enum InputsSharedButtonID button_id,
    bool pressed,
    uint32_t current_tick_ms) {
    if (button_id < 0 || button_id >= INPUTS_SHARED_BUTTON_ID_COUNT) {
        return INPUTS_RC_ERROR;
    }

    struct InputsButtonHandler *btn = &handler.buttons[button_id];
    if (!btn->enabled) {
        return INPUTS_RC_OK;
    }

    struct InputsSharedEvent event = {
        .button.button_id = button_id
    };

    if (pressed) {
        if (btn->state == INPUTS_BUTTON_STATE_IDLE) {
            btn->state = INPUTS_BUTTON_STATE_PRESSED;
            btn->press_tick = current_tick_ms;

            event.type = INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS;
            return prv_inputs_dispatch(event);
        }
    } else if (btn->state == INPUTS_BUTTON_STATE_PRESSED || btn->state == INPUTS_BUTTON_STATE_LONG_PRESSED) {
        btn->state = INPUTS_BUTTON_STATE_IDLE;

        event.type = INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE;
        return prv_inputs_dispatch(event);
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_api_update_knob(
    enum InputsSharedKnobID knob_id,
    int16_t current_position) {
    if (knob_id < 0 || knob_id >= INPUTS_SHARED_KNOB_ID_COUNT) {
        return INPUTS_RC_ERROR;
    }

    struct InputsKnobHandler *knob = &handler.knobs[knob_id];
    if (!knob->enabled) {
        return INPUTS_RC_OK;
    }

    int16_t delta = current_position - knob->last_position;

    knob->last_position = current_position;

    if (delta != 0) {
        struct InputsSharedEvent event = {
            .type = INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION,
            .knob.knob_id = knob_id,
            .knob.delta = (int8_t)delta
        };

        return prv_inputs_dispatch(event);
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_api_poll_for_long_press(uint32_t current_tick_ms) {
    for (size_t i = 0; i < INPUTS_SHARED_BUTTON_ID_COUNT; i++) {
        struct InputsButtonHandler *btn = &handler.buttons[i];

        if (!btn->enabled) {
            continue;
        }

        if (btn->state == INPUTS_BUTTON_STATE_PRESSED && current_tick_ms - btn->press_tick >= INPUTS_LONG_PRESS_THRESHOLD_MS) {
            btn->state = INPUTS_BUTTON_STATE_LONG_PRESSED;

            struct InputsSharedEvent event = {
                .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
                .button.button_id = i
            };

            enum InputsReturnCode ret = prv_inputs_dispatch(event);
            if (ret != INPUTS_RC_OK) {
                return ret;
            }
        }

        if (btn->state == INPUTS_BUTTON_STATE_IDLE) {
            btn->press_tick = 0;
        }
    }

    return INPUTS_RC_OK;
}
