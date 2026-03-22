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
 * \brief Encodes GRB color data into a PWM buffer suitable for WS2812B LEDs.
 *
 * This function takes an array of GRB color values and converts them into a format that can be transmitted to WS2812B LEDs. The output buffer will contain the appropriate PWM values for each bit of the color data, as well as the necessary reset slots at the end.
 *
 * \param brightness Brightness level for the LEDs (0-255), where 0 is off and 255 is full brightness (scaling is internal)
 * \param grb_color_buffer Pointer to an array of GRB color values (3 bytes per LED, in GRB order).
 * \param pwm_duty_out Pointer to a buffer where the encoded PWM values will be stored. The buffer must be large enough to hold the encoded data for all LEDs plus the reset slots.
 * \param num_leds The number of LEDs in the input array.
 *
 * \retval WS2812B_RC_OK Encoding successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for either the input color buffer or the output PWM buffer.
 */
enum WS2812BReturnCode ws2812b_encode(uint8_t brightness, const uint8_t *grb_color_buffer, uint16_t *pwm_duty_out, size_t num_leds);

#endif // WS2812B_API_H
