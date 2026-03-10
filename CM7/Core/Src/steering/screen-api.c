#include "screen-api.h"
#include "inputs-shared.h"

enum InputEventsReturnCode mock_input_event_button_press_callback(enum InputsSharedButtonID button_id) {
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_button_release_callback(enum InputsSharedButtonID button_id) {
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_button_long_press_callback(enum InputsSharedButtonID button_id) {
    return INPUT_EVENTS_RC_OK;
}

enum InputEventsReturnCode mock_input_event_knob_rotation_callback(enum InputsSharedKnobID knob_id, int8_t delta) {
    return INPUT_EVENTS_RC_OK;
}
