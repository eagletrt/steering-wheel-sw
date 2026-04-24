/*!
 * \file parameters.h
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Hardware-agnostic definitions for the steering wheel tunable parameters.
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "inputs.h"
#include "inputs-shared.h"
#include <stdint.h>

/*!
 * \brief Return codes for parameters operations.
 */
enum ParametersReturnCode {
    PARAMETERS_RC_OK,           /*!< Operation completed successfully */
    PARAMETERS_RC_NOTIFY_ERROR, /*!< Error while notifying the change to CM7 */
    PARAMETERS_RC_ERROR,        /*!< General error */
};

/*!
 * \brief Parameters handler structure.
 */
struct ParametersHandler {
    inputs_notify_callback notify_callback;           /*!< Callback used to notify CM7 about parameter changes */
    uint8_t values[INPUTS_SHARED_PARAMETER_ID_COUNT]; /*!< Current value of each parameter */
};

#endif // PARAMETERS_H
