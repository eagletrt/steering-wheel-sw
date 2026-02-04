#ifndef INPUTS_EVENTS_DRIVER_H
#define INPUTS_EVENTS_DRIVER_H

#include "input-events.h"

extern struct InputEventHandler input_event_handler;
extern struct InputEvent shared_data;

enum InputEventReturnCode mock_input_event_button_press_callback(enum ButtonID button_id);
enum InputEventReturnCode mock_input_event_button_release_callback(enum ButtonID button_id);
enum InputEventReturnCode mock_input_event_button_long_press_callback(enum ButtonID button_id);
enum InputEventReturnCode mock_input_event_knob_rotation_callback(enum KnobID knob_id, int8_t delta);

#endif // INPUTS_EVENTS_DRIVER_H
