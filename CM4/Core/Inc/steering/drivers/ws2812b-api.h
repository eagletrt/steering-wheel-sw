/*!
 * \file ws2812b-api.h
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief This file defines the API for encoding GRB color data into a PWM buffer suitable for WS2812B LEDs, including handling brightness scaling and gamma correction.
 */

#ifndef WS2812B_API_H
#define WS2812B_API_H

#include "ws2812b.h"

/*!
 * \brief Macro to calculate the required buffer size for encoding a given number of WS2812B LEDs.
 */
#define WS2812B_API_BUFFER_SIZE(num_leds) ((num_leds) * 24 + WS2812B_RESET_SLOTS)

/*!
 * \brief Initializes the WS2812B handler with timing parameters based on the provided tick frequency.
 *
 * This function calculates the duty cycle values for representing binary 0 and 1, as well as the number of reset slots required after transmitting data to the LEDs, based on the specified tick frequency. The calculated values are stored in the provided handler structure.
 *
 * \param handler Pointer to a WS2812BHandler structure.
 * \param transmit_callback A callback function that will be called to transmit the PWM data to the WS2812B LEDs. This function should handle the actual transmission of the PWM buffer to the LEDs.
 * \param get_tick_hz_callback A callback function that returns the frequency of the timer ticks in Hz, which is used to calculate the duty cycle values for WS2812B data transmission.
 *
 * \retval WS2812B_RC_OK Initialization successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 */
enum WS2812BReturnCode ws2812b_api_init(struct WS2812BHandler *handler, ws2812b_pwm_transmit_callback transmit_callback, ws2812b_get_tick_hz_callback get_tick_hz_callback);

/*!
 * \brief Encodes GRB color data into a PWM buffer suitable for WS2812B LEDs.
 *
 * This function takes an array of GRB color values and converts them into a format that can be transmitted to WS2812B LEDs. The output buffer will contain the appropriate PWM values for each bit of the color data, as well as the necessary reset slots at the end.
 *
 * \param handler Pointer to an initialized WS2812BHandler structure containing the timing parameters for encoding.
 * \param brightness Brightness level for the LEDs (0-255), where 0 is off and 255 is full brightness (scaling is internal)
 * \param grb_color_buffer Pointer to an array of GRB color values (3 bytes per LED, in GRB order).
 * \param pwm_duty_out Pointer to a buffer where the encoded PWM values will be stored. The buffer must be large enough to hold the encoded data for all LEDs plus the reset slots.
 * \param num_leds The number of LEDs in the input array.
 *
 * \retval WS2812B_RC_OK Encoding successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for either the input color buffer or the output PWM buffer.
 */
enum WS2812BReturnCode ws2812b_encode(struct WS2812BHandler *handler, uint8_t brightness, const uint8_t *grb_color_buffer, uint16_t *pwm_duty_out, size_t num_leds);

/*!
 * \brief Transmits the encoded PWM buffer to the WS2812B LEDs using the handler's transmit callback.
 *
 * This function calls the transmit callback function provided in the handler to send the encoded PWM data to the WS2812B LEDs. It is important to ensure that the handler is not currently busy transmitting data before calling this function.
 *
 * \param handler Pointer to a WS2812BHandler structure that has been initialized and contains a valid transmit callback.
 * \param pwm_buffer Pointer to the buffer containing the PWM duty cycle values to be transmitted to the LEDs. This buffer should have been filled by a previous call to ws2812b_encode.
 * \param length The number of PWM duty cycle values in the buffer, which should correspond to the number of LEDs multiplied by 24 (for 24 bits per LED) plus the reset slots.
 *
 * \retval WS2812B_RC_OK Transmission successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for either the handler or the PWM buffer.
 */
enum WS2812BReturnCode ws2812b_transmit(struct WS2812BHandler *handler, uint16_t *pwm_buffer, size_t length);

/*!
 * \brief Sets the busy flag in the handler to indicate whether it is currently transmitting data.
 *
 * \param handler Pointer to a WS2812BHandler structure.
 * \param busy Boolean value indicating whether the handler is busy (true) or not (false
 */
enum WS2812BReturnCode ws2812b_set_busy(struct WS2812BHandler *handler, bool busy);

#endif // WS2812B_API_H
