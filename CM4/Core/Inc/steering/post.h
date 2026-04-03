/*!
 * \file post.h
 * \date 2026-04-01
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines Power-On Self-Test (POST) structures for system diagnostics.
 */

#ifndef POST_H
#define POST_H

#include "inputs.h"
#include "leds.h"
#include "ipc.h"

enum PostReturnCode {
    POST_RC_OK,    /*!< POST completed successfully. */
    POST_RC_ERROR, /*!< POST encountered an error. */
};

struct PostInitData {
    ipc_critical_section_callback ipc_critical_section; /*!< Callback function required by leds module. */
    leds_transmit_callback leds_transmit;               /*!< Callback function required by leds module. */
    inputs_notify_callback inputs_notify;               /*!< Callback function required by inputs module. */
};

#endif // POST_H
