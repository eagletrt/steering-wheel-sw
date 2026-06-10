/*!
 * \file test-dashboard.c
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for the dashboard API.
 */

#include "unity.h"
#include "dashboard-api.h"
#include "raster-fonts.h"
#include <string.h>

struct DashboardHandler dashboard_handler;

void setUp(void) {
    memset(&dashboard_handler, 0, sizeof(dashboard_handler));
    dashboard_api_init(&dashboard_handler);
    // Clear updated flags so each test only sees what the test itself triggered.
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        dashboard_handler.boxes[i].updated = false;
    }
}

/*!
 * \defgroup dashboard_api_init Test cases for dashboard_api_init function
 * \{
 */

void test_dashboard_init_success(void) {
    enum DashboardReturnCode rc = dashboard_api_init(&dashboard_handler);
    TEST_ASSERT_EQUAL_MESSAGE(DASHBOARD_RC_OK, rc, "init must return DASHBOARD_RC_OK on a valid handler");
}

void test_dashboard_init_null_handler(void) {
    enum DashboardReturnCode rc = dashboard_api_init(NULL);
    TEST_ASSERT_EQUAL(DASHBOARD_RC_NULL_POINTER, rc);
}

void test_dashboard_init_populates_every_box(void) {
    dashboard_api_init(&dashboard_handler);
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        TEST_ASSERT_EQUAL_MESSAGE(i, dashboard_handler.boxes[i].id, "Each box id must match its field index");
        TEST_ASSERT_NOT_NULL_MESSAGE(dashboard_handler.boxes[i].label, "Each box must have a label attached");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(&dashboard_handler.labels[i], dashboard_handler.boxes[i].label, "Box must reference the matching label slot");
    }
}

void test_dashboard_init_labels_point_at_internal_buffers(void) {
    dashboard_api_init(&dashboard_handler);
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        TEST_ASSERT_EQUAL_PTR_MESSAGE(dashboard_handler.text[i], dashboard_handler.labels[i].text, "Label text pointer must reference the handler's per-field buffer");
        TEST_ASSERT_EQUAL_PTR_MESSAGE(&font_konexy, dashboard_handler.labels[i].font, "All labels must use font_konexy");
    }
}

void test_dashboard_init_seeds_placeholder_text(void) {
    dashboard_api_init(&dashboard_handler);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SCENARIO", dashboard_handler.text[DASHBOARD_FIELD_SCENARIO_HEADER], "Scenario header carries its static label");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("HV", dashboard_handler.text[DASHBOARD_FIELD_HV_HEADER], "HV header carries its static label");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("TRS", dashboard_handler.text[DASHBOARD_FIELD_TRS_HEADER], "TRS header carries its static label");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("MTR", dashboard_handler.text[DASHBOARD_FIELD_MTR_HEADER], "MTR header carries its static label");
}

void test_dashboard_init_box_count_matches_field_count(void) {
    TEST_ASSERT_EQUAL_MESSAGE(DASHBOARD_FIELD_COUNT, DASHBOARD_BOX_COUNT, "Box count macro must mirror field count enum");
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_state Tests for dashboard_api_set_state
 * \{
 */

void test_dashboard_set_state_writes_text_and_flags_updated(void) {
    enum DashboardReturnCode rc = dashboard_api_set_state(&dashboard_handler, "DRIVE");
    TEST_ASSERT_EQUAL_MESSAGE(DASHBOARD_RC_OK, rc, "Setter must return OK on a valid call");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("DRIVE", dashboard_handler.text[DASHBOARD_FIELD_STATE], "State buffer must hold the new text");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_STATE].updated, "Box must be flagged updated after a real change");
}

void test_dashboard_set_state_null_handler(void) {
    enum DashboardReturnCode rc = dashboard_api_set_state(NULL, "DRIVE");
    TEST_ASSERT_EQUAL(DASHBOARD_RC_NULL_POINTER, rc);
}

void test_dashboard_set_state_null_text(void) {
    enum DashboardReturnCode rc = dashboard_api_set_state(&dashboard_handler, NULL);
    TEST_ASSERT_EQUAL(DASHBOARD_RC_NULL_POINTER, rc);
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_numeric Tests for the power/regen/torque setters
 * \{
 */

void test_dashboard_set_power_formats_value(void) {
    dashboard_api_set_power(&dashboard_handler, 7U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("POW 7", dashboard_handler.text[DASHBOARD_FIELD_POWER], "Power field must read 'POW <value>'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated, "Power box must be flagged updated");
}

void test_dashboard_set_regen_formats_value(void) {
    dashboard_api_set_regen(&dashboard_handler, 3U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RGN 3", dashboard_handler.text[DASHBOARD_FIELD_REGEN], "Regen field must read 'RGN <value>'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_REGEN].updated, "Regen box must be flagged updated");
}

void test_dashboard_set_torque_formats_value(void) {
    dashboard_api_set_torque(&dashboard_handler, 10U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("TQ 10", dashboard_handler.text[DASHBOARD_FIELD_TORQUE], "Torque field must read 'TQ <value>'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_TORQUE].updated, "Torque box must be flagged updated");
}

void test_dashboard_set_power_null_handler(void) {
    TEST_ASSERT_EQUAL(DASHBOARD_RC_NULL_POINTER, dashboard_api_set_power(NULL, 5U));
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_slip Tests for dashboard_api_set_slip
 * \{
 */

void test_dashboard_set_slip_on(void) {
    dashboard_api_set_slip(&dashboard_handler, true);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SLIP ON", dashboard_handler.text[DASHBOARD_FIELD_SLIP], "Slip ON must format as 'SLIP ON'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_SLIP].updated, "Slip box must be flagged updated");
}

void test_dashboard_set_slip_off(void) {
    dashboard_api_set_slip(&dashboard_handler, false);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SLIP OFF", dashboard_handler.text[DASHBOARD_FIELD_SLIP], "Slip OFF must format as 'SLIP OFF'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_SLIP].updated, "Slip box must be flagged updated");
}

void test_dashboard_set_slip_null_handler(void) {
    TEST_ASSERT_EQUAL(DASHBOARD_RC_NULL_POINTER, dashboard_api_set_slip(NULL, true));
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_soc Tests for dashboard_api_set_soc
 * \{
 */

void test_dashboard_set_soc_formats_percent(void) {
    dashboard_api_set_soc(&dashboard_handler, 69U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("69%", dashboard_handler.text[DASHBOARD_FIELD_HV_SOC], "SoC must format as '<percent>%'");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_HV_SOC].color.argb, "SoC color must be set to white");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_HV_SOC].updated, "SoC box must be flagged updated");
}

void test_dashboard_set_soc_clamps_above_100(void) {
    dashboard_api_set_soc(&dashboard_handler, 200U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("100%", dashboard_handler.text[DASHBOARD_FIELD_HV_SOC], "Values above 100 must clamp to '100%%'");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_HV_SOC].color.argb, "SoC color must be set to white");
}

void test_dashboard_set_soc_zero(void) {
    dashboard_api_set_soc(&dashboard_handler, 0U);
    TEST_ASSERT_EQUAL_STRING("0%", dashboard_handler.text[DASHBOARD_FIELD_HV_SOC]);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_ERROR, dashboard_handler.labels[DASHBOARD_FIELD_HV_SOC].color.argb, "SoC color must be set to red");
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_temps Tests for the temperature setters
 * \{
 */

void test_dashboard_set_hv_temp_positive(void) {
    dashboard_api_set_hv_temp(&dashboard_handler, 104);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("104C", dashboard_handler.text[DASHBOARD_FIELD_HV_TEMP], "HV temp must format as '<value>C'");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_ERROR, dashboard_handler.labels[DASHBOARD_FIELD_HV_TEMP].color.argb, "Motor color must be set to red");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_HV_TEMP].updated, "HV temp box must be flagged updated");
}

void test_dashboard_set_hv_temp_negative(void) {
    dashboard_api_set_hv_temp(&dashboard_handler, -5);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_HV_TEMP].color.argb, "Motor color must be set to white");
    TEST_ASSERT_EQUAL_STRING("-5C", dashboard_handler.text[DASHBOARD_FIELD_HV_TEMP]);
}

void test_dashboard_set_inv_temp_formats_value(void) {
    dashboard_api_set_inv_temp(&dashboard_handler, 22);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("INV 22C", dashboard_handler.text[DASHBOARD_FIELD_INV], "Inverter temp must format as 'INV <value>C'");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_INV].color.argb, "Motor color must be set to white");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_INV].updated, "INV box must be flagged updated");
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_lap Tests for the lap counter and delta
 * \{
 */

void test_dashboard_set_lap_formats_current_over_total(void) {
    dashboard_api_set_lap(&dashboard_handler, 9U, 11U);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("LAP 9/11", dashboard_handler.text[DASHBOARD_FIELD_LAP], "Lap must format as 'LAP current/total'");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_LAP].updated, "Lap box must be flagged updated");
}

void test_dashboard_set_lap_delta_positive(void) {
    dashboard_api_set_lap_delta_ms(&dashboard_handler, 1234);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("+1.234", dashboard_handler.text[DASHBOARD_FIELD_LAP_DELTA], "Positive delta must keep the '+' sign and 3-digit ms");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_LAP_DELTA].updated, "Lap delta box must be flagged updated");
}

void test_dashboard_set_lap_delta_negative(void) {
    dashboard_api_set_lap_delta_ms(&dashboard_handler, -420);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("-0.420", dashboard_handler.text[DASHBOARD_FIELD_LAP_DELTA], "Negative delta must use a '-' sign and keep 3-digit ms");
}

void test_dashboard_set_lap_delta_zero(void) {
    dashboard_api_set_lap_delta_ms(&dashboard_handler, 0);
    TEST_ASSERT_EQUAL_STRING("+0.000", dashboard_handler.text[DASHBOARD_FIELD_LAP_DELTA]);
}

/*! \} */

/*!
 * \defgroup dashboard_api_set_tire_motor_temps Tests for the four-temp setters
 * \{
 */

void test_dashboard_set_tire_temps_updates_all_four(void) {
    dashboard_api_set_tire_temps(&dashboard_handler, 95, 60, 10, 40);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("95C", dashboard_handler.text[DASHBOARD_FIELD_TRS_FL], "Front-left tire reads the FL argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("60C", dashboard_handler.text[DASHBOARD_FIELD_TRS_FR], "Front-right tire reads the FR argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("10C", dashboard_handler.text[DASHBOARD_FIELD_TRS_RL], "Rear-left tire reads the RL argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("40C", dashboard_handler.text[DASHBOARD_FIELD_TRS_RR], "Rear-right tire reads the RR argument");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_WARNING, dashboard_handler.labels[DASHBOARD_FIELD_TRS_FL].color.argb, "Front left tyre color must be set to yellow");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_TRS_FR].color.argb, "Front right tyre color must be set to white");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_COLD_TIRES, dashboard_handler.labels[DASHBOARD_FIELD_TRS_RL].color.argb, "Rear left tyre color must be set to cyan");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_TRS_RR].color.argb, "Rear right tyre color must be set to white");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_TRS_FL].updated, "FL tire box must be flagged updated");
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_TRS_RR].updated, "RR tire box must be flagged updated");
}

void test_dashboard_set_motor_temps_updates_all_four(void) {
    dashboard_api_set_motor_temps(&dashboard_handler, 22, 23, 81, 105);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("22C", dashboard_handler.text[DASHBOARD_FIELD_MTR_FL], "Front-left motor reads the FL argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("23C", dashboard_handler.text[DASHBOARD_FIELD_MTR_FR], "Front-right motor reads the FR argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("81C", dashboard_handler.text[DASHBOARD_FIELD_MTR_RL], "Rear-left motor reads the RL argument");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("105C", dashboard_handler.text[DASHBOARD_FIELD_MTR_RR], "Rear-right motor reads the RR argument");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_MTR_FL].color.argb, "Front left motor color must be set to white");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_SECONDARY, dashboard_handler.labels[DASHBOARD_FIELD_MTR_FR].color.argb, "Front left motor color must be set to white");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_WARNING, dashboard_handler.labels[DASHBOARD_FIELD_MTR_RL].color.argb, "Front left motor color must be set to yellow");
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(DASHBOARD_COLOR_ERROR, dashboard_handler.labels[DASHBOARD_FIELD_MTR_RR].color.argb, "Front left motor color must be set to red");
}

/*! \} */

/*!
 * \defgroup dashboard_no_op Tests for the no-op skip optimization
 * \{
 */

void test_dashboard_setter_does_not_re_flag_when_value_unchanged(void) {
    dashboard_api_set_power(&dashboard_handler, 5U);
    dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated = false;

    dashboard_api_set_power(&dashboard_handler, 5U);

    TEST_ASSERT_FALSE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated, "Setting the same value twice must not re-flag the box");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("POW 5", dashboard_handler.text[DASHBOARD_FIELD_POWER], "The text buffer must still hold the previous value");
}

void test_dashboard_setter_flags_on_real_change(void) {
    dashboard_api_set_power(&dashboard_handler, 5U);
    dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated = false;

    dashboard_api_set_power(&dashboard_handler, 6U);

    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated, "A genuine change must flag the box as updated");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("POW 6", dashboard_handler.text[DASHBOARD_FIELD_POWER], "The text buffer must hold the new value");
}

void test_dashboard_setter_only_touches_target_box(void) {
    dashboard_api_set_power(&dashboard_handler, 5U);
    TEST_ASSERT_TRUE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_POWER].updated, "Power box must be updated");
    TEST_ASSERT_FALSE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_REGEN].updated, "Regen box must stay clean when only power changed");
    TEST_ASSERT_FALSE_MESSAGE(dashboard_handler.boxes[DASHBOARD_FIELD_TORQUE].updated, "Torque box must stay clean when only power changed");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_dashboard_init_success);
    RUN_TEST(test_dashboard_init_null_handler);
    RUN_TEST(test_dashboard_init_populates_every_box);
    RUN_TEST(test_dashboard_init_labels_point_at_internal_buffers);
    RUN_TEST(test_dashboard_init_seeds_placeholder_text);
    RUN_TEST(test_dashboard_init_box_count_matches_field_count);

    RUN_TEST(test_dashboard_set_state_writes_text_and_flags_updated);
    RUN_TEST(test_dashboard_set_state_null_handler);
    RUN_TEST(test_dashboard_set_state_null_text);

    RUN_TEST(test_dashboard_set_power_formats_value);
    RUN_TEST(test_dashboard_set_regen_formats_value);
    RUN_TEST(test_dashboard_set_torque_formats_value);
    RUN_TEST(test_dashboard_set_power_null_handler);

    RUN_TEST(test_dashboard_set_slip_on);
    RUN_TEST(test_dashboard_set_slip_off);
    RUN_TEST(test_dashboard_set_slip_null_handler);

    RUN_TEST(test_dashboard_set_soc_formats_percent);
    RUN_TEST(test_dashboard_set_soc_clamps_above_100);
    RUN_TEST(test_dashboard_set_soc_zero);

    RUN_TEST(test_dashboard_set_hv_temp_positive);
    RUN_TEST(test_dashboard_set_hv_temp_negative);
    RUN_TEST(test_dashboard_set_inv_temp_formats_value);

    RUN_TEST(test_dashboard_set_lap_formats_current_over_total);
    RUN_TEST(test_dashboard_set_lap_delta_positive);
    RUN_TEST(test_dashboard_set_lap_delta_negative);
    RUN_TEST(test_dashboard_set_lap_delta_zero);

    RUN_TEST(test_dashboard_set_tire_temps_updates_all_four);
    RUN_TEST(test_dashboard_set_motor_temps_updates_all_four);

    RUN_TEST(test_dashboard_setter_does_not_re_flag_when_value_unchanged);
    RUN_TEST(test_dashboard_setter_flags_on_real_change);
    RUN_TEST(test_dashboard_setter_only_touches_target_box);

    return UNITY_END();
}
