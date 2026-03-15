/*!
 * \file input-events.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic definitions for the steering wheel input events handling.
 */

#ifndef INPUT_EVENTS_H
#define INPUT_EVENTS_H

#include "inputs-shared.h"
#include <stdint.h>

/*!
 * \brief Return codes for input event callbacks.
 */
enum InputEventsReturnCode {
    INPUT_EVENTS_RC_OK,    /*!< Event handled successfully. */
    INPUT_EVENTS_RC_ERROR, /*!< An error occurred while handling the event. */
};

/*!
 * \brief Callback type for button press events.
 *
 * \param button_id The ID of the button that was pressed.
 *
 * \return An InputEventsReturnCode indicating success or failure.
 */
typedef enum InputEventsReturnCode (*input_events_button_event_callback)(enum InputsSharedButtonID button_id);

/*!
 * \brief Callback type for knob rotation events.
 *
 * \param knob_id The ID of the knob that was rotated.
 * \param delta The amount of rotation; positive for clockwise, negative for counter-clockwise.
 *
 * \return An InputEventsReturnCode indicating success or failure.
 */
typedef enum InputEventsReturnCode (*input_events_knob_rotation_callback)(enum InputsSharedKnobID knob_id, int8_t delta);

/*!
 * \brief Structure holding all input event callbacks.
 */
struct InputEventHandler {
    input_events_button_event_callback on_button_event;   /*!< Callback for button press events */
    input_events_knob_rotation_callback on_knob_rotation; /*!< Callback for knob rotation events */
};

#endif // INPUT_EVENTS_H
