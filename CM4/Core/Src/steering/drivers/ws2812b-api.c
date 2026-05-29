/*!
 * \file ws2812b-api.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Implementation of the API for encoding GRB color data into a PWM buffer suitable for WS2812B LEDs, including handling brightness scaling and gamma correction.
 */

#include "ws2812b-api.h"
#include "eagletrt-api.h"
#include "ws2812b.h"
#include <string.h>

// clang-format off

/*!
 * \brief Gamma correction table for 8-bit color values.
 */
EAGLETRT_STATIC const uint8_t WS2812B_gamma_correction_table[256] = {
    0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 11, 12, 12, 13, 13, 14, 14,
    15, 16, 16, 17, 17, 18, 19, 19, 20, 20, 20, 21, 21, 22, 22, 22,
    23, 23, 24, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 29, 29,
    30, 30, 31, 32, 32, 33, 33, 34, 34, 35, 35, 36, 37, 37, 38, 39,
    39, 40, 40, 41, 42, 42, 43, 44, 44, 45, 46, 47, 47, 48, 49, 49,
    50, 51, 52, 53, 53, 54, 55, 56, 56, 57, 58, 59, 60, 61, 62, 62,
    63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77,
    78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 91, 92, 93, 94,
    95, 96, 97, 98, 99, 101, 102, 103, 104, 105, 106, 108, 109, 110,
    111, 112, 114, 115, 116, 117, 119, 120, 121, 122, 124, 125, 126,
    128, 129, 130, 132, 133, 134, 136, 137, 138, 140, 141, 143, 144,
    145, 147, 148, 150, 151, 152, 154, 155, 157, 158, 160, 161, 163,
    164, 166, 167, 169, 170, 172, 173, 175, 177, 178, 180, 181, 183,
    184, 186, 188, 189, 191, 193, 194, 196, 198, 199, 201, 203, 204,
    206, 208, 209, 211, 213, 215, 216, 218, 220, 222, 223, 225, 227,
    229, 230, 232, 234, 236, 238, 240, 241, 243, 245, 247, 249, 251,
    253, 255
};
// clang-format on

enum WS2812BReturnCode ws2812b_api_encode(
    const uint8_t *grb_color_buffer,
    enum WS2812BDutyCycle *pwm_duty_out,
    float brightness,
    size_t led_count) {

    if (grb_color_buffer == NULL || pwm_duty_out == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }

    size_t out_idx = 0;
    size_t in_idx = 0;

    brightness = EAGLETRT_API_CLAMP(brightness, 0.0F, 1.0F);

    for (size_t led = 0; led < led_count; led++) {
        // 3 bytes per LED: G, R, B
        for (int channel = 0; channel < 3; channel++) {
            uint8_t value = WS2812B_gamma_correction_table[grb_color_buffer[in_idx]] * brightness;
            in_idx++;
            // MSB first
            constexpr uint8_t bit_index = 7;
            for (int bit = bit_index; bit >= 0; bit--) {
                pwm_duty_out[out_idx] =
                    EAGLETRT_API_BIT_GET(value, bit) ? WS2812B_DUTY_CYCLE_1 : WS2812B_DUTY_CYCLE_0;
                out_idx++;
            }
        }
    }

    // reset (low for >50µs)
    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        pwm_duty_out[out_idx] = WS2812B_DUTY_CYCLE_RESET;
        out_idx++;
    }

    if (out_idx != WS2812B_API_BUFFER_SIZE(led_count)) {
        // This should never happen, but we can check to be sure we filled the expected number of slots
        return WS2812B_RC_ENCODING_ERROR;
    }

    return WS2812B_RC_OK;
}
