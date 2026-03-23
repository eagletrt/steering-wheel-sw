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
#define WS2812B_SLOT_DUR (125U)        /*!< Duration of each time slot in nanoseconds for WS2812B data transmission. */
#define WS2812B_DUTY_0_RATIO (35U)     /*!< Duty cycle ratio for representing binary 0 (35% of the time slot). */
#define WS2812B_DUTY_1_RATIO (70U)     /*!< Duty cycle ratio for representing binary 1 (70% of the time slot). */

/*!
 * Forward declaration of the WS2812BHandler structure, which is used in the callback function type definition.
 */
struct WS2812BHandler;

/*!
 * \brief Callback function type for transmitting PWM data to WS2812B LEDs.
 *
 * \param pwm_buffer Pointer to the buffer containing the PWM duty cycle values to be transmitted to the LEDs.
 * \param length The number of PWM duty cycle values in the buffer.
 */
typedef enum WS2812BReturnCode (*ws2812b_pwm_transmit_callback)(struct WS2812BHandler *handler, const uint32_t *pwm_buffer, uint16_t length);

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
    WS2812B_RC_OK,                 /*!< Operation successful. */
    WS2812B_RC_TRANSMISSION_ERROR, /*!< An error occurred during data transmission. */
    WS2812B_RC_NULL_POINTER,       /*!< A null pointer was passed to a function. */
    WS2812B_RC_BUSY,               /*!< The handler is currently busy transmitting data. */
};

/*!
 * \brief Handler structure for WS2812B LED control.
 *
 * This structure contains the duty cycle values for representing binary 0 and 1, as well as the number of reset slots required after transmitting data to the LEDs.
 */
struct WS2812BHandler {
    uint32_t duty_0;                                 /*!< Duty cycle value representing binary 0. */
    uint32_t duty_1;                                 /*!< Duty cycle value representing binary 1. */
    ws2812b_pwm_transmit_callback transmit_callback; /*!< Callback function for transmitting PWM data to the LEDs. */
    volatile bool busy;                              /*!< Flag indicating whether the handler is currently busy transmitting data. */
};

#endif // WS2812B_H
