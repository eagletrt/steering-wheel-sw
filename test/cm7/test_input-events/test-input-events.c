/*!
 * \file test_input_events.c
 * \date 2026-03-14
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Test Input Events API functionality.
 */

#include "unity.h"
#include "fff.h"
#include "input-events-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_parameter_change_callback, enum InputsSharedParameterID, uint8_t);

extern struct InputEventHandler handler;

void setUp(void) {
    RESET_FAKE(test_parameter_change_callback);
    FFF_RESET_HISTORY();
}

/*!
 * \defgroup input_events_api_init Input Events API Initialization Tests
 * \{
 */

void test_input_events_api_init_should_store_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(test_parameter_change_callback, handler.on_parameter_change, "Parameter change callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
}

void test_input_events_api_init_should_fail_null_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(NULL);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup input_events_api_handle_event Input Events API Event Handling Tests
 * \{
 */

void test_input_events_api_handle_event_should_invoke_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .parameter_id = INPUTS_SHARED_PARAMETER_ID_POWER,
        .value = 7
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_parameter_change_callback_fake.call_count, "Parameter change callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_POWER, test_parameter_change_callback_fake.arg0_val, "Wrong parameter ID passed to callback");
    TEST_ASSERT_EQUAL_MESSAGE(7, test_parameter_change_callback_fake.arg1_val, "Wrong value passed to callback");
}

void test_input_events_api_handle_event_should_forward_all_parameter_ids(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");

    struct InputsSharedEvent event = {
        .parameter_id = INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG,
        .value = 1
    };
    input_events_api_handle_event(event);

    TEST_ASSERT_EQUAL_MESSAGE(1, test_parameter_change_callback_fake.call_count, "Parameter change callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, test_parameter_change_callback_fake.arg0_val, "Wrong parameter ID passed to callback");
    TEST_ASSERT_EQUAL_MESSAGE(1, test_parameter_change_callback_fake.arg1_val, "Wrong value passed to callback");
}

/*! \} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_input_events_api_init_should_store_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_callback);

    RUN_TEST(test_input_events_api_handle_event_should_invoke_callback);
    RUN_TEST(test_input_events_api_handle_event_should_forward_all_parameter_ids);

    return UNITY_END();
}
