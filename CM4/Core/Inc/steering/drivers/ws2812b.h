/*!
 * \file ws2812b.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines constants and return codes for WS2812B LED control, including duty cycle values for representing binary 0 and 1, as well as the number of reset slots required after transmitting data to the LEDs.
 */

#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stddef.h>

#define WS2812B_DUTY_0 (35U)
#define WS2812B_DUTY_1 (70U)
#define WS2812B_RESET_SLOTS (50U)

/*!
 * \brief Return codes for WS2812B functions.
 */
enum WS2812BReturnCode {
    WS2812B_RC_OK,           /*!< Operation successful. */
    WS2812B_RC_NULL_POINTER, /*!< A null pointer was passed to a function. */
};

#endif // WS2812B_H
