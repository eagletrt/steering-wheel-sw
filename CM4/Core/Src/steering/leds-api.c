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

enum LedsReturnCode leds_api_init(ws2812b_pwm_transmit_callback transmit, ws2812b_get_tick_hz_callback get_tick_hz) {
    if (transmit == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    memset(&leds_handler, 0, sizeof(leds_handler));
    leds_handler.brightness = 255;
    if (ws2812b_api_init(&leds_handler.ws2812b_handler, transmit, get_tick_hz) != WS2812B_RC_OK) {
        return LEDS_RC_NULL_POINTER;
    }
    return LEDS_RC_OK;
}

enum LedsReturnCode leds_api_set_led_color(enum LedsIndex index, struct LedColor color) {
    if (index < 0 || index >= LEDS_COUNT) {
        return LEDS_RC_INVALID_LED;
    }

    leds_handler.leds[index] = color;
    return LEDS_RC_OK;
}

void leds_api_set_led_color_all(struct LedColor color) {
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        leds_handler.leds[i] = color;
    }
}

void leds_api_clear(void) {
    struct LedColor off = { 0, 0, 0 };
    leds_api_set_led_color_all(off);
}

void leds_api_set_brightness(uint8_t brightness) {
    leds_handler.brightness = brightness;
}

enum LedsReturnCode leds_api_show() {
    enum WS2812BReturnCode encode_rc = ws2812b_encode(
        &leds_handler.ws2812b_handler,
        leds_handler.brightness,
        (const uint8_t *)leds_handler.leds,
        leds_handler.buffer,
        LEDS_COUNT);

    if (encode_rc != WS2812B_RC_OK) {
        return LEDS_RC_TRANSMISSION_ERROR;
    }

    enum WS2812BReturnCode transmit_rc = ws2812b_transmit(&leds_handler.ws2812b_handler, leds_handler.buffer, WS2812B_API_BUFFER_SIZE(LEDS_COUNT));

    if (transmit_rc != WS2812B_RC_OK) {
        return LEDS_RC_TRANSMISSION_ERROR;
    }

    return LEDS_RC_OK;
}

void leds_api_set_ptt_pattern(void) {
    struct LedColor blue = { .g = 0, .r = 0, .b = 255 };
    for (size_t i = LEDS_CENTER_0; i < LEDS_CENTER_4 + 1; i++) {
        leds_handler.leds[i] = blue;
    }
}

void leds_api_set_target_lap_pattern(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = LEDS_TOP_LEFT_1; i < LEDS_TOP_RIGHT_1 + 1; i++) {
        leds_handler.leds[i] = off;
    }
}

void leds_api_set_fast_lap_pattern(void) {
    struct LedColor green = { .g = 255, .r = 0, .b = 0 };
    for (size_t i = LEDS_TOP_LEFT_1; i < LEDS_TOP_RIGHT_1 + 1; i++) {
        leds_handler.leds[i] = green;
    }
}

void leds_api_set_slow_lap_pattern(void) {
    struct LedColor yellow = { .g = 255, .r = 255, .b = 0 };
    for (size_t i = LEDS_TOP_LEFT_1; i < LEDS_TOP_RIGHT_1 + 1; i++) {
        leds_handler.leds[i] = yellow;
    }
}

void leds_api_set_error_pattern(void) {
    struct LedColor red = { .g = 0, .r = 255, .b = 0 };
    for (size_t i = LEDS_CENTER_0; i < LEDS_CENTER_4 + 1; i++) {
        leds_handler.leds[i] = red;
    }
}

void leds_api_set_ok_pattern(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = LEDS_CENTER_0; i < LEDS_CENTER_4 + 1; i++) {
        leds_handler.leds[i] = off;
    }
}
