/*!
 * \file input-events.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Hardware-agnostic definitions for the steering wheel input events handling.
 *
 * \details CM7 only ever observes one kind of cross-core event: a parameter
 *     value update pushed by CM4. This module is the thin dispatch layer
 *     between the IPC queue and the UI.
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
 * \brief Callback type for parameter change events.
 *
 * \param parameter_id The ID of the parameter that changed.
 * \param value The new value of the parameter.
 *
 * \return An InputEventsReturnCode indicating success or failure.
 */
typedef enum InputEventsReturnCode (*input_events_parameter_change_callback)(
    enum InputsSharedParameterID parameter_id,
    uint8_t value);

/*!
 * \brief Structure holding the registered input event callbacks.
 */
struct InputEventHandler {
    input_events_parameter_change_callback on_parameter_change; /*!< Callback for parameter change events */
};

#endif // INPUT_EVENTS_H
