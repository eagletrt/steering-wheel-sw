/*!
 * \file input-events.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input event callback definitions for the steering wheel.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include "inputs-shared.h"
#include <stdint.h>

/*!
 * \brief Return codes for input event callbacks.
 */
enum InputEventReturnCode {
    INPUT_EVENT_RC_OK,    /*!< Event handled successfully. */
    INPUT_EVENT_RC_ERROR, /*!< An error occurred while handling the event. */
};

/*!
 * \brief Callback type for button press events.
 *
 * \param button_id The ID of the button that was pressed.
 *
 * \return An InputEventReturnCode indicating success or failure.
 */
typedef enum InputEventReturnCode (*input_event_button_press_callback)(enum ButtonID button_id);

/*!
 * \brief Callback type for button release events.
 *
 * \param button_id The ID of the button that was released.
 *
 * \return An InputEventReturnCode indicating success or failure.
 */
typedef enum InputEventReturnCode (*input_event_button_release_callback)(enum ButtonID button_id);

/*!
 * \brief Callback type for button long press events.
 *
 * \param button_id The ID of the button that was long pressed.
 *
 * \return An InputEventReturnCode indicating success or failure.
 */
typedef enum InputEventReturnCode (*input_event_button_long_press_callback)(enum ButtonID button_id);

/*!
 * \brief Callback type for knob rotation events.
 *
 * \param knob_id The ID of the knob that was rotated.
 * \param delta The amount of rotation; positive for clockwise, negative for counter-clockwise.
 *
 * \return An InputEventReturnCode indicating success or failure.
 */
typedef enum InputEventReturnCode (*input_event_knob_rotation_callback)(enum KnobID knob_id, int8_t delta);

/*!
 * \brief Structure holding all input event callbacks.
 */
struct InputEventHandler {
    input_event_button_press_callback on_button_press;           /*!< Callback for button press events */
    input_event_button_release_callback on_button_release;       /*!< Callback for button release events */
    input_event_button_long_press_callback on_button_long_press; /*!< Callback for button long press events */
    input_event_knob_rotation_callback on_knob_rotation;         /*!< Callback for knob rotation events */
};

#endif // EVENTS_H
