#include "unity.h"
#include "fff.h"
#include "leds-api.h"
#include <string.h>

extern struct LedsHandler leds_handler;

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum LedsReturnCode, fake_transmit, const uint16_t *, size_t);

void setUp(void) {
    RESET_FAKE(fake_transmit);
    FFF_RESET_HISTORY();
    memset(&leds_handler, 0, sizeof(leds_handler));
}

/*!
 * \defgroup leds_api_init Test cases for leds_api_init function
 * @{
 */

void test_leds_api_init_success(void) {
    enum LedsReturnCode rc = leds_api_init(fake_transmit);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(fake_transmit, leds_handler.transmit, "leds_handler.transmit should be set to the provided transmit function");
}

void test_leds_api_init_null_pointer(void) {
    enum LedsReturnCode rc = leds_api_init(NULL);
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_init_clears_leds(void) {
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        leds_handler.leds[i].r = 255;
        leds_handler.leds[i].g = 255;
        leds_handler.leds[i].b = 255;
    }
    for (size_t i = 0; i < LEDS_PWM_BUFFER_SIZE; i++) {
        leds_handler.pwm_buffer[i] = 0xFFFF;
    }

    enum LedsReturnCode rc = leds_api_init(fake_transmit);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_init should return LEDS_RC_OK on successful initialization");
    for (size_t i = 0; i < LEDS_COUNT; i++) {
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].r, "leds_handler.leds should be cleared to 0 on initialization");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].g, "leds_handler.leds should be cleared to 0 on initialization");
        TEST_ASSERT_EQUAL_UINT8_MESSAGE(0, leds_handler.leds[i].b, "leds_handler.leds should be cleared to 0 on initialization");
    }
    for (size_t i = 0; i < LEDS_PWM_BUFFER_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT16_MESSAGE(0, leds_handler.pwm_buffer[i], "leds_handler.pwm_buffer should be cleared to 0 on initialization");
    }
}

/*! @} */

/*!
 * \defgroup leds_api_set_led Test cases for leds_api_set_led function
 * @{
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

/*! @} */

/*!
 * \defgroup leds_api_fill Test cases for leds_api_fill function
 * @{
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

/*! @} */

/*! 
 * \defgroup leds_api_clear Test cases for leds_api_clear function
 * @{
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

/*! @} */

/*!
 * \defgroup leds_api_show Test cases for leds_api_show function
 * @{
 */

void test_leds_api_show_success(void) {
    enum LedsReturnCode rc_init = leds_api_init(fake_transmit);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc_init, "leds_api_init should return LEDS_RC_OK on successful initialization");
    fake_transmit_fake.return_val = LEDS_RC_OK;
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc, "leds_api_show should return LEDS_RC_OK when transmission is successful");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(1, fake_transmit_fake.call_count, "leds_api_show should call the transmit function exactly once");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(leds_handler.pwm_buffer, fake_transmit_fake.arg0_val, "leds_api_show should call the transmit function with the correct PWM buffer");
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_PWM_BUFFER_SIZE, fake_transmit_fake.arg1_val, "leds_api_show should call the transmit function with the correct buffer size");
}

void test_leds_api_show_null_transmit(void) {
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_NULL_POINTER, rc);
}

void test_leds_api_show_transmission_error(void) {
    enum LedsReturnCode rc_init = leds_api_init(fake_transmit);
    TEST_ASSERT_EQUAL_MESSAGE(LEDS_RC_OK, rc_init, "leds_api_init should return LEDS_RC_OK on successful initialization");
    fake_transmit_fake.return_val = LEDS_RC_TRANSMISSION_ERROR;
    enum LedsReturnCode rc = leds_api_show();
    TEST_ASSERT_EQUAL(LEDS_RC_TRANSMISSION_ERROR, rc);
}

/*! @} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_leds_api_init_success);
    RUN_TEST(test_leds_api_init_null_pointer);
    RUN_TEST(test_leds_api_init_clears_leds);

    RUN_TEST(test_leds_api_set_led_valid_index);
    RUN_TEST(test_leds_api_set_led_invalid_index);
    RUN_TEST(test_leds_api_set_led_negative_index);

    RUN_TEST(test_leds_api_fill);

    RUN_TEST(test_leds_api_clear);

    RUN_TEST(test_leds_api_show_success);
    RUN_TEST(test_leds_api_show_null_transmit);
    RUN_TEST(test_leds_api_show_transmission_error);

    return UNITY_END();
}
