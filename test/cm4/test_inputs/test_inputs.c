#include "inputs-api.h"
#include "inputs-shared.h"
#include "unity.h"

int notify_callback_call_count = 0;
int action_callback_call_count = 0;

void critical_section_callback(void) {
}

bool notify_callback(struct InputsSharedEvent *ev, void (*critical_section_callback)(void)) {
    notify_callback_call_count++;

    return true;
}

enum InputsReturnCode action_callback(struct InputsSharedEvent *ev) {
    action_callback_call_count++;
    return INPUTS_RC_OK;
}

struct InputsHandler handler;

void setUp(void) {
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

/*! @} */

/*!
 * \defgroup inputs_api_update_button Tests for inputs_api_update_button function
 * @{
 */

void test_inputs_update_button_simple(void) {
    enum InputsReturnCode ret = inputs_api_update_button(&handler, INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT, true, 100);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, ret, "Failed to update button state");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, notify_callback_call_count, "Notify callback was not called");
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, action_callback_call_count, "Action callback was not called");
}

/*! @} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_inputs_init_success);
    RUN_TEST(test_inputs_init_fail);

    RUN_TEST(test_inputs_update_button_simple);

    return UNITY_END();
}
