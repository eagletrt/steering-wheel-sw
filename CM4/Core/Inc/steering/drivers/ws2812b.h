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
 * \brief Callback function type for retrieving the timer tick frequency in Hz.
 *
 * This function should return the frequency of the timer ticks in Hz, which is used to calculate the duty cycle values and reset slots for WS2812B data transmission.
 *
 * \return The timer tick frequency in Hz.
 */
typedef uint32_t (*ws2812b_get_tick_hz_callback)(void);

/*!
 * \brief Return codes for WS2812B functions.
 */
enum WS2812BReturnCode {
    WS2812B_RC_OK,             /*!< Operation successful. */
    WS2812B_RC_ENCODING_ERROR, /*!< An error occurred during data encoding. */
    WS2812B_RC_NULL_POINTER,   /*!< A null pointer was passed to a function. */
};

/*!
 * \brief Handler structure for WS2812B LED control.
 *
 * This structure contains the duty cycle values for representing binary 0 and 1, as well as the number of reset slots required after transmitting data to the LEDs.
 */
struct WS2812BHandler {
    uint16_t duty_0; /*!< Duty cycle value representing binary 0. */
    uint16_t duty_1; /*!< Duty cycle value representing binary 1. */
};

#endif // WS2812B_H
