/*!
 * \file input-events-api.c
 * \date 2025-12-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Hardware-agnostic API to handle input events.
 */

#include "input-events-api.h"
#include "eagletrt.h"
#include <stddef.h>

EAGLETRT_STATIC struct InputEventsHandler input_events_handler;

enum InputEventsReturnCode input_events_api_init(
    input_events_parameter_change_callback on_parameter_change) {
    if (on_parameter_change == NULL) {
        return INPUT_EVENTS_RC_ERROR;
    }
    input_events_handler.on_parameter_change = on_parameter_change;
    return INPUT_EVENTS_RC_OK;
}

void input_events_api_handle_event(struct InputsSharedEvent event) {
    if (input_events_handler.on_parameter_change != NULL) {
        input_events_handler.on_parameter_change(event.parameter_id, event.value);
    }
}
