/*!
 * \file inputs.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling implementation for the steering wheel.
 */

#include "inputs.h"
#include <string.h>

enum InputsReturnCode inputs_init(
    struct InputHandler *handler,
    input_event_notify_callback callback) {
    if (!handler || !callback) {
        return INPUTS_ERROR;
    }

    memset(handler, 0, sizeof(struct InputHandler));

    handler->notify_callback = callback;

    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        handler->buttons[i].enabled = true;
        handler->buttons[i].state = BUTTON_STATE_IDLE;
    }

    for (uint8_t i = 0; i < KNOB_COUNT; i++) {
        handler->knobs[i].enabled = true;
        handler->knobs[i].last_position = 0;
    }

    return INPUTS_OK;
}

enum InputsReturnCode inputs_on_button_interrupt(
    struct InputHandler *handler,
    enum ButtonID button_id,
    bool pressed,
    uint32_t current_tick_ms) {
    if (!handler || button_id < 0 || button_id >= BUTTON_COUNT) {
        return INPUTS_ERROR;
    }

    struct ButtonTracker *btn = &handler->buttons[button_id];
    if (!btn->enabled) {
        return INPUTS_OK;
    }

    struct InputEvent event;
    event.button.button_id = button_id;

    if (pressed) {
        if (btn->state == BUTTON_STATE_IDLE) {
            btn->state = BUTTON_STATE_PRESSED;
            btn->press_tick = current_tick_ms;

            event.type = INPUT_EVENT_TYPE_BUTTON_PRESS;
            return handler->notify_callback(event);
        }
    } else if (btn->state == BUTTON_STATE_PRESSED || btn->state == BUTTON_STATE_LONG_PRESSED) {
        btn->state = BUTTON_STATE_IDLE;

        event.type = INPUT_EVENT_TYPE_BUTTON_RELEASE;
        return handler->notify_callback(event);
    }

    return INPUTS_OK;
}

enum InputsReturnCode inputs_check_knobs(
    struct InputHandler *handler,
    enum KnobID knob_id,
    int16_t current_position) {
    if (!handler || knob_id < 0 || knob_id >= KNOB_COUNT) {
        return INPUTS_ERROR;
    }

    struct KnobTracker *knob = &handler->knobs[knob_id];
    if (!knob->enabled) {
        return INPUTS_OK;
    }

    int16_t delta = current_position - knob->last_position;

    knob->last_position = current_position;

    if (delta != 0) {
        struct InputEvent event;
        event.type = INPUT_EVENT_TYPE_KNOB;
        event.knob.knob_id = knob_id;
        event.knob.delta = (int8_t)delta;

        return handler->notify_callback(event);
    }

    return INPUTS_OK;
}

enum InputsReturnCode inputs_update(struct InputHandler *handler, uint32_t current_tick_ms) {
    if (!handler) {
        return INPUTS_ERROR;
    }

    for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
        struct ButtonTracker *btn = &handler->buttons[i];

        if (!btn->enabled) {
            continue;
        }

        if (btn->state == BUTTON_STATE_PRESSED && current_tick_ms - btn->press_tick >= LONG_PRESS_THRESHOLD_MS) {
            btn->state = BUTTON_STATE_LONG_PRESSED;

            struct InputEvent event;
            event.type = INPUT_EVENT_TYPE_BUTTON_LONG_PRESS;
            event.button.button_id = i;

            enum InputsReturnCode ret = handler->notify_callback(event);
            if (ret != INPUTS_OK) {
                return ret;
            }
        }

        if (btn->state == BUTTON_STATE_IDLE) {
            btn->press_tick = 0;
        }
    }

    return INPUTS_OK;
}
