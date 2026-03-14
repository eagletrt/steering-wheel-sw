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

/*! @} */

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_inputs_init_success);
    RUN_TEST(test_inputs_init_fail);

    RUN_TEST(test_inputs_update_button_success);

    return UNITY_END();
}
