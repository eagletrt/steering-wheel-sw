#include "ws2812b-api.h"
#include "eagletrt.h"
#include <string.h>

EAGLETRT_STATIC void prv_ws2812b_encode(struct WS2812BHandler *handler) {
    size_t pwm_index = 0;

    for (size_t i = 0; i < handler->num_leds; i++) {
        uint8_t colors[3] = {
            handler->leds[i].g,
            handler->leds[i].r,
            handler->leds[i].b
        };

        for (size_t c = 0; c < 3; c++) {
            uint8_t byte = colors[c];
            for (int bit = 7; bit >= 0; bit--) {
                if (byte & (1 << bit)) {
                    handler->pwm_buffer[pwm_index++] = handler->duty_1;
                } else {
                    handler->pwm_buffer[pwm_index++] = handler->duty_0;
                }
            }
        }
    }

    // reset time
    handler->pwm_buffer[pwm_index++] = 0;
    handler->pwm_buffer[pwm_index++] = 0;
    handler->pwm_buffer[pwm_index++] = 0;
}

enum WS2812BReturnCode ws2812b_api_init(struct WS2812BHandler *handler, struct WS2812BColor *leds, uint16_t *pwm_buffer, uint16_t num_leds, uint16_t duty_0, uint16_t duty_1, ws2812b_transmit_callback transmit) {
    if (handler == NULL || leds == NULL || pwm_buffer == NULL || transmit == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }
    if (num_leds == 0) {
        return WS2812B_RC_INVALID_LED;
    }

    handler->leds = leds;
    handler->pwm_buffer = pwm_buffer;
    handler->num_leds = num_leds;
    handler->duty_0 = duty_0;
    handler->duty_1 = duty_1;
    handler->transmit = transmit;

    memset(handler->leds, 0, num_leds * sizeof(struct WS2812BColor)); // Initialize all LEDs to off
    memset(handler->pwm_buffer, 0, num_leds * 24 * sizeof(uint16_t)); // Clear PWM buffer

    return WS2812B_RC_OK;
}

enum WS2812BReturnCode ws2812b_api_set_led(struct WS2812BHandler *handler, size_t index, struct WS2812BColor color) {
    if (handler == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }
    if (index >= handler->num_leds) {
        return WS2812B_RC_INVALID_LED;
    }

    handler->leds[index] = color;
    return WS2812B_RC_OK;
}

enum WS2812BReturnCode ws2812b_api_fill(struct WS2812BHandler *handler, struct WS2812BColor color) {
    if (handler == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }

    for (size_t i = 0; i < handler->num_leds; i++) {
        handler->leds[i] = color;
    }
    return WS2812B_RC_OK;
}

enum WS2812BReturnCode ws2812b_api_clear(struct WS2812BHandler *handler) {
    if (handler == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }

    memset(handler->leds, 0, handler->num_leds * sizeof(struct WS2812BColor)); // Set all LEDs to off
    return WS2812B_RC_OK;
}

enum WS2812BReturnCode ws2812b_api_show(struct WS2812BHandler *handler) {
    if (handler == NULL) {
        return WS2812B_RC_NULL_POINTER;
    }

    prv_ws2812b_encode(handler);

    size_t size = handler->num_leds * 24 + 3;
    return handler->transmit(handler->pwm_buffer, size);
}
