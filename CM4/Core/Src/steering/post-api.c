/*!
 * \file post-api.c
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines Power-On Self-Test (POST) functions for system diagnostics.
 */

#include "post-api.h"
#include "inputs-api.h"
#include "ipc-api.h"
#include "leds-api.h"
#include "eagletrt-api.h"

// THIS IS HERE AS A PLACEHOLDER, WILL BE CHANGED WITH UI MODULE IMPLEMENTATION
EAGLETRT_STATIC enum InputsReturnCode input_action_noop(struct InputsSharedEvent ev) {
    EAGLETRT_API_UNUSED(ev);
    return INPUTS_RC_OK;
}

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    enum PostReturnCode ret_code = POST_RC_OK;

    if (inputs_api_init(post_init_data->ipc_critical_section, ipc_api_push_event, input_action_noop) != INPUTS_RC_OK) {
        ret_code = POST_RC_ERROR_SEVERE;
    }

    if (leds_api_init(post_init_data->leds_transmit) != LEDS_RC_OK) {
        ret_code = POST_RC_ERROR_SEVERE;
    }

    return ret_code;
}
