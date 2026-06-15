/*!
 * \file ipc-ui-data.h
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Cross-core snapshot consumed by the CM7 dashboard.
 *
 * \details A single struct in shared AXI SRAM holds every value the
 *     dashboard renders. CM4 writes the fields whenever a parameter changes
 *     or a telemetry packet arrives; CM7 reads them once per render and
 *     pushes any deltas into the dashboard through screen_sync. Single
 *     producer / single consumer, so no atomic protocol is needed — every
 *     field is volatile so the writes/reads are not elided, and the dashboard
 *     never blocks on a stale value because it polls the struct each frame.
 *
 *     Adding a new dashboard field is: extend IPCUIData here, extend
 *     screen_sync to forward the value, write the CM4 producer.
 */

#ifndef IPC_UI_DATA_H
#define IPC_UI_DATA_H

#include <stdint.h>

/*!
 * \brief High-level vehicle state shown in the center-top box.
 *
 * \details Generic states that map onto the CM4 FSM; the dashboard turns the
 *     enum into a short human-readable string. Add an entry here when a new
 *     state needs to surface on the UI.
 */
enum IPCUIVehicleState {
    IPC_UI_VEHICLE_STATE_IDLE,       /*!< "IDLE"   */
    IPC_UI_VEHICLE_STATE_READY,      /*!< "READY"  */
    IPC_UI_VEHICLE_STATE_DRIVE,      /*!< "DRIVE"  */
    IPC_UI_VEHICLE_STATE_AUTONOMOUS, /*!< "AUTO"   */
    IPC_UI_VEHICLE_STATE_FLASH,      /*!< "FLASH"  */
    IPC_UI_VEHICLE_STATE_ERROR,      /*!< "ERROR"  */
    IPC_UI_VEHICLE_STATE_COUNT,
};

/*!
 * \brief Shared snapshot rendered by the dashboard.
 *
 * \details Every field is volatile because the struct lives in shared AXI
 *     SRAM and is written by CM4 while CM7 reads it. The layout is grouped
 *     by visual section to make the matching screen_sync code easy to read.
 */
struct IPCUIData {
    /* Scenario presets (mirror of CM4 parameters). */
    volatile uint8_t power;   /*!< 0..10 */
    volatile uint8_t regen;   /*!< 0..10 */
    volatile uint8_t torque;  /*!< 0..10 */
    volatile uint8_t slip_on; /*!< 0/1 (slip / traction control toggle) */

    /* Vehicle state shown in the center-top box. */
    volatile uint8_t vehicle_state; /*!< IPCUIVehicleState as a uint8_t to keep the layout packed */

    /* High-voltage pack. */
    volatile uint8_t soc;     /*!< State of charge, 0..100 */
    volatile int16_t hv_temp; /*!< Pack temperature in °C */

    /* Inverter. */
    volatile int16_t inverter_temp; /*!< Inverter temperature in °C */

    /* Lap. */
    volatile uint8_t lap_current;  /*!< Current lap number */
    volatile uint8_t lap_total;    /*!< Total laps in the session */
    volatile int32_t lap_delta_ms; /*!< Signed delta vs reference lap, in ms */

    /* Tire temperatures (clockwise FL, FR, RL, RR). */
    volatile int16_t tire_fl_temp;
    volatile int16_t tire_fr_temp;
    volatile int16_t tire_rl_temp;
    volatile int16_t tire_rr_temp;

    /* Motor temperatures (same layout as tires). */
    volatile int16_t motor_fl_temp;
    volatile int16_t motor_fr_temp;
    volatile int16_t motor_rl_temp;
    volatile int16_t motor_rr_temp;
};

#endif // IPC_UI_DATA_H
