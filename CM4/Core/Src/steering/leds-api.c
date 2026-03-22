/*!
 * \file leds-api.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Implementation of the API for controlling the LED strip on the steering wheel, including functions for setting individual LED colors, filling the strip with a color, clearing the strip, and transmitting data to the LEDs.
 */

#include "leds-api.h"
#include "ws2812b-api.h"
#include "eagletrt.h"
#include <string.h>

EAGLETRT_STATIC struct LedsHandler leds_handler;

enum LedsReturnCode leds_api_init(leds_transmit_callback transmit) {
    if (transmit == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    memset(&leds_handler, 0, sizeof(leds_handler));
    leds_handler.transmit = transmit;
    return LEDS_RC_OK;
}

enum LedsReturnCode leds_api_set_led(enum LedsIndex index, struct LedColor color) {
    if (index < 0 || index >= LEDS_COUNT) {
        return LEDS_RC_INVALID_LED;
    }

    leds_handler.leds[index] = color;
    return LEDS_RC_OK;
}

void leds_api_fill(struct LedColor color) {
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        leds_handler.leds[i] = color;
    }
}

void leds_api_clear(void) {
    struct LedColor off = { 0, 0, 0 };
    leds_api_fill(off);
}

enum LedsReturnCode leds_api_show(uint8_t brightness) {
    if (leds_handler.transmit == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    enum WS2812BReturnCode encode_rc = ws2812b_encode(
        brightness,
        (const uint8_t *)leds_handler.leds,
        leds_handler.pwm_buffer,
        LEDS_COUNT);

    if (encode_rc != WS2812B_RC_OK) {
        return LEDS_RC_TRANSMISSION_ERROR;
    }

    enum LedsReturnCode transmit_rc = leds_handler.transmit(
        leds_handler.pwm_buffer,
        LEDS_PWM_BUFFER_SIZE);

    if (transmit_rc != LEDS_RC_OK) {
        return LEDS_RC_TRANSMISSION_ERROR;
    }

    return LEDS_RC_OK;
}

void leds_api_ptt(void) {
    struct LedColor blue = { .g = 0, .r = 0, .b = 255 };
    for (size_t i = 0; i < 5; i++) {
        leds_handler.leds[i] = blue;
    }
}

void leds_api_target_lap(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = 5; i < LEDS_COUNT; i++) {
        leds_handler.leds[i] = off;
    }
}

void leds_api_fast_lap(void) {
    struct LedColor green = { .g = 255, .r = 0, .b = 0 };
    for (size_t i = 5; i < LEDS_COUNT; i++) {
        leds_handler.leds[i] = green;
    }
}

void leds_api_slow_lap(void) {
    struct LedColor yellow = { .g = 255, .r = 255, .b = 0 };
    for (size_t i = 5; i < LEDS_COUNT; i++) {
        leds_handler.leds[i] = yellow;
    }
}

void leds_api_error(void) {
    struct LedColor red = { .g = 0, .r = 255, .b = 0 };
    for (size_t i = 0; i < 5; i++) {
        leds_handler.leds[i] = red;
    }
}

void leds_api_ok(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = 0; i < 5; i++) {
        leds_handler.leds[i] = off;
    }
}
