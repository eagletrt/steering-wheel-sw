#include "ui-api.h"
#include "inputs-shared.h"

enum InputEventReturnCode mock_input_event_button_press_callback(enum ButtonID button_id) {
    return INPUT_EVENT_RC_OK;
}

enum InputEventReturnCode mock_input_event_button_release_callback(enum ButtonID button_id) {
    return INPUT_EVENT_RC_OK;
}

enum InputEventReturnCode mock_input_event_button_long_press_callback(enum ButtonID button_id) {
    return INPUT_EVENT_RC_OK;
}

enum InputEventReturnCode mock_input_event_knob_rotation_callback(enum KnobID knob_id, int8_t delta) {
    return INPUT_EVENT_RC_OK;
}
