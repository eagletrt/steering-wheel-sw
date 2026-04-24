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
#include "parameters-api.h"

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    if (post_init_data == NULL || post_init_data->leds_transmit == NULL || post_init_data->inputs_notify == NULL) {
        return POST_RC_ERROR;
    }

    enum PostReturnCode ret_code = POST_RC_OK;

    if (parameters_api_init(post_init_data->inputs_notify) != PARAMETERS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    if (inputs_api_init(post_init_data->inputs_notify, parameters_api_handle_input) != INPUTS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    if (leds_api_init(post_init_data->leds_transmit) != LEDS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    return ret_code;
}
