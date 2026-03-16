#include "leds-api.h"
#include "ws2812b-api.h"

struct LedsHandler leds_handler;
struct WS2812BColor leds[LEDS_COUNT];
uint16_t pwm_buffer[LEDS_PWM_BUFFER_SIZE];

// TODO: implement the actual transmission logic and move this to timer logic
static enum WS2812BReturnCode leds_transmit_callback(const uint16_t *buffer, size_t size) {
    return WS2812B_RC_OK;
}

enum LedsReturnCode leds_api_init() {
    enum WS2812BReturnCode ws2812b_rc = ws2812b_api_init(&leds_handler.ws2812b_handler, leds, pwm_buffer, LEDS_COUNT, LEDS_PWM_DUTY_0, LEDS_PWM_DUTY_1, leds_transmit_callback);
    if (ws2812b_rc != WS2812B_RC_OK) {
        return LEDS_DRIVER_ERROR;
    }
    return LEDS_RC_OK;
}
