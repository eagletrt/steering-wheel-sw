/*!
 * \file inputs.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling implementation for the steering wheel.
 */

// needs it just because of memset
#include <string.h>

#include "inputs.h"
#include "eagletrt.h"

/*!
 * \brief Dispatch an event to both the notify and action callbacks.
 *
 * The notify callback is always called first.
 * If either fails the first non-OK code is returned, but both
 * are always attempted.
 */
EAGLETRT_STATIC enum InputsReturnCode inputs_dispatch(
    const struct InputHandler *handler,
    struct InputEvent event) {
    enum InputsReturnCode notify_rc = handler->notify_callback(event);
    enum InputsReturnCode action_rc = handler->action_callback(event);
    if (notify_rc == INPUTS_RC_OK && action_rc != INPUTS_RC_OK) {
        return action_rc;
    }
    return notify_rc;
}

enum InputsReturnCode inputs_init(
    struct InputHandler *handler,
    input_event_notify_callback notify_callback,
    input_event_notify_callback action_callback) {
    if (handler == NULL || notify_callback == NULL || action_callback == NULL) {
        return INPUTS_RC_ERROR;
    }

    memset(handler, 0, sizeof(*handler));

    handler->notify_callback = notify_callback;
    handler->action_callback = action_callback;

    for (size_t i = 0; i < BUTTON_ID_COUNT; i++) {
        handler->buttons[i].enabled = true;
        handler->buttons[i].state = BUTTON_STATE_IDLE;
    }

    for (size_t i = 0; i < KNOB_ID_COUNT; i++) {
        handler->knobs[i].enabled = true;
        handler->knobs[i].last_position = 0;
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_update_button(
    struct InputHandler *handler,
    enum ButtonID button_id,
    bool pressed,
    uint32_t current_tick_ms) {
    if (handler == NULL || button_id < 0 || button_id >= BUTTON_ID_COUNT) {
        return INPUTS_RC_ERROR;
    }

    struct ButtonHandler *btn = &handler->buttons[button_id];
    if (!btn->enabled) {
        return INPUTS_RC_OK;
    }

    struct InputEvent event = {
        .button.button_id = button_id
    };

    if (pressed) {
        if (btn->state == BUTTON_STATE_IDLE) {
            btn->state = BUTTON_STATE_PRESSED;
            btn->press_tick = current_tick_ms;

            event.type = INPUT_EVENT_TYPE_BUTTON_PRESS;
            return inputs_dispatch(handler, event);
        }
    } else if (btn->state == BUTTON_STATE_PRESSED || btn->state == BUTTON_STATE_LONG_PRESSED) {
        btn->state = BUTTON_STATE_IDLE;

        event.type = INPUT_EVENT_TYPE_BUTTON_RELEASE;
        return inputs_dispatch(handler, event);
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_update_knob(
    struct InputHandler *handler,
    enum KnobID knob_id,
    int16_t current_position) {
    if (handler == NULL || knob_id < 0 || knob_id >= KNOB_ID_COUNT) {
        return INPUTS_RC_ERROR;
    }

    struct KnobHandler *knob = &handler->knobs[knob_id];
    if (!knob->enabled) {
        return INPUTS_RC_OK;
    }

    int16_t delta = current_position - knob->last_position;

    knob->last_position = current_position;

    if (delta != 0) {
        struct InputEvent event = {
            .type = INPUT_EVENT_TYPE_KNOB,
            .knob.knob_id = knob_id,
            .knob.delta = (int8_t)delta
        };

        return inputs_dispatch(handler, event);
    }

    return INPUTS_RC_OK;
}

enum InputsReturnCode inputs_poll_for_long_press(struct InputHandler *handler, uint32_t current_tick_ms) {
    if (handler == NULL) {
        return INPUTS_RC_ERROR;
    }

    for (size_t i = 0; i < BUTTON_ID_COUNT; i++) {
        struct ButtonHandler *btn = &handler->buttons[i];

        if (!btn->enabled) {
            continue;
        }

        if (btn->state == BUTTON_STATE_PRESSED && current_tick_ms - btn->press_tick >= INPUTS_LONG_PRESS_THRESHOLD_MS) {
            btn->state = BUTTON_STATE_LONG_PRESSED;

            struct InputEvent event = {
                .type = INPUT_EVENT_TYPE_BUTTON_LONG_PRESS,
                .button.button_id = i
            };

            enum InputsReturnCode ret = inputs_dispatch(handler, event);
            if (ret != INPUTS_RC_OK) {
                return ret;
            }
        }

        if (btn->state == BUTTON_STATE_IDLE) {
            btn->press_tick = 0;
        }
    }

    return INPUTS_RC_OK;
}
