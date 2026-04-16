/*!
 * \file test_ws2812b.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the WS2812B encoding functionality.
 */

#include "unity.h"
#include "ws2812b-api.h"
#include "eagletrt-api.h"
#include <stdio.h>
#include <string.h>

#define TEST_FREQUENCY_HZ (80000000U) // 80 MHz timer clock for testing
#define ONE_LED_INPUT_SIZE (3U)
#define ONE_LED_OUTPUT_SIZE (ONE_LED_INPUT_SIZE * 8 + WS2812B_RESET_SLOTS)

extern const uint8_t WS2812BGammaCorrectionTable[256];

EAGLETRT_STATIC void encode_byte(uint8_t value, enum WS2812BDutyCycle *out, float brightness) {
    uint8_t scaled_value = WS2812BGammaCorrectionTable[value] * brightness;
    for (int i = 7; i >= 0; i--) {
        out[7 - i] = EAGLETRT_API_BIT_GET(scaled_value, i) ? WS2812B_DUTY_CYCLE_1 : WS2812B_DUTY_CYCLE_0;
    }
}

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
 * \defgroup WS2812B_API_CALCULATE_DUTY_VALUE WS2812B_API_CALCULATE_DUTY_VALUE macro tests
 * \{
 */

void test_ws2812b_api_calculate_duty_value_macro_0(void) {
    uint16_t duty_val_0 = WS2812B_API_CALCULATE_DUTY_VALUE(TEST_FREQUENCY_HZ, WS2812B_DUTY_0_RATIO);
    uint16_t expected_0 = 35;
    TEST_ASSERT_EQUAL_UINT16(expected_0, duty_val_0);
}

void test_ws2812b_api_calculate_duty_value_macro_1(void) {
    uint16_t duty_val_1 = WS2812B_API_CALCULATE_DUTY_VALUE(TEST_FREQUENCY_HZ, WS2812B_DUTY_1_RATIO);
    uint16_t expected_1 = 70;
    TEST_ASSERT_EQUAL_UINT16(expected_1, duty_val_1);
}

/*! \} */

/*!
 * \defgroup ws2812b_encode ws2812b_encode tests
 * \{
 */

void test_ws2812b_encode_null_input(void) {
    enum WS2812BDutyCycle output[ONE_LED_OUTPUT_SIZE] = { 0 };
    enum WS2812BDutyCycle expected[ONE_LED_OUTPUT_SIZE] = { 0 };
    float brightness = 1.0f;
    enum WS2812BReturnCode rc = ws2812b_api_encode( NULL, output, brightness, 1);
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(
        expected,
        output,
        ONE_LED_OUTPUT_SIZE,
        "Output buffer should remain unchanged when input is NULL");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_NULL_POINTER, rc, "ws2812b_api_encode should return WS2812B_RC_NULL_POINTER when input is NULL");
}

void test_ws2812b_encode_null_output(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Green
    float brightness = 1.0f;
    enum WS2812BReturnCode rc = ws2812b_api_encode(input, NULL, brightness, 1);
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

    enum WS2812BDutyCycle output[out_size];
    memset(output, 0, sizeof(output));

    enum WS2812BDutyCycle expected[out_size];
    for (size_t i = 0; i < leds_num; i++) {
        encode_byte(input[i * 3], &expected[i * ONE_LED_INPUT_SIZE * 8], brightness);
        encode_byte(input[i * 3 + 1], &expected[i * ONE_LED_INPUT_SIZE * 8 + 8], brightness);
        encode_byte(input[i * 3 + 2], &expected[i * ONE_LED_INPUT_SIZE * 8 + 16], brightness);
    }
    for (size_t i = leds_num * ONE_LED_INPUT_SIZE * 8; i < out_size; i++) {
        expected[i] = WS2812B_DUTY_CYCLE_RESET; // Reset slots
    }

    enum WS2812BReturnCode rc = ws2812b_api_encode(input, output, brightness, leds_num);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(expected, output, out_size, "ws2812b_encode should produce the correct PWM pattern for multiple LED input");
}

void test_ws2812b_encode_msb_first(void) {
    uint8_t input[3] = {
        0x80,
        0x01,
        0x00,
    };

    float brightness = 1.0f;

    enum WS2812BDutyCycle output[ONE_LED_OUTPUT_SIZE] = { 0 };

    enum WS2812BReturnCode rc = ws2812b_api_encode(input, output, brightness, 1);

    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_CYCLE_0, output[0], "First bit of the first byte should be encoded as WS2812B_DUTY_CYCLE_0");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_CYCLE_1, output[1], "Second bit of the first byte should be encoded as WS2812B_DUTY_CYCLE_1");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_CYCLE_0, output[15], "Last bit of the second byte should be encoded as WS2812B_DUTY_CYCLE_0");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, rc, "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");
}

void test_ws2812b_encode_reset_slots_are_zero(void) {
    uint8_t input[3] = { 0x00, 0x00, 0x00 };
    float brightness = 1.0f;
    enum WS2812BDutyCycle output[ONE_LED_OUTPUT_SIZE] = { 0 };
    enum WS2812BDutyCycle expected[WS2812B_RESET_SLOTS] = { [0 ... WS2812B_RESET_SLOTS - 1] = WS2812B_DUTY_CYCLE_RESET };
    ws2812b_api_encode(input, output, brightness, 1);
    size_t start = 24;

    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(
        expected,
        &output[start],
        WS2812B_RESET_SLOTS,
        "Reset slots at the end of the output buffer should be set to WS2812B_DUTY_CYCLE_RESET");
}

void test_ws2812b_encode_brightness_scaling(void) {
    uint8_t input[3] = { 0xFF, 0xFF, 0xFF };
    float brightness = 0.5f; // 50% brightness
    uint8_t expected_scaled_value = WS2812BGammaCorrectionTable[0xFF];
    enum WS2812BDutyCycle expected_scaled_out[8] = { 0 };
    encode_byte(expected_scaled_value, expected_scaled_out, 0.5f);
    enum WS2812BDutyCycle output[ONE_LED_OUTPUT_SIZE];
    ws2812b_api_encode(input, output, brightness, 1);
    TEST_ASSERT_EQUAL_UINT32_ARRAY_MESSAGE(
        expected_scaled_out,
        output,
        8,
        "Brightness scaling should correctly adjust the duty cycle for the first byte of the LED data");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ws2812b_api_buffer_size_macro);
    RUN_TEST(test_ws2812b_api_buffer_size_macro_zero_leds);

    RUN_TEST(test_ws2812b_api_calculate_duty_value_macro_0);
    RUN_TEST(test_ws2812b_api_calculate_duty_value_macro_1);

    RUN_TEST(test_ws2812b_encode_null_input);
    RUN_TEST(test_ws2812b_encode_null_output);
    RUN_TEST(test_ws2812b_encode_two_leds);
    RUN_TEST(test_ws2812b_encode_msb_first);
    RUN_TEST(test_ws2812b_encode_reset_slots_are_zero);
    RUN_TEST(test_ws2812b_encode_brightness_scaling);

    return UNITY_END();
}
