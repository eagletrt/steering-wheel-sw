#ifndef WS2812B_H
#define WS2812B_H

#include <stdint.h>
#include <stddef.h>

/*!
 * \brief Return codes for WS2812B functions.
 */
enum WS2812BReturnCode {
    WS2812B_RC_OK,                 /*!< Operation successful. */
    WS2812B_RC_NULL_POINTER,       /*!< A null pointer was passed to a function. */
    WS2812B_RC_INVALID_LED,        /*!< The specified LED index is out of range. */
    WS2812B_RC_TRANSMISSION_ERROR, /*!< An error occurred during data transmission. */
};

/*!
 * \brief Structure representing the color of a WS2812B LED.
 */
struct WS2812BColor {
    uint8_t r; /*!< Red component (0-255). */
    uint8_t g; /*!< Green component (0-255). */
    uint8_t b; /*!< Blue component (0-255). */
};

/*!
 * \brief Function pointer type for transmitting data to the WS2812B LEDs.
 *
 * This callback function is responsible for sending the PWM data to the LED strip.
 *
 * \param buffer Pointer to the buffer containing the PWM data to be transmitted.
 * \param size Size of the buffer in bytes.
 *
 * \retval WS2812B_RC_OK Data transmitted successfully.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the buffer.
 * \retval WS2812B_RC_TRANSMISSION_ERROR An error occurred during data transmission.
 */
typedef enum WS2812BReturnCode (*ws2812b_transmit_callback)(const uint16_t *buffer, size_t size);

/*!
 * \brief Structure representing the handler for WS2812B LED control.
 *
 * This structure contains a function pointer for transmitting data to the LEDs,
 * a pointer to an array of WS2812BColor structures representing the LED colors,
 * and the number of LEDs in the strip.
 */
struct WS2812BHandler {
    struct WS2812BColor *leds;          /*!< Pointer to an array of WS2812BColor structures representing the LED colors. */
    uint16_t *pwm_buffer;               /*!< Pointer to a buffer for PWM data to be transmitted to the LEDs. */
    size_t num_leds;                    /*!< Number of LEDs in the strip. */
    uint16_t duty_0;                    /*!< PWM duty cycle value representing a '0' bit for the WS2812B protocol. */
    uint16_t duty_1;                    /*!< PWM duty cycle value representing a '1' bit for the WS2812B protocol. */
    ws2812b_transmit_callback transmit; /*!< Function pointer for transmitting data to the LEDs. */
};

#endif // WS2812B_H
