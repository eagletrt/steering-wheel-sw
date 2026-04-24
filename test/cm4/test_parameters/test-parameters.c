/*!
 * \file test_parameters.c
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the Parameters API functionality.
 */

#include "parameters-api.h"
#include "inputs-shared.h"
#include "fff.h"
#include "unity.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(bool, on_change_cb, enum InputsSharedParameterID, uint8_t);

extern struct ParametersHandler handler;

void setUp(void) {
    RESET_FAKE(on_change_cb);
    FFF_RESET_HISTORY();
    on_change_cb_fake.return_val = true;
    parameters_api_init(on_change_cb);
}

/*!
 * \defgroup parameters_api_init Tests for parameters_api_init function
 * \{
 */

void test_parameters_init_stores_callback(void) {
    enum ParametersReturnCode rc = parameters_api_init(on_change_cb);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code from parameters_api_init");
    TEST_ASSERT_EQUAL_MESSAGE(on_change_cb, handler.on_change, "on_change callback was not stored correctly");
}

void test_parameters_init_rejects_null_callback(void) {
    enum ParametersReturnCode rc = parameters_api_init(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_ERROR, rc, "Expected error return code when passing NULL callback");
}

void test_parameters_init_resets_values_to_zero(void) {
    handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 5;
    handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = 1;
    parameters_api_init(on_change_cb);
    for (int i = 0; i < INPUTS_SHARED_PARAMETER_ID_COUNT; i++) {
        TEST_ASSERT_EQUAL_MESSAGE(0U, handler.values[i], "Expected all parameter values to be reset to 0");
    }
}

void test_parameters_init_does_not_fire_callback(void) {
    parameters_api_init(on_change_cb);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "on_change callback must not fire during init");
}

/*! \} */

/*!
 * \defgroup parameters_api_get Tests for parameters_api_get function
 * \{
 */

void test_parameters_get_returns_current_value(void) {
    handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 7;
    TEST_ASSERT_EQUAL(7, parameters_api_get(INPUTS_SHARED_PARAMETER_ID_POWER));
}

void test_parameters_get_returns_zero_for_invalid_id(void) {
    TEST_ASSERT_EQUAL(0U, parameters_api_get(INPUTS_SHARED_PARAMETER_ID_COUNT));
}

/*! \} */

/*!
 * \defgroup parameters_api_set Tests for parameters_api_set function
 * \{
 */

void test_parameters_set_updates_value_and_fires_callback(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 5);
    TEST_ASSERT_EQUAL(PARAMETERS_RC_OK, rc);
    TEST_ASSERT_EQUAL(5, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER]);
    TEST_ASSERT_EQUAL(1, on_change_cb_fake.call_count);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_POWER, on_change_cb_fake.arg0_val);
    TEST_ASSERT_EQUAL(5, on_change_cb_fake.arg1_val);
}

void test_parameters_set_does_not_fire_callback_when_value_unchanged(void) {
    parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 5);
    RESET_FAKE(on_change_cb);
    on_change_cb_fake.return_val = true;
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 5);
    TEST_ASSERT_EQUAL(PARAMETERS_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "on_change must not fire when value does not change");
}

void test_parameters_set_clamps_numeric_above_max(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 42);
    TEST_ASSERT_EQUAL(PARAMETERS_RC_OK, rc);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, on_change_cb_fake.arg1_val);
}

void test_parameters_set_clamps_toggle_above_one(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, 17);
    TEST_ASSERT_EQUAL(PARAMETERS_RC_OK, rc);
    TEST_ASSERT_EQUAL(1U, handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG]);
    TEST_ASSERT_EQUAL(1U, on_change_cb_fake.arg1_val);
}

void test_parameters_set_rejects_invalid_id(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_COUNT, 0);
    TEST_ASSERT_EQUAL(PARAMETERS_RC_ERROR, rc);
    TEST_ASSERT_EQUAL(0, on_change_cb_fake.call_count);
}

void test_parameters_set_propagates_callback_failure(void) {
    on_change_cb_fake.return_val = false;
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 3);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_ERROR, rc, "Expected error when on_change returns false");
    TEST_ASSERT_EQUAL_MESSAGE(3, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Value should still update locally even if callback fails");
}

/*! \} */

/*!
 * \defgroup parameters_api_handle_button Tests for parameters_api_handle_button function
 * \{
 */

void test_parameters_handle_button_toggles_traction_control(void) {
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(1U, handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG]);
    TEST_ASSERT_EQUAL(1, on_change_cb_fake.call_count);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, on_change_cb_fake.arg0_val);
    TEST_ASSERT_EQUAL(1U, on_change_cb_fake.arg1_val);

    rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(0U, handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG]);
    TEST_ASSERT_EQUAL(2, on_change_cb_fake.call_count);
    TEST_ASSERT_EQUAL(0U, on_change_cb_fake.arg1_val);
}

void test_parameters_handle_button_toggles_launch_control(void) {
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_RIGHT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(1U, handler.values[INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_LAUNCH_CONTROL, on_change_cb_fake.arg0_val);
}

void test_parameters_handle_button_ignores_unmapped_button(void) {
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_TS_ON);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Unmapped buttons must return OK without error");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "Unmapped buttons must not fire on_change");
}

void test_parameters_handle_button_propagates_callback_failure(void) {
    on_change_cb_fake.return_val = false;
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup parameters_api_handle_knob Tests for parameters_api_handle_knob function
 * \{
 */

void test_parameters_handle_knob_increments_power(void) {
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 3);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(3, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_POWER, on_change_cb_fake.arg0_val);
    TEST_ASSERT_EQUAL(3, on_change_cb_fake.arg1_val);
}

void test_parameters_handle_knob_maps_front_right_to_regen(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_RIGHT, 4);
    TEST_ASSERT_EQUAL(4, handler.values[INPUTS_SHARED_PARAMETER_ID_REGEN]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_REGEN, on_change_cb_fake.arg0_val);
}

void test_parameters_handle_knob_maps_side_left_to_torque_vectoring(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_SIDE_LEFT, 2);
    TEST_ASSERT_EQUAL(2, handler.values[INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING, on_change_cb_fake.arg0_val);
}

void test_parameters_handle_knob_clamps_below_zero(void) {
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, -5);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(0, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected clamping to floor at 0");
    // value was already 0, no transition, no callback
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "on_change must not fire when clamped value matches current value");
}

void test_parameters_handle_knob_clamps_above_max(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 50);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER]);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, on_change_cb_fake.arg1_val);
}

void test_parameters_handle_knob_ignores_unmapped_knob(void) {
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_SIDE_RIGHT, 3);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Unmapped knobs must return OK without error");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "Unmapped knobs must not fire on_change");
}

void test_parameters_handle_knob_no_change_on_zero_delta_after_non_zero(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 4);
    RESET_FAKE(on_change_cb);
    on_change_cb_fake.return_val = true;
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 0);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(4, handler.values[INPUTS_SHARED_PARAMETER_ID_POWER]);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_cb_fake.call_count, "Zero delta must not fire callback when value stays the same");
}

void test_parameters_handle_knob_propagates_callback_failure(void) {
    on_change_cb_fake.return_val = false;
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 3);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_parameters_init_stores_callback);
    RUN_TEST(test_parameters_init_rejects_null_callback);
    RUN_TEST(test_parameters_init_resets_values_to_zero);
    RUN_TEST(test_parameters_init_does_not_fire_callback);

    RUN_TEST(test_parameters_get_returns_current_value);
    RUN_TEST(test_parameters_get_returns_zero_for_invalid_id);

    RUN_TEST(test_parameters_set_updates_value_and_fires_callback);
    RUN_TEST(test_parameters_set_does_not_fire_callback_when_value_unchanged);
    RUN_TEST(test_parameters_set_clamps_numeric_above_max);
    RUN_TEST(test_parameters_set_clamps_toggle_above_one);
    RUN_TEST(test_parameters_set_rejects_invalid_id);
    RUN_TEST(test_parameters_set_propagates_callback_failure);

    RUN_TEST(test_parameters_handle_button_toggles_traction_control);
    RUN_TEST(test_parameters_handle_button_toggles_launch_control);
    RUN_TEST(test_parameters_handle_button_ignores_unmapped_button);
    RUN_TEST(test_parameters_handle_button_propagates_callback_failure);

    RUN_TEST(test_parameters_handle_knob_increments_power);
    RUN_TEST(test_parameters_handle_knob_maps_front_right_to_regen);
    RUN_TEST(test_parameters_handle_knob_maps_side_left_to_torque_vectoring);
    RUN_TEST(test_parameters_handle_knob_clamps_below_zero);
    RUN_TEST(test_parameters_handle_knob_clamps_above_max);
    RUN_TEST(test_parameters_handle_knob_ignores_unmapped_knob);
    RUN_TEST(test_parameters_handle_knob_no_change_on_zero_delta_after_non_zero);
    RUN_TEST(test_parameters_handle_knob_propagates_callback_failure);

    return UNITY_END();
}
