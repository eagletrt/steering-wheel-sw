/*!
 * \file post-api.c
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief This file defines Power-On Self-Test (POST) functions for system diagnostics.
 */

#include "post-api.h"
#include "inputs-api.h"
#include "leds-api.h"
#include "eagletrt-api.h"

// THIS IS HERE AS A PLACEHOLDER, WILL BE CHANGED WITH UI MODULE IMPLEMENTATION
EAGLETRT_STATIC enum InputsReturnCode input_action_noop(struct InputsSharedEvent ev) {
    EAGLETRT_API_UNUSED(ev);
    return INPUTS_RC_OK;
}

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    if (post_init_data == NULL || post_init_data->leds_transmit == NULL || post_init_data->inputs_notify == NULL) {
        return POST_RC_ERROR;
    }

    enum PostReturnCode ret_code = POST_RC_OK;

    if (inputs_api_init(post_init_data->inputs_notify, input_action_noop) != INPUTS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    if (leds_api_init(post_init_data->leds_transmit) != LEDS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    return ret_code;
}
