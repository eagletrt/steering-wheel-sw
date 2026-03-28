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

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(uint32_t, fake_get_tick_hz);

void setUp(void) {
    RESET_FAKE(fake_get_tick_hz);
    FFF_RESET_HISTORY();
    fake_get_tick_hz_fake.return_val = TEST_FREQUENCY_HZ;
    ws2812b_api_init(&ws2812b_handler, fake_get_tick_hz);
}

EAGLETRT_STATIC void encode_byte(uint8_t value, uint16_t *out, float brightness) {
    uint8_t scaled_value = WS2812BGammaCorrectionTable[value] * brightness;
    for (int i = 7; i >= 0; i--) {
        out[7 - i] = EAGLETRT_API_BIT_GET(scaled_value, i) ? ws2812b_handler.duty_1 : ws2812b_handler.duty_0;
    }
}

EAGLETRT_STATIC uint16_t calculate_expected_duty(float duty_ratio) {
    uint32_t arr = (TEST_FREQUENCY_HZ / WS2812B_FREQUENCY_HZ) - 1;
    return (uint16_t)((arr + 1) * duty_ratio);
}

/*!
 * \defgroup ws2812b_api_init ws2812b_api_init tests
 * \{
 */

void test_ws2812b_api_init_success(void) {
    RESET_FAKE(fake_get_tick_hz);
    fake_get_tick_hz_fake.return_val = TEST_FREQUENCY_HZ;
    enum WS2812BReturnCode rc = ws2812b_api_init(&ws2812b_handler, fake_get_tick_hz);
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_get_tick_hz_fake.call_count, "ws2812b_api_init should call get_tick_hz exactly once");
    TEST_ASSERT_EQUAL_MESSAGE(calculate_expected_duty(WS2812B_DUTY_0_RATIO), ws2812b_handler.duty_0, "handler.duty_0 should be calculated based on the tick frequency");
    TEST_ASSERT_EQUAL_MESSAGE(calculate_expected_duty(WS2812B_DUTY_1_RATIO), ws2812b_handler.duty_1, "handler.duty_1 should be calculated based on the tick frequency");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, rc, "ws2812b_api_init should return WS2812B_RC_OK on successful initialization");
}

void test_ws2812b_api_init_null_handler(void) {
    enum WS2812BReturnCode rc = ws2812b_api_init(NULL, fake_get_tick_hz);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_api_init_null_get_tick_hz(void) {
    enum WS2812BReturnCode rc = ws2812b_api_init(&ws2812b_handler, NULL);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
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
    uint16_t expected[ONE_LED_OUTPUT_SIZE] = { 0 };
    float brightness = 1.0f;
    enum WS2812BReturnCode rc = ws2812b_api_encode(NULL, brightness, input, output, 1);
    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(
        expected,
        output,
        ONE_LED_OUTPUT_SIZE,
        "Output buffer should remain unchanged when handler is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_NULL_POINTER, rc, "ws2812b_api_encode should return WS2812B_RC_NULL_POINTER when handler is NULL");
}

void test_ws2812b_encode_null_input(void) {
    uint16_t output[ONE_LED_OUTPUT_SIZE] = { 0 };
    uint16_t expected[ONE_LED_OUTPUT_SIZE] = { 0 };
    float brightness = 1.0f;
    enum WS2812BReturnCode rc = ws2812b_api_encode(&ws2812b_handler, brightness, NULL, output, 1);
    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(
        expected,
        output,
        ONE_LED_OUTPUT_SIZE,
        "Output buffer should remain unchanged when input is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_NULL_POINTER, rc, "ws2812b_api_encode should return WS2812B_RC_NULL_POINTER when input is NULL");
}

void test_ws2812b_encode_null_output(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Green
    float brightness = 1.0f;
    enum WS2812BReturnCode rc = ws2812b_api_encode(&ws2812b_handler, brightness, input, NULL, 1);
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

    float brightness = 1.0f;

    size_t leds_num = 2;
    size_t out_size = WS2812B_API_BUFFER_SIZE(leds_num);

    uint16_t output[out_size];
    memset(output, 0, sizeof(output));

    uint16_t expected[out_size];
    for (size_t i = 0; i < leds_num; i++) {
        encode_byte(input[i * 3], &expected[i * ONE_LED_INPUT_SIZE * 8], brightness);
        encode_byte(input[i * 3 + 1], &expected[i * ONE_LED_INPUT_SIZE * 8 + 8], brightness);
        encode_byte(input[i * 3 + 2], &expected[i * ONE_LED_INPUT_SIZE * 8 + 16], brightness);
    }
    for (size_t i = leds_num * ONE_LED_INPUT_SIZE * 8; i < out_size; i++) {
        expected[i] = 0; // Reset slots
    }

    enum WS2812BReturnCode rc = ws2812b_api_encode(&ws2812b_handler, brightness, input, output, leds_num);
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

    float brightness = 1.0f;

    uint16_t output[ONE_LED_OUTPUT_SIZE] = { 0 };

    enum WS2812BReturnCode rc = ws2812b_api_encode(&ws2812b_handler, brightness, input, output, 1);

    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_0, output[0], "First bit of the first byte should be encoded as handler.duty_0");
    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_1, output[1], "Second bit of the first byte should be encoded as handler.duty_1");
    TEST_ASSERT_EQUAL_MESSAGE(ws2812b_handler.duty_0, output[15], "Last bit of the second byte should be encoded as handler.duty_0");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, rc, "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");
}

void test_ws2812b_encode_reset_slots_are_zero(void) {
    uint8_t input[3] = { 0x00, 0x00, 0x00 };
    float brightness = 1.0f;
    uint16_t output[ONE_LED_OUTPUT_SIZE] = { 0 };
    uint16_t expected[WS2812B_RESET_SLOTS] = { 0 };
    ws2812b_api_encode(&ws2812b_handler, brightness, input, output, 1);
    size_t start = 24;

    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(
        expected,
        &output[start],
        WS2812B_RESET_SLOTS,
        "Reset slots at the end of the output buffer should be set to 0");
}

void test_ws2812b_encode_brightness_scaling(void) {
    uint8_t input[3] = { 0xFF, 0xFF, 0xFF };
    float brightness = 0.5f; // 50% brightness
    uint8_t expected_scaled_value = WS2812BGammaCorrectionTable[0xFF];
    uint16_t expected_scaled_out[8] = { 0 };
    encode_byte(expected_scaled_value, expected_scaled_out, 0.5f);
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    ws2812b_api_encode(&ws2812b_handler, brightness, input, output, 1);
    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(
        expected_scaled_out,
        output,
        8,
        "Brightness scaling should correctly adjust the duty cycle for the first byte of the LED data");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ws2812b_api_init_success);
    RUN_TEST(test_ws2812b_api_init_null_handler);
    RUN_TEST(test_ws2812b_api_init_null_get_tick_hz);

    RUN_TEST(test_ws2812b_api_buffer_size_macro);
    RUN_TEST(test_ws2812b_api_buffer_size_macro_zero_leds);

    RUN_TEST(test_ws2812b_encode_null_input);
    RUN_TEST(test_ws2812b_encode_null_output);
    RUN_TEST(test_ws2812b_encode_two_leds);
    RUN_TEST(test_ws2812b_encode_msb_first);
    RUN_TEST(test_ws2812b_encode_reset_slots_are_zero);
    RUN_TEST(test_ws2812b_encode_brightness_scaling);

    return UNITY_END();
}
