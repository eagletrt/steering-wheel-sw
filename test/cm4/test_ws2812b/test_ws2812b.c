#include "unity.h"
#include "ws2812b-api.h"
#include <string.h>
#include <stdio.h>

#define ONE_LED_INPUT_SIZE 3
#define ONE_LED_OUTPUT_SIZE (ONE_LED_INPUT_SIZE * 8 + WS2812B_RESET_SLOTS)

static void encode_byte(uint8_t value, uint16_t *out) {
    for (int bit = 7; bit >= 0; bit--) {
        *out++ = (value & (1 << bit)) ? WS2812B_DUTY_1 : WS2812B_DUTY_0;
    }
}

/*!
 * \defgroup ws2812b_encode ws2812b_encode tests
 * \{
 */

void test_ws2812b_encode_null_input(void) {
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    enum WS2812BReturnCode rc = ws2812b_encode(NULL, output, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_null_output(void) {
    uint8_t input[ONE_LED_INPUT_SIZE] = { 0xFF, 0x00, 0x00 }; // Red
    enum WS2812BReturnCode rc = ws2812b_encode(input, NULL, 1);
    TEST_ASSERT_EQUAL(WS2812B_RC_NULL_POINTER, rc);
}

void test_ws2812b_encode_single_led_exact_pattern(void) {
    uint8_t input[3] = {
        0xAA,
        0x55,
        0xFF
    };

    uint16_t output[ONE_LED_OUTPUT_SIZE];
    memset(output, 0xFFFF, sizeof(output));

    enum WS2812BReturnCode rc = ws2812b_encode(input, output, 1);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

    uint16_t expected[ONE_LED_OUTPUT_SIZE];
    uint16_t *ptr = expected;

    encode_byte(0xAA, ptr);
    ptr += 8;
    encode_byte(0x55, ptr);
    ptr += 8;
    encode_byte(0xFF, ptr);
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

    size_t leds = 2;
    size_t out_size = leds * 24 + WS2812B_RESET_SLOTS;

    uint16_t output[out_size];
    memset(output, 0, sizeof(output));

    enum WS2812BReturnCode rc = ws2812b_encode(input, output, leds);
    TEST_ASSERT_EQUAL_MESSAGE(
        WS2812B_RC_OK,
        rc,
        "ws2812b_encode should return WS2812B_RC_OK for valid input and output pointers");

    uint16_t expected[out_size];
    uint16_t *ptr = expected;

    encode_byte(0x00, ptr);
    ptr += 8;
    encode_byte(0xFF, ptr);
    ptr += 8;
    encode_byte(0x00, ptr);
    ptr += 8;

    encode_byte(0xFF, ptr);
    ptr += 8;
    encode_byte(0x00, ptr);
    ptr += 8;
    encode_byte(0xFF, ptr);
    ptr += 8;

    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        *ptr++ = 0;
    }

    TEST_ASSERT_EQUAL_UINT16_ARRAY_MESSAGE(expected, output, out_size, "ws2812b_encode should produce the correct PWM pattern for multiple LED input");
}

void test_ws2812b_encode_msb_first(void) {
    uint8_t input[3] = {
        0x80, /* 10000000 */
        0x01, /* 00000001 */
        0x00
    };

    uint16_t output[ONE_LED_OUTPUT_SIZE];

    ws2812b_encode(input, output, 1);

    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_1, output[0], "First bit of the first byte should be encoded as WS2812B_DUTY_1");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_0, output[1], "Second bit of the first byte should be encoded as WS2812B_DUTY_0");
    TEST_ASSERT_EQUAL_MESSAGE(WS2812B_DUTY_1, output[15], "Last bit of the second byte should be encoded as WS2812B_DUTY_1");
}

void test_ws2812b_encode_reset_slots_are_zero(void) {
    uint8_t input[3] = { 0x00, 0x00, 0x00 };
    uint16_t output[ONE_LED_OUTPUT_SIZE];
    ws2812b_encode(input, output, 1);
    size_t start = 24;

    for (size_t i = 0; i < WS2812B_RESET_SLOTS; i++) {
        char msg[50];
        snprintf(msg, sizeof(msg), "Reset slot %zu should be 0", i);
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(0, output[start + i], msg);
    }
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

    return UNITY_END();
}
