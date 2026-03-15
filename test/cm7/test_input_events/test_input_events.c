#include "unity.h"
#include "fff.h"
#include "input-events-api.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_button_event_callback, enum InputsSharedButtonID);
FAKE_VALUE_FUNC(enum InputEventsReturnCode, test_knob_rotation_callback, enum InputsSharedKnobID, int8_t);

void setUp(void) {
    RESET_FAKE(test_button_event_callback);
    RESET_FAKE(test_knob_rotation_callback);
    FFF_RESET_HISTORY();
}

/*!
 * \defgroup input_events_api_tests Input Events API Tests
 * @{
 */

void test_input_events_api_init_should_store_callbacks(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_event_callback, test_knob_rotation_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_OK, rc);
}

void test_input_events_api_init_should_fail_null_button_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(NULL, test_knob_rotation_callback);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

void test_input_events_api_init_should_fail_null_rotation_callback(void) {
    enum InputEventsReturnCode rc = input_events_api_init(test_button_event_callback, NULL);
    TEST_ASSERT_EQUAL(INPUT_EVENTS_RC_ERROR, rc);
}

/*! @} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_input_events_api_init_should_store_callbacks);
    RUN_TEST(test_input_events_api_init_should_fail_null_button_callback);
    RUN_TEST(test_input_events_api_init_should_fail_null_rotation_callback);

    return UNITY_END();
}
