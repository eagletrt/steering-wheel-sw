/*!
 * \file test_ws2812b.c
 * \date 2026-03-18
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the WS2812B encoding functionality.
 */

#include "unity.h"
#include "ws2812b-api.h"
#include <string.h>
#include <stdio.h>
#include "fff.h"

#define ONE_LED_INPUT_SIZE 3
#define ONE_LED_OUTPUT_SIZE (ONE_LED_INPUT_SIZE * 8 + WS2812B_RESET_SLOTS)

extern const uint8_t WS2812BGammaCorrectionTable[256];
struct WS2812BHandler handler;
enum WS2812BReturnCode init_rc;

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum WS2812BReturnCode, fake_transmit, struct WS2812BHandler *, const uint32_t *, uint16_t);
FAKE_VALUE_FUNC(uint32_t, fake_get_tick_hz);

void setUp(void) {
    RESET_FAKE(fake_transmit);
    RESET_FAKE(fake_get_tick_hz);
    FFF_RESET_HISTORY();
    fake_get_tick_hz_fake.return_val = 160000;
    init_rc = ws2812b_api_init(&handler, fake_transmit, fake_get_tick_hz);
}

static void encode_byte(uint8_t value, uint16_t *out, uint8_t brightness) {
    uint8_t scaled_value = ((uint16_t)WS2812BGammaCorrectionTable[value] * brightness) / 255;
    for (int i = 7; i >= 0; i--) {
        out[7 - i] = (scaled_value & (1 << i)) ? WS2812B_DUTY_1 : WS2812B_DUTY_0;
    }
}

/*!
 * \defgroup ws2812b_api_init ws2812b_api_init tests
 * \{
 */

void test_ws2812b_api_init_success(void) {
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, handler.transmit_callback, "handler.transmit_callback should be set to the provided transmit function");
    TEST_ASSERT_EQUAL_MESSAGE(, handler.duty_0, "handler.duty_0 should be calculated based on the tick frequency");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_RC_OK, init_rc, "ws2812b_api_init should return WS2812B_RC_OK on successful initialization");

/*! \} */

/*!
 * \defgroup ws2812b_encode ws2812b_encode tests
 * \{
 */

void test_ws2812b_encode_null_input(void) {
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    uint8_t brightness = 255;
    enum WS2812BReturnCode rc = ws2812b_encode(brightness, NULL, output, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_null_output(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Red
    uint8_t brightness = 255;
    enum WS2812BReturnCode rc = ws2812b_encode(brightness, input, NULL, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_single_led_exact_pattern(void) {
    uint8_t input[3] = {
        0xAA,
        0x55,
        0xFF
    };

    uint8_t brightness = 255;

    uint16_t output[ONE_LED_OUTPUT_SIZE];
    memset(output, 0xFFFF, sizeof(output));

    enum WS2812BReturnCode rc = ws2812b_encode(brightness, input, output, 1);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

    uint16_t expected[ONE_LED_OUTPUT_SIZE];
    uint16_t *ptr = expected;

    encode_byte(0xAA, ptr, brightness);
    ptr += 8;
    encode_byte(0x55, ptr, brightness);
    ptr += 8;
    encode_byte(0xFF, ptr, brightness);
    ptr += 8;

    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        *ptr++ = 0;
    }

    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, output, ONE_LED_OUTPUT_SIZE, "ws2812b_encode should produce the correct PWM pattern for a single LED input");
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

    size_t leds = 2;
    size_t out_size = leds * 24 + WS2812B_RESET_SLOTS;

    uint16_t output[out_size];
    memset(output, 0, sizeof(output));

    enum WS2812BReturnCode rc = ws2812b_encode(brightness, input, output, leds);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

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

    ws2812b_encode(brightness, input, output, 1);

    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_0, output[0], "First bit of the first byte should be encoded as WS2812B_DUTY_1");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_1, output[1], "Second bit of the first byte should be encoded as WS2812B_DUTY_0");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_0, output[15], "Last bit of the second byte should be encoded as WS2812B_DUTY_1");
}

void test_ws2812b_encode_reset_slots_are_zero(void) {
    uint8_t input[3] = { 0x00, 0x00, 0x00 };
    uint8_t brightness = 255;
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    ws2812b_encode(brightness, input, output, 1);
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
    ws2812b_encode(brightness, input, output, 1);
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ws2812b_encode_null_input);
    RUN_TEST(test_ws2812b_encode_null_output);
    RUN_TEST(test_ws2812b_encode_single_led_exact_pattern);
    RUN_TEST(test_ws2812b_encode_two_leds);
    RUN_TEST(test_ws2812b_encode_msb_first);
    RUN_TEST(test_ws2812b_encode_reset_slots_are_zero);
    RUN_TEST(test_ws2812b_encode_brightness_scaling);

    return UNITY_END();
}
