#include "input-events-driver.h"
#include "inputs-shared.h"

struct InputEventHandler input_event_handler = { 0 };
struct InputEvent shared_data
    __attribute__((section(".shared_axi"), aligned(32)));

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
