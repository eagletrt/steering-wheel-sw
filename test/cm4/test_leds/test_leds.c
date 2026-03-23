/*!
 * \file test_leds.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the LEDs API functionality.
 */

#include "unity.h"
#include "fff.h"
#include "leds-api.h"
#include <string.h>

extern struct LedsHandler leds_handler;

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum WS2812BReturnCode, fake_transmit, struct WS2812BHandler *, const uint32_t *, uint16_t);
FAKE_VALUE_FUNC(uint32_t, fake_get_tick_hz);

enum LedsReturnCode init_rc;

void setUp(void) {
    RESET_FAKE(fake_transmit);
    RESET_FAKE(fake_get_tick_hz);
    FFF_RESET_HISTORY();
    init_rc = leds_api_init(fake_transmit, fake_get_tick_hz);
}

/*!
 * \defgroup leds_api_init Test cases for leds_api_init function
 * \{
 */

void test_leds_api_init_success(void) {
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, leds_handler.transmit, "leds_handler.transmit should be set to the provided transmit function");
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds should be cleared to 0 on initialization");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds should be cleared to 0 on initialization");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds should be cleared to 0 on initialization");
    }
    for (size_t i = 0; i < WS2812B_API_BUFFER_SIZE(LEDS_COUNT); i++) {
        TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, leds_handler.buffer[i], "leds_handler.pwm_buffer should be cleared to 0 on initialization");
    }
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, init_rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
}

void test_leds_api_init_null_pointer(void) {
    enum LedsReturnCode rc = leds_api_init(NULL);
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_set_led Test cases for leds_api_set_led function
 * \{
 */

void test_leds_api_set_led_valid_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 }; // Red
    enum LedsReturnCode rc = leds_api_set_led(0, color);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_set_led should return LEDS_RC_OK when setting a valid LED index");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[0].r, "leds_handler.leds[0].r should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[0].g, "leds_handler.leds[0].g should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[0].b, "leds_handler.leds[0].b should be set to the specified color value");
}

void test_leds_api_set_led_invalid_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 };         // Red
    enum LedsReturnCode rc = leds_api_set_led(LEDS_COUNT, color); // Out of range index
    TEST_ASSERT_EQUAL(LEDS_RC_INVALID_LED, rc);
}

void test_leds_api_set_led_negative_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 }; // Red
    enum LedsReturnCode rc = leds_api_set_led(-1, color); // Negative index
    TEST_ASSERT_EQUAL(LEDS_RC_INVALID_LED, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_fill Test cases for leds_api_fill function
 * \{
 */

void test_leds_api_fill(void) {
    struct LedColor color = { .r = 0, .g = 255, .b = 0 }; // Green
    leds_api_fill(color);
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to the specified color value");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to the specified color value");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to the specified color value");
    }
}

/*! \} */

/*! 
 * \defgroup leds_api_clear Test cases for leds_api_clear function
 * \{
 */

void test_leds_api_clear(void) {
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        leds_handler.leds[i].r = 255;
        leds_handler.leds[i].g = 255;
        leds_handler.leds[i].b = 255;
    }

    leds_api_clear();
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be cleared to 0");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be cleared to 0");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be cleared to 0");
    }
}

/*! \} */

/*!
 * \defgroup leds_api_show Test cases for leds_api_show function
 * \{
 */

void test_leds_api_show_success(void) {
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, init_rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    fake_transmit_fake.return_val = LEDS_RC_OK;
    enum LedsReturnCode rc = leds_api_show(128);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_show should return LEDS_RC_OK when transmission is successful");
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "leds_api_show should call the transmit function exactly once");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(leds_handler.pwm_buffer, fake_transmit_fake.arg0_val, "leds_api_show should call the transmit function with the correct PWM buffer");
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_PWM_BUFFER_SIZE, fake_transmit_fake.arg1_val, "leds_api_show should call the transmit function with the correct buffer size");
}

void test_leds_api_show_null_transmit(void) {
    memset(&leds_handler, 0, sizeof(leds_handler));
    enum LedsReturnCode rc = leds_api_show(128);
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_show_transmission_error(void) {
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, init_rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    fake_transmit_fake.return_val = LEDS_RC_TRANSMISSION_ERROR;
    enum LedsReturnCode rc = leds_api_show(128);
    TEST_ASSERT_EQUAL(LEDS_RC_TRANSMISSION_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup Steering wheel specific LED patterns
 * \{
 */

void test_leds_api_ptt(void) {
    leds_api_ptt();
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 0 for PTT indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for PTT indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 255 for PTT indication");
    }
}

void test_leds_api_target_lap(void) {
    leds_api_target_lap();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 0 for target lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for target lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for target lap indication");
    }
}

void test_leds_api_fast_lap(void) {
    leds_api_fast_lap();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for fast lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for fast lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for fast lap indication");
    }
}

void test_leds_api_slow_lap(void) {
    leds_api_slow_lap();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for slow lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 255 for slow lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for slow lap indication");
    }
}

void test_leds_api_error(void) {
    leds_api_error();
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for error indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for error indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for error indication");
    }
}

void test_leds_api_ok(void) {
    leds_api_ok();
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 0 for OK indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 255 for OK indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for OK indication");
    }
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_leds_api_init_success);
    RUN_TEST(test_leds_api_init_null_pointer);

    RUN_TEST(test_leds_api_set_led_valid_index);
    RUN_TEST(test_leds_api_set_led_invalid_index);
    RUN_TEST(test_leds_api_set_led_negative_index);

    RUN_TEST(test_leds_api_fill);

    RUN_TEST(test_leds_api_clear);

    RUN_TEST(test_leds_api_show_success);
    RUN_TEST(test_leds_api_show_null_transmit);
    RUN_TEST(test_leds_api_show_transmission_error);

    RUN_TEST(test_leds_api_ptt);
    RUN_TEST(test_leds_api_target_lap);
    RUN_TEST(test_leds_api_fast_lap);
    RUN_TEST(test_leds_api_slow_lap);
    RUN_TEST(test_leds_api_error);
    RUN_TEST(test_leds_api_ok);

    return UNITY_END();
}
