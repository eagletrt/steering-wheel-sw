/*!
 * \file leds.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief This file defines structures and enumerations to manage 9 leds on the steering wheel.
 */

#ifndef LEDS_H
#define LEDS_H

#include "ws2812b-api.h"
#include <stdint.h>
#include <stddef.h>

/*!
 * \brief Callback function type for transmitting LED data.
 *
 * This function should be implemented by the user to handle the actual transmission of the LED data to the hardware. The buffer will contain the encoded values for the LEDs, and the length will indicate how many values are in the buffer.
 *
 * \param buffer Pointer to the buffer containing the values to be transmitted to the LEDs.
 * \param length The number of values in the buffer.
 *
 * \retval LEDS_RC_OK Transmission successful.
 * \retval LEDS_RC_TRANSMISSION_ERROR An error occurred during data transmission.
 * \retval LEDS_RC_NULL_POINTER A null pointer was passed for the buffer.
 * \retval LEDS_RC_BUSY The handler is currently busy transmitting data.
 */
typedef enum LedsReturnCode (*leds_transmit_callback)(const enum WS2812BDutyCycle *buffer, uint16_t length);

/*!
 * \brief Return codes for LED operations.
 */
enum LedsReturnCode {
    LEDS_RC_OK,                 /*!< Operation successful. */
    LEDS_RC_INVALID_LED,        /*!< The specified LED index is out of range. */
    LEDS_RC_NULL_POINTER,       /*!< A null pointer was passed */
    LEDS_RC_TRANSMISSION_ERROR, /*!< An error ocurred during data transmission */
    LEDS_RC_BUSY,               /*!< The handler is currently busy transmitting data. */
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
    LEDS_INDEX_CENTER_0 = 0,    /*!< Center LED 0 */
    LEDS_INDEX_CENTER_1 = 1,    /*!< Center LED 1 */
    LEDS_INDEX_CENTER_2 = 2,    /*!< Center LED 2 */
    LEDS_INDEX_CENTER_3 = 3,    /*!< Center LED 3 */
    LEDS_INDEX_CENTER_4 = 4,    /*!< Center LED 4 */
    LEDS_INDEX_TOP_LEFT_1 = 5,  /*!< Top left LED 1 */
    LEDS_INDEX_TOP_LEFT_0 = 6,  /*!< Top left LED 0 */
    LEDS_INDEX_TOP_RIGHT_0 = 7, /*!< Top right LED 0 */
    LEDS_INDEX_TOP_RIGHT_1 = 8, /*!< Top right LED 1 */
    LEDS_INDEX_COUNT = 9        /*!< Total number of LEDs in the strip. */
};

/*!
 * \brief Handler structure for managing the LED system.
 */
struct LedsHandler {
    struct LedColor colors[LEDS_INDEX_COUNT];                                /*!< Array of LedColor structures representing the colors of each LED. */
    float brightness;                                                        /*!< Brightness level for the LEDs (0-1), where 0 is off and 1 is full brightness. (default: 1) */
    enum WS2812BDutyCycle buffer[WS2812B_API_BUFFER_SIZE(LEDS_INDEX_COUNT)]; /*!< Buffer for storing the encoded LED data to be transmitted to the hardware. The size is determined by the number of LEDs and the requirements of the hardware peripheral. */
    leds_transmit_callback transmit_callback;                                /*!< Callback function for transmitting the LED data to the hardware. */
};

#endif // LEDS_H
