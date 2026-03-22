/*!
 * \file ws2812b-api.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Implementation of the API for encoding GRB color data into a PWM buffer suitable for WS2812B LEDs, including handling brightness scaling and gamma correction.
 */

#include "ws2812b-api.h"
#include "eagletrt-api.h"

// clang-format off

/*!
 * \brief Gamma correction table for 8-bit color values.
 */
EAGLETRT_STATIC const uint8_t WS2812BGammaCorrectionTable[256] = {
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

enum WS2812BReturnCode ws2812b_encode(
    uint8_t brightness,
    const uint8_t *input,
    uint16_t *output,
    size_t num_leds) {

    if (input == NULL || output == NULL)
        return WS2812B_RC_NULL_POINTER;

    size_t out_idx = 0;
    size_t in_idx = 0;

    for (size_t led = 0; led < num_leds; led++) {
        /* 3 bytes per LED: G, R, B */
        for (int byte = 0; byte < 3; byte++) {
            uint8_t value = ((uint16_t)WS2812BGammaCorrectionTable[input[in_idx]] * brightness) / 255;
            in_idx++;
            /* MSB first */
            for (int bit = 7; bit >= 0; bit--) {
                output[out_idx] =
                    EAGLETRT_API_BIT_GET(value, bit) ? WS2812B_DUTY_1 : WS2812B_DUTY_0;
                out_idx++;
            }
        }
    }

    /* reset (low for >50µs) */
    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        output[out_idx] = 0;
        out_idx++;
    }

    return WS2812B_RC_OK;
}
