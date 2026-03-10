/*!
 * \file input-events-api.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic API to handle input events.
 */

#include "input-events-api.h"
#include "eagletrt.h"
#include <stddef.h>

EAGLETRT_STATIC struct InputEventHandler handler;

enum InputEventsReturnCode input_events_api_init(input_events_button_press_callback press_cb, input_events_button_release_callback release_cb, input_events_button_long_press_callback long_press_cb, input_events_knob_rotation_callback rotation_cb) {
    handler.on_button_press = press_cb;
    handler.on_button_release = release_cb;
    handler.on_button_long_press = long_press_cb;
    handler.on_knob_rotation = rotation_cb;

    return INPUT_EVENTS_RC_OK;
}

void input_events_api_handle_event(struct InputsSharedEvent *event) {
    if (event == NULL) {
        return;
    }

    switch (event->type) {
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS:
            if (handler.on_button_press != NULL) {
                handler.on_button_press(event->button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE:
            if (handler.on_button_release != NULL) {
                handler.on_button_release(event->button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS:
            if (handler.on_button_long_press != NULL) {
                handler.on_button_long_press(event->button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION:
            if (handler.on_knob_rotation != NULL) {
                handler.on_knob_rotation(event->knob.knob_id, event->knob.delta);
            }
            break;
    }
}
