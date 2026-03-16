#ifndef WS2812B_API_H
#define WS2812B_API_H

#include "ws2812b.h"

/*!
 * \brief Initializes the WS2812B handler with the specified LED array and number of LEDs.
 *
 * \param handler Pointer to the WS2812BHandler structure to be initialized.
 * \param leds Pointer to an array of WS2812BColor structures representing the LED colors.
 * \param num_leds Number of LEDs in the strip.
 * \param duty_0 PWM duty cycle value representing a '0' bit for the WS2812B protocol.
 * \param duty_1 PWM duty cycle value representing a '1' bit for the WS2812B protocol.
 * \param transmit Function pointer for transmitting data to the LEDs.
 *
 * \retval WS2812B_RC_OK Initialization successful.
 * \retval WS2812B_RC_INVALID_LED The number of LEDs is zero.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler, LEDs or transmit callback.
 */
enum WS2812BReturnCode ws2812b_api_init(struct WS2812BHandler *handler, struct WS2812BColor *leds, uint16_t *pwm_buffer, uint16_t num_leds, uint16_t duty_0, uint16_t duty_1, ws2812b_transmit_callback transmit);

/*!
 * \brief Sets the color of a specific LED in the strip.
 *
 * \param handler Pointer to the WS2812BHandler structure.
 * \param index Index of the LED to be set (0-based).
 * \param color WS2812BColor structure representing the desired color for the LED.
 *
 * \retval WS2812B_RC_OK Color set successfully.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 * \retval WS2812B_RC_INVALID_LED The specified LED index is out of range.
 */
enum WS2812BReturnCode ws2812b_api_set_led(struct WS2812BHandler *handler, size_t index, struct WS2812BColor color);

/*!
 * \brief Fills the entire LED strip with a specific color.
 *
 * \param handler Pointer to the WS2812BHandler structure.
 * \param color WS2812BColor structure representing the desired color for all LEDs.
 *
 * \retval WS2812B_RC_OK Strip filled successfully.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 */
enum WS2812BReturnCode ws2812b_api_fill(struct WS2812BHandler *handler, struct WS2812BColor color);

/*!
 * \brief Clears the LED strip by setting all LEDs to off (black).
 *
 * \param handler Pointer to the WS2812BHandler structure.
 *
 * \retval WS2812B_RC_OK Strip cleared successfully.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 */
enum WS2812BReturnCode ws2812b_api_clear(struct WS2812BHandler *handler);

/*!
 * \brief Transmits the current LED color data to the WS2812B strip.
 *
 * \param handler Pointer to the WS2812BHandler structure.
 *
 * \retval WS2812B_RC_OK Data transmitted successfully.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 * \retval WS2812B_RC_TRANSMISSION_ERROR An error occurred during data transmission.
 */
enum WS2812BReturnCode ws2812b_api_show(struct WS2812BHandler *handler);

#endif // WS2812B_API_H
