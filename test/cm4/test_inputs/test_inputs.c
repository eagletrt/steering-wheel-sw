#include "inputs-api.h"
#include "inputs-shared.h"
#include "fff.h"
#include "unity.h"

DEFINE_FFF_GLOBALS;

typedef void (*cs_callback)(void);

FAKE_VOID_FUNC(critical_section_callback);
FAKE_VALUE_FUNC(bool, notify_callback, struct InputsSharedEvent, cs_callback);
FAKE_VALUE_FUNC(enum InputsReturnCode, action_callback, struct InputsSharedEvent);

struct InputsHandler handler;

void setUp(void) {
    RESET_FAKE(critical_section_callback);
    RESET_FAKE(notify_callback);
    RESET_FAKE(action_callback);
    FFF_RESET_HISTORY();
    inputs_api_init(&handler, critical_section_callback, notify_callback, action_callback);
}

/*!
 * \defgroup inputs_api_init Tests for inputs_api_init function
 * @{
 */

void test_inputs_init_success(void) {
    handler = (struct InputsHandler){ 0 };
    enum InputsReturnCode rc = inputs_api_init(&handler, critical_section_callback, notify_callback, action_callback);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
}

void test_inputs_init_fail(void) {
    handler = (struct InputsHandler){ 0 };
    enum InputsReturnCode rc = inputs_api_init(&handler, NULL, NULL, NULL);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

void test_inputs_init_null_handler(void) {
    enum InputsReturnCode rc = inputs_api_init(NULL, critical_section_callback, notify_callback, action_callback);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

/*! @} */

/*!
 * \defgroup inputs_api_update_button Tests for inputs_api_update_button function
 * @{
 */

void test_inputs_update_button_success(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_button_notify_fail(void) {
    notify_callback_fake.return_val = false;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_NOTIFY_ERROR, ret, "Expected error return code when notify fails");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_button_action_fail(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_ERROR;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when action fails");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_button_invalid_handler(void) {
    enum InputsReturnCode ret = inputs_api_update_button(NULL, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when handler is NULL");
}

void test_inputs_update_button_invalid_button_id(void) {
    enum InputsReturnCode ret = inputs_api_update_button(&handler, -1, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when button ID is invalid");
    ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_COUNT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when button ID is out of range");
}

void test_inputs_update_button_disabled(void) {
    handler.buttons[INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT].enabled = false;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Expected success return code when button is disabled");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, notify_callback_fake.call_count, "Notify callback should not be called when button is disabled");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, action_callback_fake.call_count, "Action callback should not be called when button is disabled");
}

void test_inputs_update_button_release(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, false, 200);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state on release");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, notify_callback_fake.call_count, "Notify callback should be called twice (press and release)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, action_callback_fake.call_count, "Action callback should be called twice (press and release)");
}

/*! @} */

/*!
 * \defgroup inputs_api_update_knob Tests for inputs_api_update_knob function
 * @{
 */

void test_inputs_update_knob_success(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update knob state");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_knob_notify_fail(void) {
    notify_callback_fake.return_val = false;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_NOTIFY_ERROR, ret, "Expected error return code when notify fails");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_knob_action_fail(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_ERROR;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when action fails");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback was not called");
}

void test_inputs_update_knob_invalid_handler(void) {
    enum InputsReturnCode ret = inputs_api_update_knob(NULL, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when handler is NULL");
}

void test_inputs_update_knob_invalid_knob_id(void) {
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, -1, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when knob ID is invalid");
    ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_COUNT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when knob ID is out of range");
}

void test_inputs_update_knob_disabled(void) {
    handler.knobs[INPUTS_SHARED_KNOB_ID_FRONT_LEFT].enabled = false;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Expected success return code when knob is disabled");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, notify_callback_fake.call_count, "Notify callback should not be called when knob is disabled");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, action_callback_fake.call_count, "Action callback should not be called when knob is disabled");
}

void test_inputs_update_knob_no_movement(void) {
    handler.knobs[INPUTS_SHARED_KNOB_ID_FRONT_LEFT].last_position = 10;
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 10);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Expected success return code when knob position does not change");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, notify_callback_fake.call_count, "Notify callback should not be called when knob position does not change");
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, action_callback_fake.call_count, "Action callback should not be called when knob position does not change");
}

void test_inputs_update_knob_movement(void) {
    handler.knobs[INPUTS_SHARED_KNOB_ID_FRONT_LEFT].last_position = 10;
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_knob(&handler, INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 15);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update knob state on movement");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback should be called once for knob movement");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback should be called once for knob movement");
}

/*! @} */

/*!
 * \defgroup inputs_api_poll_for_long_press Tests for inputs_api_poll_for_long_press function
 * @{
 */

void test_inputs_poll_for_long_press(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state for long press test");
    ret = inputs_api_poll_for_long_press(&handler, 100 + INPUTS_LONG_PRESS_THRESHOLD_MS);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to poll for long press");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, notify_callback_fake.call_count, "Notify callback should be called twice (press and long press)");
    TEST_ASSERT_EQUAL_INT_MESSAGE(2, action_callback_fake.call_count, "Action callback should be called twice (press and long press)");
    struct InputsSharedEvent ev = notify_callback_fake.arg0_val;
    TEST_ASSERT_EQUAL(INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS, ev.type);
}

void test_inputs_poll_for_long_press_notify_fail(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state for long press test");
    notify_callback_fake.return_val = false;
    action_callback_fake.return_val = INPUTS_RC_OK;
    ret = inputs_api_poll_for_long_press(&handler, 100 + INPUTS_LONG_PRESS_THRESHOLD_MS);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_NOTIFY_ERROR, ret, "Expected error return code when notify fails during long press");
}

void test_inputs_poll_for_long_press_action_fail(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state for long press test");
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_ERROR;
    ret = inputs_api_poll_for_long_press(&handler, 100 + INPUTS_LONG_PRESS_THRESHOLD_MS);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when action fails during long press");
}

void test_inputs_poll_for_long_press_invalid_handler(void) {
    enum InputsReturnCode ret = inputs_api_poll_for_long_press(NULL, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_ERROR, ret, "Expected error return code when handler is NULL during long press poll");
}

void test_inputs_poll_for_long_press_no_long_press(void) {
    notify_callback_fake.return_val = true;
    action_callback_fake.return_val = INPUTS_RC_OK;
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state for long press test");
    ret = inputs_api_poll_for_long_press(&handler, 100 + INPUTS_LONG_PRESS_THRESHOLD_MS - 1);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to poll for long press when threshold not reached");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_fake.call_count, "Notify callback should only be called once for button press");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_fake.call_count, "Action callback should only be called once for button press");
}

/*! @} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_inputs_init_success);
    RUN_TEST(test_inputs_init_fail);
    RUN_TEST(test_inputs_init_null_handler);

    RUN_TEST(test_inputs_update_button_success);
    RUN_TEST(test_inputs_update_button_notify_fail);
    RUN_TEST(test_inputs_update_button_action_fail);
    RUN_TEST(test_inputs_update_button_invalid_handler);
    RUN_TEST(test_inputs_update_button_invalid_button_id);
    RUN_TEST(test_inputs_update_button_disabled);
    RUN_TEST(test_inputs_update_button_release);

    RUN_TEST(test_inputs_update_knob_success);
    RUN_TEST(test_inputs_update_knob_notify_fail);
    RUN_TEST(test_inputs_update_knob_action_fail);
    RUN_TEST(test_inputs_update_knob_invalid_handler);
    RUN_TEST(test_inputs_update_knob_invalid_knob_id);
    RUN_TEST(test_inputs_update_knob_disabled);
    RUN_TEST(test_inputs_update_knob_no_movement);
    RUN_TEST(test_inputs_update_knob_movement);

    RUN_TEST(test_inputs_poll_for_long_press);
    RUN_TEST(test_inputs_poll_for_long_press_notify_fail);
    RUN_TEST(test_inputs_poll_for_long_press_action_fail);
    RUN_TEST(test_inputs_poll_for_long_press_invalid_handler);
    RUN_TEST(test_inputs_poll_for_long_press_no_long_press);

    return UNITY_END();
}
