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
#include "can-communications-api.h"

enum PostReturnCode post_api_do_init(struct PostInitData *post_init_data) {
    if (post_init_data == NULL ||
        post_init_data->leds_transmit == NULL ||
        post_init_data->parameters_on_change == NULL ||
        post_init_data->can_send[0] == NULL ||
        post_init_data->can_send[1] == NULL ||
        post_init_data->can_on_receive[0] == NULL ||
        post_init_data->can_on_receive[1] == NULL) {
        return POST_RC_ERROR;
    }

    enum PostReturnCode ret_code = POST_RC_OK;

    if (parameters_api_init(post_init_data->parameters_on_change) != PARAMETERS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    // Press and release both feed parameters-api: release is needed so the
    // PTT chord (both top paddles held) can detect a let-go on either side.
    // Long-press is unused today.
    if (inputs_api_init(
            parameters_api_handle_button,
            NULL,
            parameters_api_handle_button_release,
            parameters_api_handle_knob) != INPUTS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    if (leds_api_init(post_init_data->leds_transmit) != LEDS_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    struct CanCommunicationsNetworkConfig primary_config = {
        .send = post_init_data->can_send[CAN_COMMUNICATION_NETWORK_PRIMARY],
        .on_receive = post_init_data->can_on_receive[CAN_COMMUNICATION_NETWORK_PRIMARY],
        .cs_enter = post_init_data->can_cs_enter[CAN_COMMUNICATION_NETWORK_PRIMARY],
        .cs_exit = post_init_data->can_cs_exit[CAN_COMMUNICATION_NETWORK_PRIMARY],
    };

    if (can_communications_api_init(CAN_COMMUNICATION_NETWORK_PRIMARY, &primary_config) != CAN_COMMUNICATION_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    struct CanCommunicationsNetworkConfig secondary_config = {
        .send = post_init_data->can_send[CAN_COMMUNICATION_NETWORK_SECONDARY],
        .on_receive = post_init_data->can_on_receive[CAN_COMMUNICATION_NETWORK_SECONDARY],
        .cs_enter = post_init_data->can_cs_enter[CAN_COMMUNICATION_NETWORK_SECONDARY],
        .cs_exit = post_init_data->can_cs_exit[CAN_COMMUNICATION_NETWORK_SECONDARY],
    };

    if (can_communications_api_init(CAN_COMMUNICATION_NETWORK_SECONDARY, &secondary_config) != CAN_COMMUNICATION_RC_OK) {
        ret_code = POST_RC_ERROR;
    }

    return ret_code;
}
