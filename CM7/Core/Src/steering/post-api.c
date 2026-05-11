/*!
 * \file post-api.c
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief This file defines Power-On Self-Test (POST) functions for system diagnostics.
 */

#include "post-api.h"
#include "input-events-api.h"
#include "screen-api.h"

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    enum PostReturnCode ret_code = POST_RC_OK;

    if (post_init_data == NULL || post_init_data->draw_line == NULL || post_init_data->draw_rectangle == NULL) {
        return POST_RC_ERROR;
    }

    if (input_events_api_init(screen_on_parameter_change) != INPUT_EVENTS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    if (screen_init(post_init_data->draw_line, post_init_data->draw_rectangle) != SCREEN_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    return ret_code;
}
