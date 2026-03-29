/*!
 * \file inputs-shared.h
 * \date 2025-12-21
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Header file defining shared data structures
 *
 * \details This file contains the definition of data structures
 *     that are shared between the 2 cores.
 */

#ifndef INPUTS_SHARED_H
#define INPUTS_SHARED_H

#include <stdint.h>

/*!
 * \brief Enumeration of input event types
 */
enum InputsSharedEventType {
    INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION,
    INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS,
    INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE,
    INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
};

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
 * \brief Structure representing an input event
 *
 * \details This structure uses a union to store different types of input events.
 */
struct InputsSharedEvent {
    enum InputsSharedEventType type; /*!< Type of the input event */
    union {
        struct {
            enum InputsSharedKnobID knob_id; /*!< Identifier for the knob */
            int8_t delta;                    /*!< Change in knob position */
        } knob;
        struct {
            enum InputsSharedButtonID button_id; /*!< Identifier for the button */
        } button;
    };
};

#endif // INPUTS_SHARED_H
