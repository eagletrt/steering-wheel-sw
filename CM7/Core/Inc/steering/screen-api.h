#ifndef INPUTS_EVENTS_DRIVER_H
#define INPUTS_EVENTS_DRIVER_H

#include "input-events.h"
#include "screen.h"

enum InputEventsReturnCode mock_input_event_button_event_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_button_long_press_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_button_release_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_knob_rotation_callback(enum InputsSharedKnobID knob_id, int8_t delta);

enum ScreenReturnCode screen_init(font_draw_line_callback draw_line, raster_draw_rectangle_callback draw_rectangle);
enum ScreenReturnCode screen_update(uint32_t tick);

#endif // INPUTS_EVENTS_DRIVER_H
