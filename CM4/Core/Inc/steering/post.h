/*!
 * \file post.h
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief This file defines Power-On Self-Test (POST) structures for system diagnostics.
 */

#ifndef POST_H
#define POST_H

#include "leds.h"
#include "can-communications.h"
#include "parameters.h"

enum PostReturnCode {
    POST_RC_OK,    /*!< POST completed successfully. */
    POST_RC_ERROR, /*!< POST encountered an error. */
};

struct PostInitData {
    leds_transmit_callback leds_transmit;                                                       /*!< Callback function required by leds module. */
    parameters_on_change_callback parameters_on_change;                                         /*!< Callback fired on every parameter transition. */
    can_communications_send_callback can_send[CAN_COMMUNICATION_NETWORK_COUNT];                 /*!< Array of callbacks for sending CAN frames, one per network. */
    can_communications_receive_callback can_on_receive[CAN_COMMUNICATION_NETWORK_COUNT];        /*!< Array of callbacks for receiving CAN frames, one per network. */
    can_communications_critical_section_callback can_cs_enter[CAN_COMMUNICATION_NETWORK_COUNT]; /*!< Array of callbacks for entering critical sections, one per network. Optional, may be NULL. */
    can_communications_critical_section_callback can_cs_exit[CAN_COMMUNICATION_NETWORK_COUNT];
};

#endif // POST_H
