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
 *
 * This macro computes the total number of PWM duty cycle values needed to represent the color data for a specified number of LEDs, including the additional slots required for resetting the LEDs after data transmission.
 *
 * \param LED_COUNT The number of WS2812B LEDs to be encoded.
 *
 * \return The total buffer size in terms of the number of duty cycle values required to encode the color data for the specified number of LEDs, including reset slots.
 */
#define WS2812B_API_BUFFER_SIZE(LED_COUNT) ((LED_COUNT) * 24 + WS2812B_RESET_SLOTS)

/*!
 * \brief Macro to calculate the duty cycle value for a given frequency and ratio.
 *
 * This macro computes the duty cycle value based on the specified frequency and ratio, which is essential for generating the correct PWM signals for WS2812B LEDs.
 *
 * \param FREQ The frequency of the timer or PWM signal in Hz.
 * \param RATIO The duty cycle ratio (e.g., WS2812B_DUTY_0_RATIO or WS2812B_DUTY_1_RATIO).
 * \return The calculated duty cycle value as a 16-bit unsigned integer.
 */
#define WS2812B_API_CALCULATE_DUTY_VALUE(FREQ, RATIO) ((uint16_t)(((float)(FREQ) / WS2812B_FREQUENCY_HZ) * RATIO))

/*!
 * \brief Encodes GRB color data into a PWM buffer suitable for WS2812B LEDs.
 *
 * This function takes an array of GRB color values and converts them into a format that can be transmitted to WS2812B LEDs. The output buffer will contain the appropriate PWM values for each bit of the color data, as well as the necessary reset slots at the end.
 *
 * \param grb_color_buffer Pointer to an array of GRB color values (3 bytes per LED, in GRB order).
 * \param pwm_duty_out Pointer to an output buffer where the encoded PWM duty cycle values will be stored. The buffer must be large enough to hold the encoded data for all LEDs plus the reset slots (use WS2812B_API_BUFFER_SIZE(LED_COUNT) to calculate the required size).
 * \param brightness Brightness level for the LEDs (0-1), where 0 is off and 1 is full brightness (scaling is internal)
 * \param led_count The number of LEDs in the input array.
 *
 * \retval WS2812B_RC_OK Encoding successful.
 * \retval WS2812B_RC_NULL_POINTER A null pointer was passed for either the input color buffer or the output PWM buffer.
 * \retval WS2812B_RC_ENCODING_ERROR An error occurred during encoding (e.g., invalid color values).
 */
enum WS2812BReturnCode ws2812b_api_encode(const uint8_t *grb_color_buffer, enum WS2812BDutyCycle *pwm_duty_out, float brightness, size_t led_count);

#endif // WS2812B_API_H
