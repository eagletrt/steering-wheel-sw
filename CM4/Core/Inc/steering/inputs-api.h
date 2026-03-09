/*!
 * \file inputs-api.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling function definitions for the steering wheel.
 */

#ifndef INPUTS_API_H
#define INPUTS_API_H

#include "inputs.h"

/*!
 * \brief Initialize the input handler
 *
 * \param handler Pointer to the input handler structure to initialize
 * \param notify_callback Callback function to notify CM7 about input events
 * \param action_callback Callback for local actions on input
 *
 * \retval INPUTS_RC_OK if initialization was successful
 * \retval INPUTS_RC_ERROR if there was an error during initialization
 */
enum InputsReturnCode inputs_init(
    struct InputHandler *handler,
    input_event_notify_callback notify_callback,
    input_event_notify_callback action_callback);

/*!
 * \brief Change state of a button
 *
 * \param handler Pointer to the input handler structure
 * \param button_id Identifier of the button that changed state
 * \param pressed True if the button is now pressed, false if released
 * \param current_tick_ms Current tick count in milliseconds (used for long press tracking)
 *
 * \retval INPUTS_OK if the button state was updated successfully
 * \retval INPUTS_ERROR if there was an error updating the button state
 */
enum InputsReturnCode inputs_update_button(
    struct InputHandler *handler,
    enum ButtonID button_id,
    bool pressed,
    uint32_t current_tick_ms);

/*!
 * \brief Change state of a knob
 *
 * \param handler Pointer to the input handler structure
 * \param knob_id Identifier of the knob that changed state
 * \param current_position Current position of the encoder
 *
 * \retval INPUTS_OK if the knob state was updated successfully
 * \retval INPUTS_ERROR if there was an error updating the knob state
 */
enum InputsReturnCode inputs_update_knob(
    struct InputHandler *handler,
    enum KnobID knob_id,
    int16_t current_position);

/*!
 * \brief Update function to be called periodically to handle long presses
 *
 * \param handler Pointer to the input handler structure
 * \param current_tick_ms Current tick count in milliseconds
 *
 * \retval INPUTS_OK if the update was successful
 * \retval INPUTS_ERROR if there was an error during the update
 */
enum InputsReturnCode inputs_poll_for_long_press(
    struct InputHandler *handler,
    uint32_t current_tick_ms);

#endif // INPUTS_API_H
