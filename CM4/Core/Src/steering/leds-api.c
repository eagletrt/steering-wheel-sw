/*!
 * \file leds-api.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Implementation of the API for controlling the LED strip on the steering wheel, including functions for setting individual LED colors, filling the strip with a color, clearing the strip, and transmitting data to the LEDs.
 */

#include "ws2812b-api.h"
#include "leds-api.h"
#include "eagletrt-api.h"
#include <string.h>

#define LEDS_API_BRIGHTNESS_MAX (255U)

EAGLETRT_STATIC struct LedsHandler leds_handler;

enum LedsReturnCode leds_api_init(leds_transmit_callback transmit) {
    if (transmit == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    memset(&leds_handler, 0, sizeof(leds_handler));
    leds_handler.brightness = 1.0F;
    leds_handler.transmit_callback = transmit;
    return LEDS_RC_OK;
}

enum LedsReturnCode leds_api_set_led_color(enum LedsIndex index, struct LedColor color) {
    if (index >= LEDS_INDEX_COUNT) {
        return LEDS_RC_INVALID_LED;
    }

    leds_handler.colors[index] = color;
    return LEDS_RC_OK;
}

void leds_api_set_led_color_all(struct LedColor color) {
    for (size_t i = 0; i < LEDS_INDEX_COUNT; i++) {
        leds_handler.colors[i] = color;
    }
}

void leds_api_clear(void) {
    struct LedColor off = { 0, 0, 0 };
    leds_api_set_led_color_all(off);
}

void leds_api_set_brightness(float brightness) {
    leds_handler.brightness = EAGLETRT_API_CLAMP(brightness, 0.0F, 1.0F);
}

enum LedsReturnCode leds_api_show() {
    if (leds_handler.transmit_callback == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    enum WS2812BReturnCode encode_rc = ws2812b_api_encode(
        (const uint8_t *)leds_handler.colors,
        leds_handler.buffer,
        leds_handler.brightness,
        LEDS_INDEX_COUNT);

    if (encode_rc != WS2812B_RC_OK) {
        return LEDS_RC_TRANSMISSION_ERROR;
    }

    return leds_handler.transmit_callback(leds_handler.buffer, WS2812B_API_BUFFER_SIZE(LEDS_INDEX_COUNT));
}

void leds_api_save_pattern(void) {
    memcpy(leds_handler.colors_backup, leds_handler.colors, sizeof(leds_handler.colors));
}

void leds_api_restore_pattern(void) {
    memcpy(leds_handler.colors, leds_handler.colors_backup, sizeof(leds_handler.colors));
}

void leds_api_set_ptt_pattern(void) {
    struct LedColor blue = { .g = 0, .r = 0, .b = LEDS_API_BRIGHTNESS_MAX };
    for (size_t i = LEDS_INDEX_CENTER_0; i < LEDS_INDEX_CENTER_4 + 1; i++) {
        leds_handler.colors[i] = blue;
    }
}

void leds_api_set_target_lap_pattern(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = LEDS_INDEX_TOP_LEFT_1; i < LEDS_INDEX_TOP_RIGHT_1 + 1; i++) {
        leds_handler.colors[i] = off;
    }
}

void leds_api_set_fast_lap_pattern(void) {
    struct LedColor green = { .g = LEDS_API_BRIGHTNESS_MAX, .r = 0, .b = 0 };
    for (size_t i = LEDS_INDEX_TOP_LEFT_1; i < LEDS_INDEX_TOP_RIGHT_1 + 1; i++) {
        leds_handler.colors[i] = green;
    }
}

void leds_api_set_slow_lap_pattern(void) {
    struct LedColor yellow = { .g = LEDS_API_BRIGHTNESS_MAX, .r = LEDS_API_BRIGHTNESS_MAX, .b = 0 };
    for (size_t i = LEDS_INDEX_TOP_LEFT_1; i < LEDS_INDEX_TOP_RIGHT_1 + 1; i++) {
        leds_handler.colors[i] = yellow;
    }
}

void leds_api_set_error_pattern(void) {
    struct LedColor red = { .g = 0, .r = LEDS_API_BRIGHTNESS_MAX, .b = 0 };
    for (size_t i = LEDS_INDEX_CENTER_0; i < LEDS_INDEX_CENTER_4 + 1; i++) {
        leds_handler.colors[i] = red;
    }
}

void leds_api_set_ok_pattern(void) {
    struct LedColor off = { 0, 0, 0 };
    for (size_t i = LEDS_INDEX_CENTER_0; i < LEDS_INDEX_CENTER_4 + 1; i++) {
        leds_handler.colors[i] = off;
    }
}
