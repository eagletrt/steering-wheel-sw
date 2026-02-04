/*!
 * \file input-events.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input event callback definitions for the steering wheel.
 */

#ifndef EVENTS_H
#define EVENTS_H

#include "shared.h"
#include <stdint.h>

/*!
 * \brief Return codes for input event callbacks.
 */
enum InputEventReturnCode {
    INPUT_EVENT_OK,    /*!< Event handled successfully. */
    INPUT_EVENT_ERROR, /*!< An error occurred while handling the event. */
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

/*!
 * \brief Initialize the input event handler with provided callbacks.
 *
 * \param handler Pointer to the InputEventHandler structure to initialize.
 * \param press_cb Callback for button press events.
 * \param release_cb Callback for button release events.
 * \param long_press_cb Callback for button long press events.
 * \param rotation_cb Callback for knob rotation events.
 *
 * \retval INPUT_EVENT_OK if initialization was successful.
 * \retval INPUT_EVENT_ERROR if there was an error during initialization.
 */
enum InputEventReturnCode input_events_init(
    struct InputEventHandler *handler,
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
enum InputEventReturnCode input_events_handle_event(
    struct InputEventHandler *handler,
    struct InputEvent *event);

#endif // EVENTS_H
