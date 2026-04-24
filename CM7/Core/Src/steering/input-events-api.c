/*!
 * \file input-events-api.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Hardware-agnostic API to handle input events.
 */

#include "input-events-api.h"
#include "eagletrt.h"
#include <stddef.h>

EAGLETRT_STATIC struct InputEventHandler handler;

enum InputEventsReturnCode input_events_api_init(input_events_button_event_callback button_press_cb, input_events_button_event_callback button_long_press_cb, input_events_button_event_callback button_release_cb, input_events_knob_rotation_callback rotation_cb, input_events_parameter_change_callback parameter_cb) {
    if (button_press_cb == NULL || button_long_press_cb == NULL || button_release_cb == NULL || rotation_cb == NULL || parameter_cb == NULL) {
        return INPUT_EVENTS_RC_ERROR;
    }
    handler.on_button_press = button_press_cb;
    handler.on_button_long_press = button_long_press_cb;
    handler.on_button_release = button_release_cb;
    handler.on_knob_rotation = rotation_cb;
    handler.on_parameter_change = parameter_cb;

    return INPUT_EVENTS_RC_OK;
}

void input_events_api_handle_event(struct InputsSharedEvent event) {
    switch (event.type) {
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS:
            if (handler.on_button_press != NULL) {
                handler.on_button_press(event.button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS:
            if (handler.on_button_long_press != NULL) {
                handler.on_button_long_press(event.button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE:
            if (handler.on_button_release != NULL) {
                handler.on_button_release(event.button.button_id);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION:
            if (handler.on_knob_rotation != NULL) {
                handler.on_knob_rotation(event.knob.knob_id, event.knob.delta);
            }
            break;
        case INPUTS_SHARED_EVENT_TYPE_PARAMETER_CHANGE:
            if (handler.on_parameter_change != NULL) {
                handler.on_parameter_change(event.parameter.parameter_id, event.parameter.value);
            }
            break;
    }
}
