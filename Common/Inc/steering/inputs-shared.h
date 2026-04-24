/*!
 * \file inputs-shared.h
 * \date 2025-12-21
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Cross-core vocabulary for the steering wheel inputs and parameters.
 *
 * \details This header carries the few symbols that need to be visible from
 *     both CM4 and CM7:
 *     - the taxonomy of physical inputs (knob and button IDs), consumed by
 *       CM4's inputs module;
 *     - the list of tunable parameters (power, regen, torque vectoring,
 *       traction/launch control) and the single payload struct pushed to CM7
 *       whenever one of them changes.
 *
 *     Raw button/knob events never cross the core boundary: they are mapped
 *     to parameter changes inside CM4 and only the resulting {parameter_id,
 *     value} pair is forwarded to CM7 for the UI popup.
 */

#ifndef INPUTS_SHARED_H
#define INPUTS_SHARED_H

#include <stdint.h>

/*!
 * \brief Enumeration of knob identifiers
 */
enum InputsSharedKnobID {
    INPUTS_SHARED_KNOB_ID_FRONT_LEFT,
    INPUTS_SHARED_KNOB_ID_FRONT_RIGHT,
    INPUTS_SHARED_KNOB_ID_SIDE_LEFT,
    INPUTS_SHARED_KNOB_ID_SIDE_RIGHT,
    INPUTS_SHARED_KNOB_ID_COUNT,
};

/*!
 * \brief Enumeration of button identifiers
 */
enum InputsSharedButtonID {
    INPUTS_SHARED_BUTTON_ID_TS_ON,
    INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1,
    INPUTS_SHARED_BUTTON_ID_TOP_LEFT_2,
    INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT,
    INPUTS_SHARED_BUTTON_ID_TOP_RIGHT_1,
    INPUTS_SHARED_BUTTON_ID_TOP_RIGHT_2,
    INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT,
    INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT,
    INPUTS_SHARED_BUTTON_ID_PADDLE_BOTTOM_LEFT,
    INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT,
    INPUTS_SHARED_BUTTON_ID_PADDLE_BOTTOM_RIGHT,
    INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_FRONT_LEFT,
    INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_FRONT_RIGHT,
    INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_SIDE_LEFT,
    INPUTS_SHARED_BUTTON_ID_KNOB_PUSH_SIDE_RIGHT,
    INPUTS_SHARED_BUTTON_ID_COUNT,
};

/*!
 * \brief Enumeration of user-facing parameters driven by inputs.
 *
 * \details Parameters with a numeric range use a uint8_t value from 0 to
 *     INPUTS_SHARED_PARAMETER_NUMERIC_MAX. Toggle parameters use 0 for OFF
 *     and 1 for ON.
 */
enum InputsSharedParameterID {
    INPUTS_SHARED_PARAMETER_ID_POWER,            /*!< Power level (0..10) */
    INPUTS_SHARED_PARAMETER_ID_REGEN,            /*!< Regenerative braking level (0..10) */
    INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING, /*!< Torque vectoring level (0..10) */
    INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG,    /*!< Telemetry log toggle */
    INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL,   /*!< Launch control toggle */
    INPUTS_SHARED_PARAMETER_ID_COUNT,
};

/*!
 * \brief Inclusive upper bound for numeric parameter values.
 */
#define INPUTS_SHARED_PARAMETER_NUMERIC_MAX (10U)

/*!
 * \brief Cross-core payload describing a parameter value update.
 *
 * \details This is the only payload that ever travels through the IPC queue.
 *     CM4 pushes one of these whenever a parameter changes (either in
 *     response to a physical input or via the external setter), and CM7
 *     reads them to drive the popup UI.
 */
struct InputsSharedEvent {
    enum InputsSharedParameterID parameter_id; /*!< Parameter that changed */
    uint8_t value;                             /*!< New value of the parameter */
};

#endif // INPUTS_SHARED_H
