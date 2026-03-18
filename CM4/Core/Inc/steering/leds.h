/*!
 * \file leds.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines structures and enumerations to manage 9 leds on the steering wheel.
 */

#ifndef LEDS_H
#define LEDS_H

#include "ws2812b.h"
#include <stdint.h>
#include <stddef.h>

#define LEDS_COUNT 9
#define LEDS_PWM_BUFFER_SIZE (LEDS_COUNT * 24 + WS2812B_RESET_SLOTS)

/*!
 * \brief Return codes for LED operations.
 */
enum LedsReturnCode {
    LEDS_RC_OK,                 /*!< Operation successful. */
    LEDS_RC_INVALID_LED,        /*!< The specified LED index is out of range. */
    LEDS_RC_NULL_POINTER,       /*!< A null pointer was passed */
    LEDS_RC_TRANSMISSION_ERROR, /*!< An error ocurred during data transmission */
};

/*!
 * \brief Structure representing the color of a LED (GRB ordering).
 */
struct LedColor {
    uint8_t g; /*!< Green component (0-255). */
    uint8_t r; /*!< Red component (0-255). */
    uint8_t b; /*!< Blue component (0-255). */
};

/*!
 * \brief Enumeration for LED indices in the strip.
 *
 * This enumeration defines the indices for each LED in the strip, allowing for easy reference when setting colors.
 */
enum LedsIndex {
    LEDS_CENTER_0 = 0,    /*!< Center LED 0 (index 0). */
    LEDS_CENTER_1 = 1,    /*!< Center LED 1 (index 1). */
    LEDS_CENTER_2 = 2,    /*!< Center LED 2 (index 2). */
    LEDS_CENTER_3 = 3,    /*!< Center LED 3 (index 3). */
    LEDS_CENTER_4 = 4,    /*!< Center LED 4 (index 4). */
    LEDS_TOP_LEFT_1 = 5,  /*!< Top left LED 1 (index 5). */
    LEDS_TOP_LEFT_0 = 6,  /*!< Top left LED 0 (index 6). */
    LEDS_TOP_RIGHT_0 = 7, /*!< Top right LED 0 (index 7). */
    LEDS_TOP_RIGHT_1 = 8, /*!< Top right LED 1 (index 8). */
};

/*!
 * \brief Function pointer type for transmitting data to the WS2812B LEDs.
 *
 * This callback function is responsible for sending the PWM data to the LED strip.
 *
 * \param buffer Pointer to the buffer containing the PWM data to be transmitted.
 * \param size Size of the buffer in bytes.
 *
 * \retval LEDS_RC_OK if the transmission was successful, or an appropriate error code if it failed.
 * \retval LEDS_RC_NULL_POINTER if a null pointer was passed to the function.
 * \retval LEDS_RC_TRANSMISSION_ERROR if an error occurred during data transmission.
 */
typedef enum LedsReturnCode (*leds_transmit_callback)(const uint16_t *buffer, size_t size);

/*!
 * \brief Handler structure for managing the LED system.
 */
struct LedsHandler {
    struct LedColor leds[LEDS_COUNT];          /*!< Array of LedColor structures representing the colors of each LED. */
    uint16_t pwm_buffer[LEDS_PWM_BUFFER_SIZE]; /*!< Buffer for storing the PWM data to be transmitted to the LEDs. */
    leds_transmit_callback transmit;           /*!< Function pointer for transmitting data to the LEDs. */
};

#endif // LEDS_H
