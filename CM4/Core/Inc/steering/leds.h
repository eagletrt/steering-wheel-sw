/*!
 * \file leds.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines structures and enumerations to manage 9 leds on the steering wheel.
 */

#ifndef LEDS_H
#define LEDS_H

#include "ws2812b-api.h"
#include <stdint.h>
#include <stddef.h>

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
    LEDS_COUNT = 9        /*!< Total number of LEDs in the strip. */
};

/*!
 * \brief Handler structure for managing the LED system.
 */
struct LedsHandler {
    struct LedColor leds[LEDS_COUNT];                     /*!< Array of LedColor structures representing the colors of each LED. */
    uint8_t brightness;                                   /*!< Brightness level for the LEDs (0-255), where 0 is off and 255 is full brightness. (default: 255) */
    uint32_t buffer[WS2812B_API_BUFFER_SIZE(LEDS_COUNT)]; /*!< Buffer for storing the PWM data to be transmitted to the LEDs. */
    struct WS2812BHandler ws2812b_handler;                /*!< Handler for managing the WS2812B LED strip. */
};

#endif // LEDS_H
