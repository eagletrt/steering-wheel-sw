/*!
 * \file test-screen-sync.c
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Unit tests for screen_api_sync_data — the IPCUIData -> dashboard pump.
 *
 * \details screen_api_sync_data forwards each field of an IPCUIData snapshot through
 *     the matching screen_api_set_*; we drive it directly and assert on the
 *     formatted text in the file-static screen_handler's dashboard buffers.
 */

#include "unity.h"
#include "fff.h"
#include "screen-api.h"
#include "raster.h"
#include <string.h>

extern struct ScreenHandler screen_handler;

DEFINE_FFF_GLOBALS;

FAKE_VALUE_FUNC(enum RasterReturnCode, fake_draw_rectangle, uint16_t, uint16_t, uint16_t, uint16_t, struct Color);

void setUp(void) {
    RESET_FAKE(fake_draw_rectangle);
    FFF_RESET_HISTORY();
    fake_draw_rectangle_fake.return_val = RASTER_RC_OK;
    memset(&screen_handler, 0, sizeof(screen_handler));
    screen_api_init(fake_draw_rectangle);
}

static struct IPCUIData prv_make_snapshot(void) {
    struct IPCUIData snapshot = { 0 };
    snapshot.power = 5U;
    snapshot.regen = 10U;
    snapshot.torque = 6U;
    snapshot.slip_on = 1U;
    snapshot.vehicle_state = IPC_UI_VEHICLE_STATE_DRIVE;
    snapshot.soc = 69U;
    snapshot.hv_temp = 104;
    snapshot.inverter_temp = 22;
    snapshot.lap_current = 9U;
    snapshot.lap_total = 11U;
    snapshot.lap_delta_ms = -420;
    snapshot.tire_fl_temp = 95;
    snapshot.tire_fr_temp = 60;
    snapshot.tire_rl_temp = 10;
    snapshot.tire_rr_temp = 40;
    snapshot.motor_fl_temp = 22;
    snapshot.motor_fr_temp = 23;
    snapshot.motor_rl_temp = 24;
    snapshot.motor_rr_temp = 25;
    return snapshot;
}

/*!
 * \defgroup screen_api_sync_data Test argument-validation tests
 * \{
 */

void test_screen_api_sync_null_pointer(void) {
    enum ScreenReturnCode rc = screen_api_sync_data(NULL);
    TEST_ASSERT_EQUAL(SCREEN_RC_NULL_POINTER, rc);
}

void test_screen_api_sync_empty_snapshot_succeeds(void) {
    struct IPCUIData empty = { 0 };
    enum ScreenReturnCode rc = screen_api_sync_data(&empty);
    TEST_ASSERT_EQUAL(SCREEN_RC_OK, rc);
}

/*! \} */

/*!
 * \defgroup screen_api_sync_data Full forwarding tests
 * \{
 */

void test_screen_api_sync_pushes_scenario_strip(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("POW 5", screen_handler.dashboard.text[DASHBOARD_FIELD_POWER], "power field must take the snapshot value");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("RGN 10", screen_handler.dashboard.text[DASHBOARD_FIELD_REGEN], "regen field must take the snapshot value");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("TQ 6", screen_handler.dashboard.text[DASHBOARD_FIELD_TORQUE], "torque field must take the snapshot value");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("SLIP ON", screen_handler.dashboard.text[DASHBOARD_FIELD_SLIP], "slip on translates to 'SLIP ON'");
}

void test_screen_api_sync_slip_off_when_zero(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    snapshot.slip_on = 0U;
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING("SLIP OFF", screen_handler.dashboard.text[DASHBOARD_FIELD_SLIP]);
}

void test_screen_api_sync_state_idle(void) {
    struct IPCUIData snapshot = { 0 };
    snapshot.vehicle_state = IPC_UI_VEHICLE_STATE_IDLE;
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING("IDLE", screen_handler.dashboard.text[DASHBOARD_FIELD_STATE]);
}

void test_screen_api_sync_state_drive(void) {
    struct IPCUIData snapshot = { 0 };
    snapshot.vehicle_state = IPC_UI_VEHICLE_STATE_DRIVE;
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING("DRIVE", screen_handler.dashboard.text[DASHBOARD_FIELD_STATE]);
}

void test_screen_api_sync_state_error(void) {
    struct IPCUIData snapshot = { 0 };
    snapshot.vehicle_state = IPC_UI_VEHICLE_STATE_ERROR;
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING("ERROR", screen_handler.dashboard.text[DASHBOARD_FIELD_STATE]);
}

void test_screen_api_sync_state_unknown_falls_back(void) {
    struct IPCUIData snapshot = { 0 };
    snapshot.vehicle_state = 200U; /* out of range */
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("----", screen_handler.dashboard.text[DASHBOARD_FIELD_STATE], "Unknown state must fall back to '----' rather than show garbage");
}

void test_screen_api_sync_pushes_hv_block(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("69%", screen_handler.dashboard.text[DASHBOARD_FIELD_HV_SOC], "SoC must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("104C", screen_handler.dashboard.text[DASHBOARD_FIELD_HV_TEMP], "HV temp must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("INV 22C", screen_handler.dashboard.text[DASHBOARD_FIELD_INV], "Inverter temp must reflect the snapshot");
}

void test_screen_api_sync_pushes_lap_block(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("LAP 9/11", screen_handler.dashboard.text[DASHBOARD_FIELD_LAP], "Lap counter must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("-0.420", screen_handler.dashboard.text[DASHBOARD_FIELD_LAP_DELTA], "Lap delta must reflect the snapshot");
}

void test_screen_api_sync_pushes_tire_block(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("95C", screen_handler.dashboard.text[DASHBOARD_FIELD_TRS_FL], "FL tire must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("60C", screen_handler.dashboard.text[DASHBOARD_FIELD_TRS_FR], "FR tire must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("10C", screen_handler.dashboard.text[DASHBOARD_FIELD_TRS_RL], "RL tire must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("40C", screen_handler.dashboard.text[DASHBOARD_FIELD_TRS_RR], "RR tire must reflect the snapshot");
}

void test_screen_api_sync_pushes_motor_block(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("22C", screen_handler.dashboard.text[DASHBOARD_FIELD_MTR_FL], "FL motor must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("23C", screen_handler.dashboard.text[DASHBOARD_FIELD_MTR_FR], "FR motor must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("24C", screen_handler.dashboard.text[DASHBOARD_FIELD_MTR_RL], "RL motor must reflect the snapshot");
    TEST_ASSERT_EQUAL_STRING_MESSAGE("25C", screen_handler.dashboard.text[DASHBOARD_FIELD_MTR_RR], "RR motor must reflect the snapshot");
}

/*! \} */

/*!
 * \defgroup screen_api_sync_data No-op tests
 * \{
 */

void test_screen_api_sync_called_twice_keeps_dashboard_clean(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        screen_handler.dashboard.boxes[i].updated = false;
    }

    enum ScreenReturnCode rc = screen_api_sync_data(&snapshot);

    TEST_ASSERT_EQUAL_MESSAGE(SCREEN_RC_OK, rc, "Second sync must still succeed");
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        TEST_ASSERT_FALSE_MESSAGE(screen_handler.dashboard.boxes[i].updated, "Replaying the same snapshot must not dirty any box");
    }
}

void test_screen_api_sync_flags_only_changed_fields(void) {
    struct IPCUIData snapshot = prv_make_snapshot();
    screen_api_sync_data(&snapshot);
    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        screen_handler.dashboard.boxes[i].updated = false;
    }

    snapshot.soc = 70U; /* only SoC moves */
    screen_api_sync_data(&snapshot);

    TEST_ASSERT_TRUE_MESSAGE(screen_handler.dashboard.boxes[DASHBOARD_FIELD_HV_SOC].updated, "Changed field must be flagged");
    TEST_ASSERT_FALSE_MESSAGE(screen_handler.dashboard.boxes[DASHBOARD_FIELD_POWER].updated, "Unchanged power must stay clean");
    TEST_ASSERT_FALSE_MESSAGE(screen_handler.dashboard.boxes[DASHBOARD_FIELD_HV_TEMP].updated, "Unchanged HV temp must stay clean");
    TEST_ASSERT_FALSE_MESSAGE(screen_handler.dashboard.boxes[DASHBOARD_FIELD_LAP].updated, "Unchanged lap must stay clean");
}

/*! \} */

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_screen_api_sync_null_pointer);
    RUN_TEST(test_screen_api_sync_empty_snapshot_succeeds);

    RUN_TEST(test_screen_api_sync_pushes_scenario_strip);
    RUN_TEST(test_screen_api_sync_slip_off_when_zero);

    RUN_TEST(test_screen_api_sync_state_idle);
    RUN_TEST(test_screen_api_sync_state_drive);
    RUN_TEST(test_screen_api_sync_state_error);
    RUN_TEST(test_screen_api_sync_state_unknown_falls_back);

    RUN_TEST(test_screen_api_sync_pushes_hv_block);
    RUN_TEST(test_screen_api_sync_pushes_lap_block);
    RUN_TEST(test_screen_api_sync_pushes_tire_block);
    RUN_TEST(test_screen_api_sync_pushes_motor_block);

    RUN_TEST(test_screen_api_sync_called_twice_keeps_dashboard_clean);
    RUN_TEST(test_screen_api_sync_flags_only_changed_fields);

    return UNITY_END();
}
