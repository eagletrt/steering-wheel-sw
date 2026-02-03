/*!
 * \file inputs.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling definitions for the steering wheel.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include "shared.h"
#include <stdint.h>
#include <stdbool.h>

#define LONG_PRESS_THRESHOLD_MS 500

/*!
 * \brief Input event types
 */
enum InputsReturnCode {
    INPUTS_OK,    /*!< Operation successful */
    INPUTS_ERROR, /*!< General error */
};

/*!
 * \brief Button states for tracking long presses
 */
enum ButtonState {
    BUTTON_STATE_IDLE,         /*!< Button is not pressed */
    BUTTON_STATE_PRESSED,      /*!< Button is currently pressed */
    BUTTON_STATE_LONG_PRESSED, /*!< Button has been held long enough to be considered a long press */
};

/*!
 * \brief Button tracking structure
 */
struct ButtonTracker {
    enum ButtonState state; /*!< Current state of the button */
    uint32_t press_tick;    /*!< Tick count when the button was pressed */
    bool enabled;           /*!< Whether this button is enabled for tracking */
};

/*!
 * \brief Knob (encoder) tracking structure
 */
struct KnobTracker {
    int16_t last_position; /*!< Last known position of the encoder */
    bool enabled;          /*!< Whether this encoder is enabled for tracking */
};

/*!
 * \brief Callback definition for notifying CM7 about input events
 *
 * \param event The input event to notify about
 * \retval INPUTS_OK if the event was notified successfully
 * \retval INPUTS_ERROR if there was an error handling notify
 */
typedef enum InputsReturnCode (*input_event_notify_callback)(struct InputEvent event);

/*!
 * \brief Main input handler structure
 */
struct InputHandler {
    input_event_notify_callback notify_callback; /*!< Callback to notify CM7 about input events */

    struct ButtonTracker buttons[BUTTON_COUNT]; /*!< Tracking state for each button */
    struct KnobTracker knobs[KNOB_COUNT];       /*!< Tracking state for each encoder */
};

/*!
 * \brief Initialize the input handler
 *
 * \param handler Pointer to the input handler structure to initialize
 * \param callback Callback function to notify CM7 about input events
 *
 * \retval INPUTS_OK if initialization was successful
 * \retval INPUTS_ERROR if there was an error during initialization
 */
enum InputsReturnCode inputs_init(
    struct InputHandler *handler,
    input_event_notify_callback callback);

/*!
 * \brief Change state of a button (called from button interrupt)
 *
 * \param handler Pointer to the input handler structure
 * \param button_id Identifier of the button that changed state
 * \param pressed True if the button is now pressed, false if released
 * \param current_tick_ms Current tick count in milliseconds (used for long press tracking)
 *
 * \retval INPUTS_OK if the button state was updated successfully
 * \retval INPUTS_ERROR if there was an error updating the button state
 */
enum InputsReturnCode inputs_on_button_interrupt(
    struct InputHandler *handler,
    enum ButtonID button_id,
    bool pressed,
    uint32_t current_tick_ms);

/*!
 * \brief Change state of a knob (called periodically or on interrupt)
 *
 * \param handler Pointer to the input handler structure
 * \param knob_id Identifier of the knob that changed state
 * \param current_position Current position of the encoder
 *
 * \retval INPUTS_OK if the knob state was updated successfully
 * \retval INPUTS_ERROR if there was an error updating the knob state
 */
enum InputsReturnCode inputs_check_knobs(
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
enum InputsReturnCode inputs_update(
    struct InputHandler *handler,
    uint32_t current_tick_ms);

#endif // INPUTS_H
