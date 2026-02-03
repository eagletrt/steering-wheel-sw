/*!
 * \file shared.h
 * \date 2025-12-21
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Header file defining shared data structures
 *
 * \details This file contains the definition of data structures
 *     that are shared between the 2 cores.
 */

#ifndef SHARED_H
#define SHARED_H

#include <stdint.h>

/*!
 * \brief Enumeration of input event types
 */
enum InputEventType {
    INPUT_EVENT_TYPE_KNOB,
    INPUT_EVENT_TYPE_BUTTON_PRESS,
    INPUT_EVENT_TYPE_BUTTON_RELEASE,
    INPUT_EVENT_TYPE_BUTTON_LONG_PRESS,
};

/*!
 * \brief Enumeration of knob identifiers
 */
enum KnobId {
    KNOB_FRONT_LEFT,
    KNOB_FRONT_RIGHT,
    KNOB_SIDE_LEFT,
    KNOB_SIDE_RIGHT,
    KNOB_COUNT,
};

/*!
 * \brief Enumeration of button identifiers
 */
enum ButtonId {
    BUTTON_TS_ON,
    BUTTON_TOP_LEFT_1,
    BUTTON_TOP_LEFT_2,
    BUTTON_BOTTOM_LEFT,
    BUTTON_TOP_RIGHT_1,
    BUTTON_TOP_RIGHT_2,
    BUTTON_BOTTOM_RIGHT,
    PADDLE_TOP_LEFT,
    PADDLE_BOTTOM_LEFT,
    PADDLE_TOP_RIGHT,
    PADDLE_BOTTOM_RIGHT,
    KNOB_PUSH_FRONT_LEFT,
    KNOB_PUSH_FRONT_RIGHT,
    KNOB_PUSH_SIDE_LEFT,
    KNOB_PUSH_SIDE_RIGHT,
    BUTTON_COUNT,
};

/*!
 * \brief Structure representing an input event
 * 
 * \details This structure uses a union to store different types of input events.
 */
struct InputEvent {
    enum InputEventType type; /*!< Type of the input event */
    union {
        struct {
            enum KnobId knob_id; /*!< Identifier for the knob */
            int8_t delta;        /*!< Change in knob position */
        } knob;
        struct {
            enum ButtonId button_id; /*!< Identifier for the button */
        } button;
    };
};

#endif // SHARED_H
