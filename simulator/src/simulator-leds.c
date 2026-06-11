/*!
 * \file simulator-leds.c
 * \date 2026-06-10
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Implementation of the WS2812B duty-cycle decoder.
 */

#include "simulator-leds.h"
#include "eagletrt.h"

#define SIMULATOR_LEDS_BITS_PER_LED (24U) /* 3 bytes (GRB) * 8 bits */

EAGLETRT_STATIC struct LedColor simulator_leds_decoded[LEDS_INDEX_COUNT];

void simulator_leds_decode(const enum WS2812BDutyCycle *buffer, uint16_t length) {
    if (buffer == NULL) {
        return;
    }
    const uint16_t bits_needed = (uint16_t)(LEDS_INDEX_COUNT * SIMULATOR_LEDS_BITS_PER_LED);
    if (length < bits_needed) {
        return;
    }

    uint16_t in_idx = 0U;
    for (uint16_t led = 0U; led < LEDS_INDEX_COUNT; led++) {
        uint8_t channel_bytes[3] = { 0U, 0U, 0U };
        for (uint8_t channel = 0U; channel < 3U; channel++) {
            uint8_t value = 0U;
            for (int8_t bit = 7; bit >= 0; bit--) {
                if (buffer[in_idx] == WS2812B_DUTY_CYCLE_1) {
                    value = (uint8_t)(value | (uint8_t)(1U << (uint8_t)bit));
                }
                in_idx++;
            }
            channel_bytes[channel] = value;
        }
        simulator_leds_decoded[led].g = channel_bytes[0];
        simulator_leds_decoded[led].r = channel_bytes[1];
        simulator_leds_decoded[led].b = channel_bytes[2];
    }
}

const struct LedColor *simulator_leds_get_colors(void) {
    return simulator_leds_decoded;
}
