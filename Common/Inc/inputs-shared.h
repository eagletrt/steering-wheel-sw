/*!
 * \file inputs-shared.h
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
 * \brief HSEM ID used by CM4 to notify CM7 about input events.
 */
#define HSEM_INPUT_ID (1U)

/*!
 * \brief Size of the IPC input event ring buffer.
 */
#define IPC_INPUT_QUEUE_SIZE 64

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
enum KnobID {
    KNOB_ID_FRONT_LEFT,
    KNOB_ID_FRONT_RIGHT,
    KNOB_ID_SIDE_LEFT,
    KNOB_ID_SIDE_RIGHT,
    KNOB_ID_COUNT,
};

/*!
 * \brief Enumeration of button identifiers
 */
enum ButtonID {
    BUTTON_ID_TS_ON,
    BUTTON_ID_TOP_LEFT_1,
    BUTTON_ID_TOP_LEFT_2,
    BUTTON_ID_BOTTOM_LEFT,
    BUTTON_ID_TOP_RIGHT_1,
    BUTTON_ID_TOP_RIGHT_2,
    BUTTON_ID_BOTTOM_RIGHT,
    BUTTON_ID_PADDLE_TOP_LEFT,
    BUTTON_ID_PADDLE_BOTTOM_LEFT,
    BUTTON_ID_PADDLE_TOP_RIGHT,
    BUTTON_ID_PADDLE_BOTTOM_RIGHT,
    BUTTON_ID_KNOB_PUSH_FRONT_LEFT,
    BUTTON_ID_KNOB_PUSH_FRONT_RIGHT,
    BUTTON_ID_KNOB_PUSH_SIDE_LEFT,
    BUTTON_ID_KNOB_PUSH_SIDE_RIGHT,
    BUTTON_ID_COUNT,
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
            enum KnobID knob_id; /*!< Identifier for the knob */
            int8_t delta;        /*!< Change in knob position */
        } knob;
        struct {
            enum ButtonID button_id; /*!< Identifier for the button */
        } button;
    };
};

/*!
 * \brief Lock-free single-producer single-consumer ring buffer for input events.
 */
struct IPCInputQueue {
    uint32_t write_idx;                             /*!< Next slot to write (owned by CM4) */
    uint32_t read_idx;                              /*!< Next slot to read  (owned by CM7) */
    struct InputEvent events[IPC_INPUT_QUEUE_SIZE]; /*!< Ring buffer storage */
};

#endif // SHARED_H
