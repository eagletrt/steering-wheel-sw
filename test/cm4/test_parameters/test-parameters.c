/*!
 * \file test_parameters.c
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the Parameters API functionality.
 */

#include "parameters-api.h"
#include "inputs-shared.h"
#include "leds-api.h"
#include "fff.h"
#include "unity.h"

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(bool, on_change_callback, enum InputsSharedParameterID, uint8_t);
FAKE_VALUE_FUNC(enum LedsReturnCode, fake_transmit, const enum WS2812BDutyCycle *, uint16_t);

extern struct ParametersHandler parameters_handler;
extern struct LedsHandler leds_handler;

void setUp(void) {
    RESET_FAKE(on_change_callback);
    RESET_FAKE(fake_transmit);
    FFF_RESET_HISTORY();
    on_change_callback_fake.return_val = true;
    fake_transmit_fake.return_val = LEDS_RC_OK;
    leds_api_init(fake_transmit);
    parameters_api_init(on_change_callback);
}

/*!
 * \defgroup parameters_api_init Tests for parameters_api_init function
 * \{
 */

void test_parameters_init_stores_callback(void) {
    enum ParametersReturnCode rc = parameters_api_init(on_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code from parameters_api_init");
    TEST_ASSERT_EQUAL_MESSAGE(on_change_callback, parameters_handler.on_change, "on_change callback was not stored correctly");
}

void test_parameters_init_rejects_null_callback(void) {
    enum ParametersReturnCode rc = parameters_api_init(NULL);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_ERROR, rc, "Expected error return code when passing NULL callback");
}

void test_parameters_init_resets_values_to_zero(void) {
    uint8_t expected[INPUTS_SHARED_PARAMETER_ID_COUNT] = { 0 };
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 5;
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = 1;
    parameters_api_init(on_change_callback);
    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected, parameters_handler.values, INPUTS_SHARED_PARAMETER_ID_COUNT, "Parameter values were not reset to zero on init");
}

void test_parameters_init_does_not_fire_callback(void) {
    parameters_api_init(on_change_callback);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "on_change callback must not fire during init");
}

/*! \} */

/*!
 * \defgroup parameters_api_get Tests for parameters_api_get function
 * \{
 */

void test_parameters_get_returns_current_value(void) {
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 7;
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
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code from parameters_api_set");
    TEST_ASSERT_EQUAL_MESSAGE(5, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Parameter value was not updated correctly");
    TEST_ASSERT_EQUAL_MESSAGE(1, on_change_callback_fake.call_count, "Expected on_change callback to be fired once");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_POWER, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
    TEST_ASSERT_EQUAL_MESSAGE(5, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with correct value");
}

void test_parameters_set_does_not_fire_callback_when_value_unchanged(void) {
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 5;
    on_change_callback_fake.return_val = true;
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 5);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code even when setting to the same value");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "on_change must not fire when value does not change");
}

void test_parameters_set_clamps_numeric_above_max(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 42);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code from parameters_api_set even when value is above max");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected parameter value to be clamped to INPUTS_SHARED_PARAMETER_NUMERIC_MAX");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with clamped value");
}

void test_parameters_set_clamps_toggle_above_one(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, 17);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_OK, rc, "Expected success return code from parameters_api_set even when value is above max");
    TEST_ASSERT_EQUAL_MESSAGE(1U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG], "Expected toggle parameter value to be clamped to 1");
    TEST_ASSERT_EQUAL_MESSAGE(1U, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with clamped value");
}

void test_parameters_set_rejects_invalid_id(void) {
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_COUNT, 0);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_ERROR, rc, "Expected error return code when setting invalid parameter ID");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "on_change must not fire when setting invalid parameter ID");
}

void test_parameters_set_propagates_callback_failure(void) {
    on_change_callback_fake.return_val = false;
    enum ParametersReturnCode rc = parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 3);
    TEST_ASSERT_EQUAL_MESSAGE(PARAMETERS_RC_ERROR, rc, "Expected error when on_change returns false");
    TEST_ASSERT_EQUAL_MESSAGE(3, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Value should still update locally even if callback fails");
}

/*! \} */

/*!
 * \defgroup parameters_api_handle_button Tests for parameters_api_handle_button function
 * \{
 */

void test_parameters_handle_button_toggle_0_to_1(void) {
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = 0;
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Expected success return code from parameters_api_handle_button");
    TEST_ASSERT_EQUAL_MESSAGE(1U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG], "Expected button press to toggle telemetry log parameter from 0 to 1");
    TEST_ASSERT_EQUAL_MESSAGE(1, on_change_callback_fake.call_count, "Expected on_change callback to be fired once");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
    TEST_ASSERT_EQUAL_MESSAGE(1U, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with correct value");
}

void test_parameters_handle_button_toggle_1_to_0(void) {
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG] = 1;
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Expected success return code from parameters_api_handle_button");
    TEST_ASSERT_EQUAL_MESSAGE(0U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG], "Expected button press to toggle telemetry log parameter from 1 to 0");
    TEST_ASSERT_EQUAL_MESSAGE(1, on_change_callback_fake.call_count, "Expected on_change callback to be fired once");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_TELEMETRY_LOG, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
    TEST_ASSERT_EQUAL_MESSAGE(0U, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with correct value");
}

void test_parameters_handle_button_ignores_unmapped_button(void) {
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_TS_ON);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Unmapped buttons must return OK without error");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Unmapped buttons must not fire on_change");
}

void test_parameters_handle_button_propagates_callback_failure(void) {
    on_change_callback_fake.return_val = false;
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
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Expected success return code from parameters_api_handle_knob");
    TEST_ASSERT_EQUAL_MESSAGE(3, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected knob rotation to increment power parameter by delta");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_POWER, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
    TEST_ASSERT_EQUAL_MESSAGE(3, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with new parameter value");
}

void test_parameters_handle_knob_maps_front_right_to_regen(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_RIGHT, 4);
    TEST_ASSERT_EQUAL_MESSAGE(4, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_REGEN], "Expected knob rotation to increment regen parameter by delta");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_REGEN, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
}

void test_parameters_handle_knob_maps_side_left_to_torque_vectoring(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_SIDE_LEFT, 2);
    TEST_ASSERT_EQUAL_MESSAGE(2, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING], "Expected knob rotation to increment torque vectoring parameter by delta");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_ID_TORQUE_VECTORING, on_change_callback_fake.arg0_val, "Expected on_change callback to be called with correct parameter_id");
}

void test_parameters_handle_knob_clamps_below_zero(void) {
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, -5);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Expected success return code from parameters_api_handle_knob even when delta would underflow");
    TEST_ASSERT_EQUAL_MESSAGE(0, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected clamping to floor at 0");
    // value was already 0, no transition, no callback
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "on_change must not fire when clamped value matches current value");
}

void test_parameters_handle_knob_clamps_above_max(void) {
    parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 50);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected clamping to cap at INPUTS_SHARED_PARAMETER_NUMERIC_MAX");
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_SHARED_PARAMETER_NUMERIC_MAX, on_change_callback_fake.arg1_val, "Expected on_change callback to be called with clamped value");
}

void test_parameters_handle_knob_ignores_unmapped_knob(void) {
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_SIDE_RIGHT, 3);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Unmapped knobs must return OK without error");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Unmapped knobs must not fire on_change");
}

void test_parameters_handle_knob_no_change_on_zero_delta_after_non_zero(void) {
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER] = 4;
    on_change_callback_fake.return_val = true;
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 0);
    TEST_ASSERT_EQUAL_MESSAGE(INPUTS_RC_OK, rc, "Expected success return code from parameters_api_handle_knob even when delta is zero");
    TEST_ASSERT_EQUAL_MESSAGE(4, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_POWER], "Expected zero delta to not change the parameter value");
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Zero delta must not fire callback when value stays the same");
}

void test_parameters_handle_knob_propagates_callback_failure(void) {
    on_change_callback_fake.return_val = false;
    enum InputsReturnCode rc = parameters_api_handle_knob(INPUTS_SHARED_KNOB_ID_FRONT_LEFT, 3);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup parameters_api_ptt Tests for the PTT paddle behavior
 * \{
 */

void test_ptt_single_paddle_press_activates(void) {
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_TRUE(parameters_handler.ptt_top_left_held);
    TEST_ASSERT_EQUAL(1U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT]);
    TEST_ASSERT_EQUAL(1, on_change_callback_fake.call_count);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_PTT, on_change_callback_fake.arg0_val);
    TEST_ASSERT_EQUAL(1U, on_change_callback_fake.arg1_val);
}

void test_ptt_second_paddle_press_does_not_refire(void) {
    parameters_handler.ptt_top_left_held = true;
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT] = 1;
    on_change_callback_fake.return_val = true;

    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(1U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT]);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Second paddle press must not re-fire on_change while PTT is already active");
}

void test_ptt_release_one_paddle_keeps_active_when_other_held(void) {
    parameters_handler.ptt_top_left_held = true;
    parameters_handler.ptt_top_right_held = true;
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT] = 1;
    on_change_callback_fake.return_val = true;

    enum InputsReturnCode rc = parameters_api_handle_button_release(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(1U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT], "PTT must stay active while the other paddle is held");
    TEST_ASSERT_FALSE(parameters_handler.ptt_top_left_held);
    TEST_ASSERT_TRUE(parameters_handler.ptt_top_right_held);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Releasing one paddle must not fire on_change while the other is held");
}

void test_ptt_release_both_paddles_deactivates(void) {
    parameters_handler.ptt_top_right_held = true;
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT] = 1;
    on_change_callback_fake.return_val = true;

    enum InputsReturnCode rc = parameters_api_handle_button_release(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_RIGHT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL(0U, parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT]);
    TEST_ASSERT_EQUAL(1, on_change_callback_fake.call_count);
    TEST_ASSERT_EQUAL(INPUTS_SHARED_PARAMETER_ID_PTT, on_change_callback_fake.arg0_val);
    TEST_ASSERT_EQUAL(0U, on_change_callback_fake.arg1_val);
}

void test_ptt_release_when_inactive_does_not_fire_callback(void) {
    enum InputsReturnCode rc = parameters_api_handle_button_release(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Release must not fire callback when value did not change");
}

void test_ptt_handle_button_release_ignores_non_paddle(void) {
    enum InputsReturnCode rc = parameters_api_handle_button_release(INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_OK, rc);
    TEST_ASSERT_EQUAL_MESSAGE(0, on_change_callback_fake.call_count, "Non-paddle release must be silently ignored");
}

void test_ptt_propagates_callback_failure(void) {
    on_change_callback_fake.return_val = false;
    enum InputsReturnCode rc = parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);
    TEST_ASSERT_EQUAL(INPUTS_RC_ERROR, rc);
}

/*! \} */

/*!
 * \defgroup parameters_api_ptt_leds Tests for PTT local LED side effect
 * \{
 */

void test_ptt_activation_overlays_blue_pattern_on_center_leds(void) {
    struct LedColor green = { .g = 255, .r = 0, .b = 0 };
    leds_handler.colors[LEDS_INDEX_TOP_LEFT_1] = green;
    uint8_t expected_ptt[] = { 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255 };

    parameters_api_handle_button(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);

    TEST_ASSERT_EQUAL_UINT8_ARRAY_MESSAGE(expected_ptt, &leds_handler.colors[LEDS_INDEX_CENTER_0], sizeof(expected_ptt), "Center LEDs must be overlaid with blue pattern on PTT activation");
    TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.colors_backup[LEDS_INDEX_TOP_LEFT_1].g, "Pre-PTT pattern must be snapshotted");
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "PTT activation must trigger leds_api_show");
}

void test_ptt_deactivation_restores_previous_pattern(void) {
    struct LedColor green = { .g = 255, .r = 0, .b = 0 };
    leds_handler.colors_backup[LEDS_INDEX_TOP_LEFT_1] = green;
    uint8_t expected_ptt[] = { 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255, 0, 0, 255 };
    memcpy(&leds_handler.colors[LEDS_INDEX_CENTER_0], expected_ptt, sizeof(expected_ptt));
    parameters_handler.ptt_top_left_held = true;
    parameters_handler.values[INPUTS_SHARED_PARAMETER_ID_PTT] = 1;

    parameters_api_handle_button_release(INPUTS_SHARED_BUTTON_ID_PADDLE_TOP_LEFT);

    TEST_ASSERT_EQUAL_UINT8_MESSAGE(255, leds_handler.colors[LEDS_INDEX_TOP_LEFT_1].g, "Original color must be restored on PTT release");
    TEST_ASSERT_EQUAL_MESSAGE(1, fake_transmit_fake.call_count, "PTT deactivation must trigger leds_api_show");
}

void test_ptt_no_led_effect_for_other_parameters(void) {
    parameters_api_set(INPUTS_SHARED_PARAMETER_ID_POWER, 5);
    TEST_ASSERT_EQUAL_MESSAGE(0, fake_transmit_fake.call_count, "Non-PTT parameter changes must not touch LEDs");
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

    RUN_TEST(test_parameters_handle_button_toggle_0_to_1);
    RUN_TEST(test_parameters_handle_button_toggle_1_to_0);
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

    RUN_TEST(test_ptt_single_paddle_press_activates);
    RUN_TEST(test_ptt_second_paddle_press_does_not_refire);
    RUN_TEST(test_ptt_release_one_paddle_keeps_active_when_other_held);
    RUN_TEST(test_ptt_release_both_paddles_deactivates);
    RUN_TEST(test_ptt_release_when_inactive_does_not_fire_callback);
    RUN_TEST(test_ptt_handle_button_release_ignores_non_paddle);
    RUN_TEST(test_ptt_propagates_callback_failure);

    RUN_TEST(test_ptt_activation_overlays_blue_pattern_on_center_leds);
    RUN_TEST(test_ptt_deactivation_restores_previous_pattern);
    RUN_TEST(test_ptt_no_led_effect_for_other_parameters);

    return UNITY_END();
}
