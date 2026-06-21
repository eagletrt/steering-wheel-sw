/*!
 * \file cm4-fsm.h
 * \date 2026-06-10
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Re-export of CM4's fsm.c and post-api.c with prefixed symbols.
 */

#ifndef CM4_FSM_H
#define CM4_FSM_H

#include "fsm.h"
#include "leds.h"
#include "parameters.h"
#include "can-communications.h"

/*!
 * \brief CM4 post-init payload, mirrored under a different struct tag.
 */
struct CM4PostInitData {
    leds_transmit_callback leds_transmit;                                                       /*!< Callback function required by leds module. */
    parameters_on_change_callback parameters_on_change;                                         /*!< Callback fired on every parameter transition. */
    struct CanCommunicationsNetworkConfig can_network_configs[CAN_COMMUNICATION_NETWORK_COUNT]; /*!< Configuration for each CAN network. */
};

/*!
 * \brief Drive one step of the CM4 FSM.
 *
 * \param cur_state Current FSM state.
 * \param data      State-data payload (see above).
 *
 * \return New FSM state.
 */
fsm_state_t cm4_fsm_run_state(fsm_state_t cur_state, void *data);

#endif // CM4_FSM_H
