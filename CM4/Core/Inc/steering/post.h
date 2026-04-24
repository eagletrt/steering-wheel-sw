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
#include "parameters.h"

enum PostReturnCode {
    POST_RC_OK,    /*!< POST completed successfully. */
    POST_RC_ERROR, /*!< POST encountered an error. */
};

struct PostInitData {
    leds_transmit_callback leds_transmit;               /*!< Callback function required by leds module. */
    parameters_on_change_callback parameters_on_change; /*!< Callback fired on every parameter transition. */
};

#endif // POST_H
