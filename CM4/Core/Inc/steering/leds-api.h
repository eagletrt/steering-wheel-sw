#ifndef LEDS_API_H
#define LEDS_API_H

#include "leds.h"

/*!
 * \brief Initializes the LED system, clearing all LEDs and preparing the driver for operation.
 *
 * \param transmit Function pointer for transmitting data to the LEDs.
 *
 * \retval LEDS_RC_OK Initialization successful.
 * \retval LEDS_RC_NULL_POINTER A null pointer was passed for the transmit callback.
 */
enum LedsReturnCode leds_api_init(leds_transmit_callback transmit);

/*!
 * \brief Sets the color of a specific LED in the strip.
 *
 * \param index Index of the LED to be set (0-based).
 * \param color LedColor structure representing the desired color for the LED.
 *
 * \retval LEDS_RC_OK Color set successfully.
 * \retval LEDS_RC_INVALID_LED The specified LED index is out of range.
 */
enum LedsReturnCode leds_api_set_led(enum LedsIndex index, struct LedColor color);

/*!
 * \brief Fills the entire LED strip with a specific color.
 *
 * \param color LedColor structure representing the desired color for all LEDs.
 */
void leds_api_fill(struct LedColor color);

/*!
 * \brief Clears the LED strip by setting all LEDs to off (black).
 */
void leds_api_clear(void);

/*!
 * \brief Transmits the current LED color data to the LED strip, updating the displayed colors.
 *
 * \retval LEDS_RC_OK Data transmitted successfully.
 * \retval LEDS_RC_TRANSMISSION_ERROR An error occurred during data transmission.
 */
enum LedsReturnCode leds_api_show(void);

#endif // LEDS_API_H
