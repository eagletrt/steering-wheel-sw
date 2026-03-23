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
#include "ws2812b-api.h"
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
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, init_rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, leds_handler.ws2812b_handler.transmit_callback, "leds_handler.ws2812b_handler.transmit_callback should be set to the provided transmit function");
}

void test_leds_api_init_transmit_null_pointer(void) {
    enum LedsReturnCode rc = leds_api_init(NULL, fake_get_tick_hz);
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_init_get_tick_hz_null_pointer(void) {
    enum LedsReturnCode rc = leds_api_init(fake_transmit, NULL);
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_set_led_color Test cases for leds_api_set_led_color function
 * \{
 */

void test_leds_api_set_led_color_valid_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 }; // Red
    enum LedsReturnCode rc = leds_api_set_led_color(0, color);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_set_led should return LEDS_RC_OK when setting a valid LED index");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[0].r, "leds_handler.leds[0].r should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[0].g, "leds_handler.leds[0].g should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[0].b, "leds_handler.leds[0].b should be set to the specified color value");
}

void test_leds_api_set_led_color_invalid_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 };               // Red
    enum LedsReturnCode rc = leds_api_set_led_color(LEDS_COUNT, color); // Out of range index
    TEST_ASSERT_EQUAL(LEDS_RC_INVALID_LED, rc);
}

void test_leds_api_set_led_color_negative_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 };       // Red
    enum LedsReturnCode rc = leds_api_set_led_color(-1, color); // Negative index
    TEST_ASSERT_EQUAL(LEDS_RC_INVALID_LED, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_fill Test cases for leds_api_fill function
 * \{
 */

void test_leds_api_set_led_color_all(void) {
    struct LedColor color = { .r = 0, .g = 255, .b = 0 }; // Green
    leds_api_set_led_color_all(color);
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
    memset(&leds_handler.leds, 0xff, sizeof(leds_handler.leds));

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
    fake_transmit_fake.return_val = WS2812B_RC_OK;
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_show should return LEDS_RC_OK when transmission is successful");
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "leds_api_show should call the transmit function exactly once");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&leds_handler.ws2812b_handler, fake_transmit_fake.arg0_val, "leds_api_show should call the transmit function with the correct handler pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(leds_handler.buffer, fake_transmit_fake.arg1_val, "leds_api_show should call the transmit function with the correct PWM buffer");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_API_BUFFER_SIZE(LEDS_COUNT), fake_transmit_fake.arg2_val, "leds_api_show should call the transmit function with the correct buffer size");
}

void test_leds_api_show_null_transmit(void) {
    memset(&leds_handler, 0, sizeof(leds_handler));
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_show_transmission_error(void) {
    fake_transmit_fake.return_val = WS2812B_RC_TRANSMISSION_ERROR;
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_TRANSMISSION_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_set_brightness Test cases for leds_api_set_brightness function
 * \{
 */

void test_leds_api_set_brightness(void) {
    leds_api_set_brightness(128);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(128, leds_handler.brightness, "leds_handler.brightness should be set to the specified value");
}

/*! \} */

/*!
 * \defgroup Steering wheel specific LED patterns
 * \{
 */

void test_leds_api_set_ptt_pattern(void) {
    leds_api_set_ptt_pattern();
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 0 for PTT indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for PTT indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 255 for PTT indication");
    }
}

void test_leds_api_set_target_lap_pattern(void) {
    leds_api_set_target_lap_pattern();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 0 for target lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for target lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for target lap indication");
    }
}

void test_leds_api_set_fast_lap_pattern(void) {
    leds_api_set_fast_lap_pattern();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for fast lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for fast lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for fast lap indication");
    }
}

void test_leds_api_set_slow_lap_pattern(void) {
    leds_api_set_slow_lap_pattern();
    for (size_t i = 5; i < 9; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for slow lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 255 for slow lap indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for slow lap indication");
    }
}

void test_leds_api_set_error_pattern(void) {
    leds_api_set_error_pattern();
    for (size_t i = 0; i < 5; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.leds[i].r, "leds_handler.leds[i].r should be set to 255 for error indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds[i].g should be set to 0 for error indication");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds[i].b should be set to 0 for error indication");
    }
}

void test_leds_api_set_ok_pattern(void) {
    leds_api_set_ok_pattern();
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
    RUN_TEST(test_leds_api_init_transmit_null_pointer);
    RUN_TEST(test_leds_api_init_get_tick_hz_null_pointer);

    RUN_TEST(test_leds_api_set_led_color_valid_index);
    RUN_TEST(test_leds_api_set_led_color_invalid_index);
    RUN_TEST(test_leds_api_set_led_color_negative_index);

    RUN_TEST(test_leds_api_set_led_color_all);

    RUN_TEST(test_leds_api_clear);

    RUN_TEST(test_leds_api_show_success);
    RUN_TEST(test_leds_api_show_null_transmit);
    RUN_TEST(test_leds_api_show_transmission_error);

    RUN_TEST(test_leds_api_set_brightness);

    RUN_TEST(test_leds_api_set_ptt_pattern);
    RUN_TEST(test_leds_api_set_target_lap_pattern);
    RUN_TEST(test_leds_api_set_fast_lap_pattern);
    RUN_TEST(test_leds_api_set_slow_lap_pattern);
    RUN_TEST(test_leds_api_set_error_pattern);
    RUN_TEST(test_leds_api_set_ok_pattern);

    return UNITY_END();
}
