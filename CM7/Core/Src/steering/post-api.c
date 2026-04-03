/*!
 * \file post-api.c
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines Power-On Self-Test (POST) functions for system diagnostics.
 */

#include "post-api.h"
#include "input-events-api.h"
#include "screen-api.h"

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    enum PostReturnCode ret_code = POST_RC_OK;

    if (input_events_api_init(
            mock_input_event_button_event_callback,
            mock_input_event_button_long_press_callback,
            mock_input_event_button_release_callback,
            mock_input_event_knob_rotation_callback) != INPUT_EVENTS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    return ret_code;
}
