/*!
 * \file ws2812b.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines constants and return codes for WS2812B LED control.
 */

#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define WS2812B_FREQUENCY_HZ (800000U) /*!< Frequency in Hz for WS2812B LED data transmission. */
#define WS2812B_RESET_SLOTS (50U)      /*!< Number of time slots required to reset the WS2812B LEDs after data transmission. */
#define WS2812B_DUTY_0_RATIO (0.35f)   /*!< Duty cycle ratio for representing binary 0 (35% of the time slot). */
#define WS2812B_DUTY_1_RATIO (0.70f)   /*!< Duty cycle ratio for representing binary 1 (70% of the time slot). */

/*!
 * \brief Enumeration for WS2812B duty cycle types.
 *
 * This enumeration defines the different duty cycle types used for representing binary 0, binary 1, and reset slots in WS2812B data transmission.
 */
enum WS2812BDutyCycle {
    WS2812B_DUTY_CYCLE_0,    /*!< Duty cycle for binary 0. */
    WS2812B_DUTY_CYCLE_1,    /*!< Duty cycle for binary 1. */
    WS2812B_DUTY_CYCLE_RESET /*!< Duty cycle for reset slots. */
};

/*!
 * \brief Return codes for WS2812B functions.
 */
enum WS2812BReturnCode {
    WS2812B_RC_OK,             /*!< Operation successful. */
    WS2812B_RC_ENCODING_ERROR, /*!< An error occurred during data encoding. */
    WS2812B_RC_NULL_POINTER,   /*!< A null pointer was passed to a function. */
};

#endif // WS2812B_H
