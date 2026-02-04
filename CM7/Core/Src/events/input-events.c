#include "input-events.h"
#include <stddef.h>

enum InputEventReturnCode input_events_init(struct InputEventHandler *handler, input_event_button_press_callback press_cb, input_event_button_release_callback release_cb, input_event_button_long_press_callback long_press_cb, input_event_knob_rotation_callback rotation_cb) {
    if (handler == NULL) {
        return INPUT_EVENT_ERROR;
    }

    handler->on_button_press = press_cb;
    handler->on_button_release = release_cb;
    handler->on_button_long_press = long_press_cb;
    handler->on_knob_rotation = rotation_cb;

    return INPUT_EVENT_OK;
}

enum InputEventReturnCode input_events_handle_event(struct InputEventHandler *handler, struct InputEvent *event) {
    if (handler == NULL || event == NULL) {
        return INPUT_EVENT_ERROR;
    }

    switch (event->type) {
        case INPUT_EVENT_TYPE_BUTTON_PRESS:
            if (handler->on_button_press) {
                handler->on_button_press(event->button.button_id);
            }
            break;
        case INPUT_EVENT_TYPE_BUTTON_RELEASE:
            if (handler->on_button_release) {
                handler->on_button_release(event->button.button_id);
            }
            break;
        case INPUT_EVENT_TYPE_BUTTON_LONG_PRESS:
            if (handler->on_button_long_press) {
                handler->on_button_long_press(event->button.button_id);
            }
            break;
        case INPUT_EVENT_TYPE_KNOB:
            if (handler->on_knob_rotation) {
                handler->on_knob_rotation(event->knob.knob_id, event->knob.delta);
            }
            break;
        default:
            return INPUT_EVENT_ERROR;
    }

    return INPUT_EVENT_OK;
}
