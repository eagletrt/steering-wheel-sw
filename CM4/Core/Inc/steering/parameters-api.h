/*!
 * \file parameters-api.h
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Hardware-agnostic API to manage the steering wheel tunable parameters.
 *
 * \details Maps raw input events (knob rotations, button presses) to changes
 *     on a small set of user-facing parameters (power, regen, torque vectoring,
 *     traction control, launch control) and notifies CM7 about every change so
 *     the UI can render a popup.
 */

#ifndef PARAMETERS_API_H
#define PARAMETERS_API_H

#include "inputs.h"
#include "parameters.h"

/*!
 * \brief Initialize the parameters handler.
 *
 * \param notify_callback Callback used to push parameter change events to CM7.
 *
 * \retval PARAMETERS_RC_OK if initialization was successful.
 * \retval PARAMETERS_RC_ERROR if the callback is NULL.
 */
enum ParametersReturnCode parameters_api_init(
    inputs_notify_callback notify_callback);

/*!
 * \brief Translate an input event into a parameter change, if any.
 *
 * \details This function is meant to be registered as the action callback of
 *     the inputs module. Input events that do not map to a parameter are
 *     silently ignored.
 *
 * \param event The input event to process.
 *
 * \retval INPUTS_RC_OK if the event was handled (or ignored) successfully.
 * \retval INPUTS_RC_NOTIFY_ERROR if the change could not be notified to CM7.
 * \retval INPUTS_RC_ERROR on generic failure.
 */
enum InputsReturnCode parameters_api_handle_input(
    struct InputsSharedEvent event);

/*!
 * \brief Read the current value of a parameter.
 *
 * \param parameter_id The parameter to read.
 *
 * \return Current value of the parameter, 0 if \p parameter_id is invalid.
 */
uint8_t parameters_api_get_value(enum InputsSharedParameterID parameter_id);

#endif // PARAMETERS_API_H
