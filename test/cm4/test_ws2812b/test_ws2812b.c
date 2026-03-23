/*!
 * \file test_ws2812b.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the WS2812B encoding functionality.
 */

#include "unity.h"
#include "ws2812b-api.h"
#include "fff.h"
#include "eagletrt-api.h"
#include <stdio.h>

#define TEST_FREQUENCY_HZ (80000000U) // 80 MHz timer clock for testing
#define ONE_LED_INPUT_SIZE (3U)
#define ONE_LED_OUTPUT_SIZE (ONE_LED_INPUT_SIZE * 8 + WS2812B_RESET_SLOTS)

extern const uint8_t WS2812BGammaCorrectionTable[256];
struct WS2812BHandler ws2812b_handler;
enum WS2812BReturnCode init_rc;

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum WS2812BReturnCode, fake_transmit, struct WS2812BHandler *, const uint32_t *, uint16_t);
FAKE_VALUE_FUNC(uint32_t, fake_get_tick_hz);

void setUp(void) {
    RESET_FAKE(fake_transmit);
    RESET_FAKE(fake_get_tick_hz);
    FFF_RESET_HISTORY();
    fake_get_tick_hz_fake.return_val = TEST_FREQUENCY_HZ;
    init_rc = ws2812b_api_init(&ws2812b_handler, fake_transmit, fake_get_tick_hz);
}

static void encode_byte(uint8_t value, uint16_t *out, uint8_t brightness) {
    uint8_t scaled_value = ((uint16_t)WS2812BGammaCorrectionTable[value] * brightness) / 255;
    for (int i = 7; i >= 0; i--) {
        out[7 - i] = EAGLETRT_API_BIT_GET(scaled_value, i) ? ws2812b_handler.duty_1 : ws2812b_handler.duty_0;
    }
}

EAGLETRT_STATIC uint16_t calculate_expected_duty(uint32_t duty_ratio) {
    uint32_t arr = (TEST_FREQUENCY_HZ / WS2812B_FREQUENCY_HZ) - 1;
    return (uint16_t)((arr + 1) * duty_ratio) / 100;
}

/*!
 * \defgroup ws2812b_api_init ws2812b_api_init tests
 * \{
 */

void test_ws2812b_api_init_success(void) {
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, ws2812b_handler.transmit_callback, "handler.transmit_callback should be set to the provided transmit function");
    TEST_ASSERT_EQUAL_MESSAGE(calculate_expected_duty(WS2812B_DUTY_0_RATIO), ws2812b_handler.duty_0, "handler.duty_0 should be calculated based on the tick frequency");
    TEST_ASSERT_EQUAL_MESSAGE(calculate_expected_duty(WS2812B_DUTY_1_RATIO), ws2812b_handler.duty_1, "handler.duty_1 should be calculated based on the tick frequency");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, init_rc, "ws2812b_api_init should return WS2812B_RC_OK on successful initialization");
}

void test_ws2812b_api_init_null_handler(void) {
    enum WS2812BReturnCode rc = ws2812b_api_init(NULL, fake_transmit, fake_get_tick_hz);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_api_init_null_transmit(void) {
    enum WS2812BReturnCode rc = ws2812b_api_init(&ws2812b_handler, NULL, fake_get_tick_hz);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_api_init_null_get_tick_hz(void) {
    enum WS2812BReturnCode rc = ws2812b_api_init(&ws2812b_handler, fake_transmit, NULL);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_api_init_get_tick_hz_called(void) {
    TEST_ASSERT_EQUAL(1, fake_get_tick_hz_fake.call_count);
}

/*! \} */

/*!
 * \defgroup WS2812B_API_BUFFER_SIZE WS2812B_API_BUFFER_SIZE macro tests
 * \{
 */

void test_ws2812b_api_buffer_size_macro(void) {
    size_t num_leds = 5;
    TEST_ASSERT_EQUAL(170, WS2812B_API_BUFFER_SIZE(num_leds));
}

void test_ws2812b_api_buffer_size_macro_zero_leds(void) {
    size_t num_leds = 0;
    TEST_ASSERT_EQUAL(WS2812B_RESET_SLOTS, WS2812B_API_BUFFER_SIZE(num_leds));
}

/*! \} */

/*!
 * \defgroup ws2812b_encode ws2812b_encode tests
 * \{
 */

void test_ws2812b_encode_null_handler(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Green
    uint16_t output[ONE_LED_OUTPUT_SIZE] = { 0 };
    uint8_t brightness = 255;
    enum WS2812BReturnCode rc = ws2812b_encode(NULL, brightness, input, output, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_null_input(void) {
    uint16_t output[ONE_LED_OUTPUT_SIZE] = { 0 };
    uint8_t brightness = 255;
    enum WS2812BReturnCode rc = ws2812b_encode(&ws2812b_handler, brightness, NULL, output, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_null_output(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Green
    uint8_t brightness = 255;
    enum WS2812BReturnCode rc = ws2812b_encode(&ws2812b_handler, brightness, input, NULL, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_two_leds(void) {
    uint8_t input[6] = {
        0x00,
        0xFF,
        0x00,
        0xFF,
        0x00,
        0xFF
    };

    uint8_t brightness = 143;

    size_t leds_num = 2;
    size_t out_size = WS2812B_API_BUFFER_SIZE(leds_num);

    uint16_t output[out_size];
    memset(output, 0, sizeof(output));

    uint16_t expected[out_size];
    uint16_t *ptr = expected;

    encode_byte(0x00, ptr, brightness);
    ptr += 8;
    encode_byte(0xFF, ptr, brightness);
    ptr += 8;
    encode_byte(0x00, ptr, brightness);
    ptr += 8;

    encode_byte(0xFF, ptr, brightness);
    ptr += 8;
    encode_byte(0x00, ptr, brightness);
    ptr += 8;
    encode_byte(0xFF, ptr, brightness);
    ptr += 8;

    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        *ptr++ = 0;
    }

    enum WS2812BReturnCode rc = ws2812b_encode(&ws2812b_handler, brightness, input, output, leds_num);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, output, out_size, "ws2812b_encode should produce the correct PWM pattern for multiple LED input");
}

void test_ws2812b_encode_msb_first(void) {
    uint8_t input[3] = {
        0x80, /* 10000000 => gamma corrected => 01001001 */
        0x01, /* 00000001 => gamma corrected => 00000000 */
        0x00
    };

    uint8_t brightness = 255;

    uint16_t output[ONE_LED_OUTPUT_SIZE];

    ws2812b_encode(&ws2812b_handler, brightness, input, output, 1);

    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_0, output[0], "First bit of the first byte should be encoded as handler.duty_0");
    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_1, output[1], "Second bit of the first byte should be encoded as handler.duty_1");
    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_0, output[15], "Last bit of the second byte should be encoded as handler.duty_0");
}

void test_ws2812b_encode_reset_slots_are_zero(void) {
    uint8_t input[3] = { 0x00, 0x00, 0x00 };
    uint8_t brightness = 255;
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    ws2812b_encode(&ws2812b_handler, brightness, input, output, 1);
    size_t start = 24;

    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        char msg[50];
        snprintf(msg, sizeof(msg), "Reset slot %zu should be 0", i);
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(0, output[start + i], msg);
    }
}

void test_ws2812b_encode_brightness_scaling(void) {
    uint8_t input[3] = { 0xFF, 0xFF, 0xFF };
    uint8_t brightness = 128; // 50% brightness
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    ws2812b_encode(&ws2812b_handler, brightness, input, output, 1);
}

/*! \} */

/*!
 * \defgroup ws2812b_transmit ws2812b_transmit tests
 * \{
 */

void test_ws2812b_transmit_null_handler(void) {
    uint16_t pwm_buffer[10] = { 0 };
    enum WS2812BReturnCode rc = ws2812b_transmit(NULL, pwm_buffer, 10);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_transmit_null_pwm_buffer(void) {
    enum WS2812BReturnCode rc = ws2812b_transmit(&ws2812b_handler, NULL, 10);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_transmit_busy_handler(void) {
    ws2812b_handler.busy = true;
    uint16_t pwm_buffer[10] = { 0 };
    enum WS2812BReturnCode rc = ws2812b_transmit(&ws2812b_handler, pwm_buffer, 10);
    TEST_ASSERT_EQUAL(WS2812B_RC_BUSY, rc);
}

void test_ws2812b_transmit_success(void) {
    ws2812b_handler.busy = false;
    uint16_t pwm_buffer[10] = { 0 };
    enum WS2812BReturnCode rc = ws2812b_transmit(&ws2812b_handler, pwm_buffer, 10);
    TEST_ASSERT_EQUAL(WS2812B_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "ws2812b_transmit should call the transmit callback exactly once");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&ws2812b_handler, fake_transmit_fake.arg0_val, "ws2812b_transmit should call the transmit callback with the correct handler pointer");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(pwm_buffer, fake_transmit_fake.arg1_val, "ws2812b_transmit should call the transmit callback with the correct PWM buffer pointer");
    TEST_ASSERT_EQUAL_MESSAGE(10, fake_transmit_fake.arg2_val, "ws2812b_transmit should call the transmit callback with the correct length");
}

/*! \} */

/*!
 * \defgroup ws2812b_set_busy ws2812b_set_busy tests
 * \{
 */

void test_ws2812b_set_busy_null_handler(void) {
    enum WS2812BReturnCode rc = ws2812b_set_busy(NULL, true);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_set_busy_true(void) {
    enum WS2812BReturnCode rc = ws2812b_set_busy(&ws2812b_handler, true);
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, rc, "ws2812b_set_busy should return WS2812B_RC_OK when setting busy to true");
    TEST_ASSERT_TRUE_MESSAGE(ws2812b_handler.busy, "handler.busy should be set to true after calling ws2812b_set_busy with busy = true");
}

void test_ws2812b_set_busy_false(void) {
    enum WS2812BReturnCode rc = ws2812b_set_busy(&ws2812b_handler, false);
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, rc, "ws2812b_set_busy should return WS2812B_RC_OK when setting busy to false");
    TEST_ASSERT_FALSE_MESSAGE(ws2812b_handler.busy, "handler.busy should be set to false after calling ws2812b_set_busy with busy = false");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ws2812b_api_init_success);
    RUN_TEST(test_ws2812b_api_init_null_handler);
    RUN_TEST(test_ws2812b_api_init_null_transmit);
    RUN_TEST(test_ws2812b_api_init_null_get_tick_hz);
    RUN_TEST(test_ws2812b_api_init_get_tick_hz_called);

    RUN_TEST(test_ws2812b_api_buffer_size_macro);
    RUN_TEST(test_ws2812b_api_buffer_size_macro_zero_leds);

    RUN_TEST(test_ws2812b_encode_null_input);
    RUN_TEST(test_ws2812b_encode_null_output);
    RUN_TEST(test_ws2812b_encode_two_leds);
    RUN_TEST(test_ws2812b_encode_msb_first);
    RUN_TEST(test_ws2812b_encode_reset_slots_are_zero);
    RUN_TEST(test_ws2812b_encode_brightness_scaling);

    RUN_TEST(test_ws2812b_transmit_null_handler);
    RUN_TEST(test_ws2812b_transmit_null_pwm_buffer);
    RUN_TEST(test_ws2812b_transmit_busy_handler);
    RUN_TEST(test_ws2812b_transmit_success);

    RUN_TEST(test_ws2812b_set_busy_null_handler);
    RUN_TEST(test_ws2812b_set_busy_true);
    RUN_TEST(test_ws2812b_set_busy_false);

    return UNITY_END();
}
