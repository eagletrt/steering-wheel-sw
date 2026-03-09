/*!
 * \file inputs.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Hardware-agnostic input handling definitions for the steering wheel.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include "inputs-shared.h"
#include <stdint.h>
#include <stdbool.h>

#define INPUTS_LONG_PRESS_THRESHOLD_MS (500)

/*!
 * \brief Input event types
 */
enum InputsReturnCode {
    INPUTS_RC_OK,           /*!< Operation successful */
    INPUTS_RC_NOTIFY_ERROR, /*!< Error while notifying */
    INPUTS_RC_ERROR,        /*!< General error */
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
struct ButtonHandler {
    enum ButtonState state; /*!< Current state of the button */
    uint32_t press_tick;    /*!< Tick count when the button was pressed */
    bool enabled;           /*!< Whether this button is enabled for tracking */
};

/*!
 * \brief Knob (encoder) tracking structure
 */
struct KnobHandler {
    int16_t last_position; /*!< Last known position of the encoder */
    bool enabled;          /*!< Whether this encoder is enabled for tracking */
};

/*!
 * \brief Callback definition for input event notifications
 *
 * \param event The input event to handle
 *
 * \retval INPUTS_RC_OK if the event was handled successfully
 * \retval INPUTS_RC_NOTIFY_ERROR if there was an error notifying the event
 * \retval INPUTS_RC_ERROR if there was an error handling the event
 */
typedef enum InputsReturnCode (*input_event_notify_callback)(struct InputEvent event);

/*!
 * \brief Main input handler structure
 */
struct InputHandler {
    input_event_notify_callback notify_callback; /*!< Callback to notify CM7 about input events */
    input_event_notify_callback action_callback; /*!< Callback to perform local actions on input */

    struct ButtonHandler buttons[BUTTON_ID_COUNT]; /*!< Tracking state for each button */
    struct KnobHandler knobs[KNOB_ID_COUNT];       /*!< Tracking state for each encoder */
};

#endif // INPUTS_H
