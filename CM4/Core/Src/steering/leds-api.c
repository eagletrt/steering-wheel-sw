#include "leds-api.h"
#include "ws2812b-api.h"
#include <string.h>

struct LedsHandler leds_handler;

enum LedsReturnCode leds_api_init(leds_transmit_callback transmit) {
    if (transmit == NULL) {
        return LEDS_RC_NULL_POINTER;
    }

    leds_handler.transmit = transmit;
    memset(leds_handler.leds, 0, sizeof(leds_handler.leds));
    memset(leds_handler.pwm_buffer, 0, sizeof(leds_handler.pwm_buffer));

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

enum LedsReturnCode leds_api_show(void) {
    enum WS2812BReturnCode encode_rc = ws2812b_encode(
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
