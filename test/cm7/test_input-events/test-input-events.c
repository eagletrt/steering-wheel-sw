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

FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_button_press_callback, enum InputsSharedButtonID);
FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_button_long_press_callback, enum InputsSharedButtonID);
FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_button_release_callback, enum InputsSharedButtonID);
FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_knob_rotation_callback, enum InputsSharedKnobID, int8_t);
FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_parameter_change_callback, enum InputsSharedParameterID, uint8_t);

extern struct InputEventHandler handler;

void setUp(void) {
    RESET_FAKE(test_button_press_callback);
    RESET_FAKE(test_button_long_press_callback);
    RESET_FAKE(test_button_release_callback);
    RESET_FAKE(test_knob_rotation_callback);
    RESET_FAKE(test_parameter_change_callback);
    FFF_RESET_HISTORY();
}

/*!
 * \defgroup input_events_api_init Input Events API Initialization Tests
 * \{
 */

void test_input_events_api_init_should_store_callbacks(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(test_button_press_callback, handler.on_button_press, "Button press callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(test_button_long_press_callback, handler.on_button_long_press, "Button long press callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(test_button_release_callback, handler.on_button_release, "Button release callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(test_knob_rotation_callback, handler.on_knob_rotation, "Knob rotation callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(test_parameter_change_callback, handler.on_parameter_change, "Parameter change callback was not stored correctly");
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
}

void test_input_events_api_init_should_fail_null_button_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(NULL, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

void test_input_events_api_init_should_fail_null_long_press_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, NULL, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

void test_input_events_api_init_should_fail_null_release_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, NULL, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

void test_input_events_api_init_should_fail_null_rotation_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, NULL, test_parameter_change_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

void test_input_events_api_init_should_fail_null_parameter_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, NULL);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup input_events_api_handle_event Input Events API Event Handling Tests
 * \{
 */

void test_input_events_api_handle_event_should_invoke_button_press_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_PRESS,
        .button = {
            .button_id = INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1 }
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_button_press_callback_fake.call_count, "Button press callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1, test_button_press_callback_fake.arg0_val, "Wrong parameter passed to button press callback");
}

void test_input_events_api_handle_event_should_invoke_button_long_press_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
        .button = {
            .button_id = INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1 }
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_button_long_press_callback_fake.call_count, "Button long press callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1, test_button_long_press_callback_fake.arg0_val, "Wrong parameter passed to button long press callback");
}

void test_input_events_api_handle_event_should_invoke_button_release_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_RELEASE,
        .button = {
            .button_id = INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1 }
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_button_release_callback_fake.call_count, "Button release callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_BUTTON_ID_TOP_LEFT_1, test_button_release_callback_fake.arg0_val, "Wrong parameter passed to button release callback");
}

void test_input_events_api_handle_event_should_invoke_knob_rotation_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_KNOB_ROTATION,
        .knob = {
            .knob_id = INPUTS_SHARED_KNOB_ID_FRONT_LEFT,
            .delta = 5 }
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_knob_rotation_callback_fake.call_count, "Knob rotation callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, test_knob_rotation_callback_fake.arg0_val, "Wrong knob ID passed to knob rotation callback");
    TEST_ASSERT_EQUAL_MESSAGE(5, test_knob_rotation_callback_fake.arg1_val, "Wrong delta passed to knob rotation callback");
}

void test_input_events_api_handle_event_should_invoke_parameter_change_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_press_callback, test_button_long_press_callback, test_button_release_callback, test_knob_rotation_callback, test_parameter_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(INPUT_EVENTS_RC_OK, rc, "Initialization failed");
    struct InputsSharedEvent event = {
        .type = INPUTS_SHARED_EVENT_TYPE_PARAMETER_CHANGE,
        .parameter = {
            .parameter_id = INPUTS_SHARED_PARAMETER_ID_POWER,
            .value = 7 }
    };
    input_events_api_handle_event(event);
    TEST_ASSERT_EQUAL_MESSAGE(1, test_parameter_change_callback_fake.call_count, "Parameter change callback was not called");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_POWER, test_parameter_change_callback_fake.arg0_val, "Wrong parameter ID passed to parameter change callback");
    TEST_ASSERT_EQUAL_MESSAGE(7, test_parameter_change_callback_fake.arg1_val, "Wrong value passed to parameter change callback");
}

/*! \} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_input_events_api_init_should_store_callbacks);
    RUN_TEST(test_input_events_api_init_should_fail_null_button_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_long_press_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_release_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_rotation_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_parameter_callback);

    RUN_TEST(test_input_events_api_handle_event_should_invoke_button_press_callback);
    RUN_TEST(test_input_events_api_handle_event_should_invoke_button_long_press_callback);
    RUN_TEST(test_input_events_api_handle_event_should_invoke_button_release_callback);
    RUN_TEST(test_input_events_api_handle_event_should_invoke_knob_rotation_callback);
    RUN_TEST(test_input_events_api_handle_event_should_invoke_parameter_change_callback);

    return UNITY_END();
}
