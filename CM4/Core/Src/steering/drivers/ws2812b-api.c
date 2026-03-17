#include "ws2812b-api.h"

enum WS2812BReturnCode ws2812b_encode(
    const uint8_t *input,
    uint16_t *output,
    size_t num_leds) {

    if (!input || !output)
        return WS2812B_RC_NULL_POINTER;

    size_t out_idx = 0;
    size_t in_idx = 0;

    for (size_t led = 0; led < num_leds; led++) {
        /* 3 bytes per LED: G, R, B */
        for (int byte = 0; byte < 3; byte++) {
            uint8_t value = input[in_idx++];
            /* MSB first */
            for (int bit = 7; bit >= 0; bit--) {
                output[out_idx++] =
                    (value & (1 << bit)) ? WS2812B_DUTY_1 : WS2812B_DUTY_0;
            }
        }
    }

    /* reset (low for >50µs) */
    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        output[out_idx++] = 0;
    }

    return WS2812B_RC_OK;
}
