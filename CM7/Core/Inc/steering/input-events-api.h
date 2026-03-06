#ifndef INPUT_EVENTS_API_H
#define INPUT_EVENTS_API_H

#include "input-events.h"

/*!
 * \brief Initialize the input event handler with provided callbacks.
 *
 * \param press_cb Callback for button press events.
 * \param release_cb Callback for button release events.
 * \param long_press_cb Callback for button long press events.
 * \param rotation_cb Callback for knob rotation events.
 *
 * \retval INPUT_EVENT_OK if initialization was successful.
 * \retval INPUT_EVENT_ERROR if there was an error during initialization.
 */
enum InputEventReturnCode input_events_api_init(
    input_event_button_press_callback press_cb,
    input_event_button_release_callback release_cb,
    input_event_button_long_press_callback long_press_cb,
    input_event_knob_rotation_callback rotation_cb);

/*!
 * \brief Handle an input event by invoking the appropriate callback.
 *
 * \param handler Pointer to the InputEventHandler structure.
 * \param event The input event to handle.
 *
 * \retval INPUT_EVENT_OK if the event was handled successfully.
 * \retval INPUT_EVENT_ERROR if there was an error handling the event.
 */
void input_events_api_handle_event(
    struct InputEvent *event);

#endif // INPUT_EVENTS_API_H
