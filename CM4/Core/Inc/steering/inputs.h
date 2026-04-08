/*!
 * \file inputs.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Hardware-agnostic input handling definitions for the steering wheel.
 */

#ifndef INPUTS_H
#define INPUTS_H

#include "inputs-shared.h"
#include "ipc.h"
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
enum InputsButtonState {
    INPUTS_BUTTON_STATE_IDLE,         /*!< Button is not pressed */
    INPUTS_BUTTON_STATE_PRESSED,      /*!< Button is currently pressed */
    INPUTS_BUTTON_STATE_LONG_PRESSED, /*!< Button has been held long enough to be considered a long press */
};

/*!
 * \brief Button tracking structure
 */
struct InputsButtonHandler {
    enum InputsButtonState state; /*!< Current state of the button */
    uint32_t press_tick;          /*!< Tick count when the button was pressed */
    bool enabled;                 /*!< Whether this button is enabled for tracking */
};

/*!
 * \brief Knob (encoder) tracking structure
 */
struct InputsKnobHandler {
    int16_t last_position; /*!< Last known position of the encoder */
    bool enabled;          /*!< Whether this encoder is enabled for tracking */
};

/*!
 * \brief Callback definition for input event notifications
 *
 * \param ev The input event to handle
 *
 * \retval true if the event was handled successfully
 * \retval false if there was an error notifying the event
 */
typedef bool (*inputs_notify_callback)(struct InputsSharedEvent ev);

/*!
 * \brief Callback definition for input actions
 *
 * \param ev The input event to handle
 *
 * \retval INPUTS_RC_OK if the action was performed successfully
 * \retval INPUTS_RC_ERROR if there was an error handling the action
 */
typedef enum InputsReturnCode (*inputs_action_callback)(struct InputsSharedEvent ev);

/*!
 * \brief Main input handler structure
 */
struct InputsHandler {
    inputs_notify_callback notify_callback; /*!< Callback to notify CM7 about input events */
    inputs_action_callback action_callback; /*!< Callback to perform local actions on input */

    struct InputsButtonHandler buttons[INPUTS_SHARED_BUTTON_ID_COUNT]; /*!< Tracking state for each button */
    struct InputsKnobHandler knobs[INPUTS_SHARED_KNOB_ID_COUNT];       /*!< Tracking state for each encoder */
};

#endif // INPUTS_H
