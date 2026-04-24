#ifndef INPUTS_EVENTS_DRIVER_H
#define INPUTS_EVENTS_DRIVER_H

#include "input-events.h"
#include "screen.h"

enum InputEventsReturnCode mock_input_event_button_event_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_button_long_press_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_button_release_callback(enum InputsSharedButtonID button_id);
enum InputEventsReturnCode mock_input_event_knob_rotation_callback(enum InputsSharedKnobID knob_id, int8_t delta);

/*!
 * \brief Handle a parameter change event by showing the popup overlay.
 *
 * \details Stamps the current tick, refreshes the popup labels and mounts
 *     the popup interface on the raster. The popup stays visible until
 *     SCREEN_POPUP_TIMEOUT_MS elapses without another change.
 *
 * \param parameter_id The parameter that was changed.
 * \param value The new value of the parameter.
 *
 * \retval INPUT_EVENTS_RC_OK on success.
 * \retval INPUT_EVENTS_RC_ERROR on failure.
 */
enum InputEventsReturnCode screen_on_parameter_change(enum InputsSharedParameterID parameter_id, uint8_t value);

enum ScreenReturnCode screen_init(font_draw_line_callback draw_line, raster_draw_rectangle_callback draw_rectangle);
enum ScreenReturnCode screen_update(uint32_t tick);

#endif // INPUTS_EVENTS_DRIVER_H
