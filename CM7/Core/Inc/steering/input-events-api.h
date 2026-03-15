/*!
 * \file input-events-api.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic API to handle input events.
 */

#ifndef INPUT_EVENTS_API_H
#define INPUT_EVENTS_API_H

#include "input-events.h"

/*!
 * \brief Initialize the input event handler with provided callbacks.
 *
 * \param button_cb Callback for button events.
 * \param rotation_cb Callback for knob rotation events.
 *
 * \retval INPUT_EVENTS_OK if initialization was successful.
 * \retval INPUT_EVENTS_ERROR if there was an error during initialization.
 */
enum InputEventsReturnCode input_events_api_init(
    input_events_button_event_callback button_cb,
    input_events_knob_rotation_callback rotation_cb);

/*!
 * \brief Handle an input event by invoking the appropriate callback.
 *
 * \param handler Pointer to the InputEventHandler structure.
 * \param event The input event to handle.
 *
 * \retval INPUT_EVENTS_OK if the event was handled successfully.
 * \retval INPUT_EVENTS_ERROR if there was an error handling the event.
 */
void input_events_api_handle_event(
    struct InputsSharedEvent event);

#endif // INPUT_EVENTS_API_H
