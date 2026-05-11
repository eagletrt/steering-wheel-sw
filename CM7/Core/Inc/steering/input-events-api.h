/*!
 * \file input-events-api.h
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Hardware-agnostic API to handle input events.
 */

#ifndef INPUT_EVENTS_API_H
#define INPUT_EVENTS_API_H

#include "input-events.h"

/*!
 * \brief Initialize the input event handler with the parameter-change callback.
 *
 * \param on_parameter_change Callback for parameter change events.
 *
 * \retval INPUT_EVENTS_RC_OK if initialization was successful.
 * \retval INPUT_EVENTS_RC_ERROR if \p on_parameter_change is NULL.
 */
enum InputEventsReturnCode input_events_api_init(
    input_events_parameter_change_callback on_parameter_change);

/*!
 * \brief Handle an input event by invoking the registered callback.
 *
 * \param event The input event to handle.
 */
void input_events_api_handle_event(
    struct InputsSharedEvent event);

#endif // INPUT_EVENTS_API_H
