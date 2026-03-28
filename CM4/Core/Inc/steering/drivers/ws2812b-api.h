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
#define WS2812B_API_BUFFER_SIZE(LED_COUNT) ((LED_COUNT) * 24 + WS2812B_RESET_SLOTS)

/*!
 * \brief Initializes the WS2812B handler with timing parameters based on the provided tick frequency.
 *
 * This function calculates the duty cycle values for representing binary 0 and 1, as well as the number of reset slots required after transmitting data to the LEDs, based on the specified tick frequency. The calculated values are stored in the provided handler structure.
 *
 * \param handler Pointer to a WS2812BHandler structure.
 * \param transmit_callback A callback function that will be called to transmit data to the LEDs.
 * \param get_tick_hz_callback A callback function that returns the frequency of the timer ticks in Hz, which is used to calculate the duty cycle values for WS2812B data transmission.
 *
 * \retval WS2812B_RC_OK Initialization successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for the handler.
 */
enum WS2812BReturnCode ws2812b_api_init(struct WS2812BHandler *handler, ws2812b_get_tick_hz_callback get_tick_hz_callback);

/*!
 * \brief Encodes GRB color data into a PWM buffer suitable for WS2812B LEDs.
 *
 * This function takes an array of GRB color values and converts them into a format that can be transmitted to WS2812B LEDs. The output buffer will contain the appropriate PWM values for each bit of the color data, as well as the necessary reset slots at the end.
 *
 * \param handler Pointer to an initialized WS2812BHandler structure containing the timing parameters for encoding.
 * \param brightness Brightness level for the LEDs (0-1), where 0 is off and 1 is full brightness (scaling is internal)
 * \param grb_color_buffer Pointer to an array of GRB color values (3 bytes per LED, in GRB order).
 * \param pwm_duty_out Pointer to a buffer where the encoded PWM values will be stored. The buffer must be large enough to hold the encoded data for all LEDs plus the reset slots. You can use the WS2812B_API_BUFFER_SIZE macro to calculate the required size based on the number of LEDs.
 * \param led_count The number of LEDs in the input array.
 *
 * \retval WS2812B_RC_OK Encoding successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for either the input color buffer or the output PWM buffer.
 */
enum WS2812BReturnCode ws2812b_api_encode(struct WS2812BHandler *handler, float brightness, const uint8_t *grb_color_buffer, uint16_t *pwm_duty_out, size_t led_count);

#endif // WS2812B_API_H
