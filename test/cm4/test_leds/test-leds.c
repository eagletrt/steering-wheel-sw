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

FAKE_VALUE_FUNC(enum LedsReturnCode, fake_transmit, const enum WS2812BDutyCycle *, uint16_t);

void setUp(void) {
    RESET_FAKE(fake_transmit);
    FFF_RESET_HISTORY();
    leds_api_init(fake_transmit);
}

/*!
 * \defgroup leds_api_init Test cases for leds_api_init function
 * \{
 */

void test_leds_api_init_success(void) {
    enum LedsReturnCode rc = leds_api_init(fake_transmit);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, leds_handler.transmit_callback, "leds_api_init should set the transmit callback correctly");
    TEST_ASSERT_EQUAL_MESSAGE(1.0f, leds_handler.brightness, "leds_api_init should set the default brightness to 1.0");
}

void test_leds_api_init_transmit_null_pointer(void) {
    enum LedsReturnCode rc = leds_api_init(NULL);
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
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.colors[0].r, "leds_handler.colors[0].r should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.colors[0].g, "leds_handler.colors[0].g should be set to the specified color value");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.colors[0].b, "leds_handler.colors[0].b should be set to the specified color value");
}

void test_leds_api_set_led_color_invalid_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 };                     // Red
    uint8_t expected_colors[LEDS_INDEX_COUNT] = { 0 };                        // Expect no changes to the colors array
    enum LedsReturnCode rc = leds_api_set_led_color(LEDS_INDEX_COUNT, color); // Out of range index
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_colors, leds_handler.colors, sizeof(expected_colors), "leds_handler.colors should remain unchanged when an invalid LED index is provided");
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_INVALID_LED, rc, "leds_api_set_led should return LEDS_RC_INVALID_LED when an out of range LED index is provided");
}

void test_leds_api_set_led_color_negative_index(void) {
    struct LedColor color = { .r = 255, .g = 0, .b = 0 };       // Red
    uint8_t expected_colors[LEDS_INDEX_COUNT] = { 0 };          // Expect no changes to the colors array
    enum LedsReturnCode rc = leds_api_set_led_color(-1, color); // Negative index
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_colors, leds_handler.colors, sizeof(expected_colors), "leds_handler.colors should remain unchanged when an invalid LED index is provided");
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_INVALID_LED, rc, "leds_api_set_led should return LEDS_RC_INVALID_LED when an out of range LED index is provided");
}

/*! \} */

/*!
 * \defgroup leds_api_fill Test cases for leds_api_fill function
 * \{
 */

void test_leds_api_set_led_color_all(void) {
    struct LedColor color = { .r = 0, .g = 255, .b = 0 }; // Green
    struct LedColor colors[LEDS_INDEX_COUNT] = { [0 ... LEDS_INDEX_COUNT - 1] = color };
    leds_api_set_led_color_all(color);
    TEST_ASSERT_EQUAL_UINT8_ARRAY(colors, leds_handler.colors, sizeof(colors));
}

/*! \} */

/*! 
 * \defgroup leds_api_clear Test cases for leds_api_clear function
 * \{
 */

void test_leds_api_clear(void) {
    memset(&leds_handler.colors, 0xff, sizeof(leds_handler.colors));
    struct LedColor colors[LEDS_INDEX_COUNT] = { 0 };

    leds_api_clear();
    TEST_ASSERT_EQUAL_UINT8_ARRAY(colors, leds_handler.colors, sizeof(colors));
}

/*! \} */

/*!
 * \defgroup leds_api_show Test cases for leds_api_show function
 * \{
 */

void test_leds_api_show_success(void) {
    fake_transmit_fake.return_val = LEDS_RC_OK;

    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_show should return LEDS_RC_OK when transmission is successful");
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "leds_api_show should call the transmit callback once");
}

void test_leds_api_show_null_transmit(void) {
    leds_handler.transmit_callback = NULL; // Simulate null transmit callback

    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_show_transmission_error(void) {
    fake_transmit_fake.return_val = LEDS_RC_TRANSMISSION_ERROR;

    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_TRANSMISSION_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup leds_api_set_brightness Test cases for leds_api_set_brightness function
 * \{
 */

void test_leds_api_set_brightness(void) {
    leds_api_set_brightness(1.0f);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1.0f, leds_handler.brightness, "leds_handler.brightness should be set to the specified value");
}

void test_leds_api_set_brightness_zero(void) {
    leds_api_set_brightness(0.0f);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0.0f, leds_handler.brightness, "leds_handler.brightness should be set to 0.0");
}

void test_leds_api_set_brightness_negative(void) {
    leds_api_set_brightness(-0.5f);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(0.0f, leds_handler.brightness, "leds_handler.brightness should be set to 0.0 when a negative value is provided");
}

void test_leds_api_set_brightness_above_one(void) {
    leds_api_set_brightness(1.5f);
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(1.0f, leds_handler.brightness, "leds_handler.brightness should be set to 1.0 when a value above 1.0 is provided");
}

/*! \} */

/*!
 * \defgroup Steering wheel specific LED patterns
 * \{
 */

void test_leds_api_set_ptt_pattern(void) {
    leds_api_set_ptt_pattern();
    struct LedColor expected_color = { .r = 0, .g = 0, .b = 255 };
    struct LedColor expected_colors[5] = { [0 ... 4] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, leds_handler.colors, sizeof(expected_colors));
}

void test_leds_api_set_target_lap_pattern(void) {
    leds_api_set_target_lap_pattern();
    struct LedColor expected_color = { .r = 0, .g = 0, .b = 0 };
    struct LedColor expected_colors[4] = { [0 ... 3] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, &leds_handler.colors[5], sizeof(expected_colors));
}

void test_leds_api_set_fast_lap_pattern(void) {
    leds_api_set_fast_lap_pattern();
    struct LedColor expected_color = { .r = 0, .g = 255, .b = 0 };
    struct LedColor expected_colors[4] = { [0 ... 3] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, &leds_handler.colors[5], sizeof(expected_colors));
}

void test_leds_api_set_slow_lap_pattern(void) {
    leds_api_set_slow_lap_pattern();
    struct LedColor expected_color = { .r = 255, .g = 255, .b = 0 };
    struct LedColor expected_colors[4] = { [0 ... 3] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, &leds_handler.colors[5], sizeof(expected_colors));
}

void test_leds_api_set_error_pattern(void) {
    leds_api_set_error_pattern();
    struct LedColor expected_color = { .r = 255, .g = 0, .b = 0 };
    struct LedColor expected_colors[5] = { [0 ... 4] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, leds_handler.colors, sizeof(expected_colors));
}

void test_leds_api_set_ok_pattern(void) {
    leds_api_set_ok_pattern();
    struct LedColor expected_color = { .r = 0, .g = 0, .b = 0 };
    struct LedColor expected_colors[5] = { [0 ... 4] = expected_color };
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_colors, leds_handler.colors, sizeof(expected_colors));
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_leds_api_init_success);
    RUN_TEST(test_leds_api_init_transmit_null_pointer);

    RUN_TEST(test_leds_api_set_led_color_valid_index);
    RUN_TEST(test_leds_api_set_led_color_invalid_index);
    RUN_TEST(test_leds_api_set_led_color_negative_index);

    RUN_TEST(test_leds_api_set_led_color_all);

    RUN_TEST(test_leds_api_clear);

    RUN_TEST(test_leds_api_show_success);
    RUN_TEST(test_leds_api_show_null_transmit);
    RUN_TEST(test_leds_api_show_transmission_error);

    RUN_TEST(test_leds_api_set_brightness);
    RUN_TEST(test_leds_api_set_brightness_zero);
    RUN_TEST(test_leds_api_set_brightness_negative);
    RUN_TEST(test_leds_api_set_brightness_above_one);

    RUN_TEST(test_leds_api_set_ptt_pattern);
    RUN_TEST(test_leds_api_set_target_lap_pattern);
    RUN_TEST(test_leds_api_set_fast_lap_pattern);
    RUN_TEST(test_leds_api_set_slow_lap_pattern);
    RUN_TEST(test_leds_api_set_error_pattern);
    RUN_TEST(test_leds_api_set_ok_pattern);

    return UNITY_END();
}
