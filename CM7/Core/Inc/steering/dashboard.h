/*!
 * \file dashboard.h
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Types and storage for the main steering-wheel dashboard interface.
 *
 * \details The dashboard is the always-on view: a strip of telemetry/setup
 *     boxes (scenario presets on the left, HV/INV in the middle, lap +
 *     tire/motor temperatures on the right). Every field on screen is one
 *     Box backed by one Label whose text comes from a small writable buffer
 *     stored in the handler. Setters re-format that buffer and flag the
 *     box as updated so the raster picks the change up on the next render.
 */

#ifndef DASHBOARD_H
#define DASHBOARD_H

#include "box.h"
#include "label.h"

#define DASHBOARD_COLOR_PRIMARY (0xFF1E1E1EU)    /* dark gray */
#define DASHBOARD_COLOR_TERTIARY (0xFFFFFFFFU)   /* white */
#define DASHBOARD_COLOR_ALL_OK (0xFF39D103U)     /* bright green */
#define DASHBOARD_COLOR_WARNING (0xFFFFFA14U)    /* yellow */
#define DASHBOARD_COLOR_ERROR (0xFFFF0000U)      /* red */
#define DASHBOARD_COLOR_FAST_LAP (0xFFFF00E6U)   /* magenta */
#define DASHBOARD_COLOR_COLD_TIRES (0xFF00CAE9U) /* cyan */

#define DASHBOARD_THRESHOLD_HW_SOC_WARNING (30U)    /* % and below is a warning */
#define DASHBOARD_THRESHOLD_HW_SOC_ERROR (15U)      /* % and below is an error */
#define DASHBOARD_THRESHOLD_HV_TEMP_WARNING (45U)   /* °C and above is a warning */
#define DASHBOARD_THRESHOLD_HV_TEMP_ERROR (50U)     /* °C and above is an error */
#define DASHBOARD_THRESHOLD_INV_TEMP_WARNING (60U)  /* °C and above is a warning */
#define DASHBOARD_THRESHOLD_INV_TEMP_ERROR (70U)    /* °C and above is an error */
#define DASHBOARD_THRESHOLD_TIRE_TEMP_LOW (30U)     /* °C and below is low */
#define DASHBOARD_THRESHOLD_TIRE_TEMP_WARNING (80U) /* °C and above is high */
#define DASHBOARD_THRESHOLD_TIRE_TEMP_ERROR (100U)  /* °C and above is an error */
#define DASHBOARD_THRESHOLD_MTR_TEMP_WARNING (80U)  /* °C and above is high */
#define DASHBOARD_THRESHOLD_MTR_TEMP_ERROR (100U)   /* °C and above is an error */

#define DASHBOARD_FONT_SIZE_HEADER (22U)
#define DASHBOARD_FONT_SIZE_VALUE (28U)
#define DASHBOARD_FONT_SIZE_STATE (36U)
#define DASHBOARD_FONT_SIZE_DELTA (40U)
#define DASHBOARD_FONT_SIZE_SOC (88U)

/*!
 * \brief Return codes for dashboard operations.
 */
enum DashboardReturnCode {
    DASHBOARD_RC_OK,           /*!< Operation completed successfully */
    DASHBOARD_RC_NULL_POINTER, /*!< A required pointer argument was NULL */
    DASHBOARD_RC_ERROR,        /*!< General error */
};

/*!
 * \brief Identifier of every drawable field on the dashboard.
 *
 * \details Each entry maps one-to-one to a Box and a Label in DashboardHandler.
 *     The ordering only matters for traversal in dashboard_api_init; consumers
 *     never index by raw integer.
 */
enum DashboardFieldId {
    DASHBOARD_FIELD_SCENARIO_HEADER, /*!< "SCENARIO" header, left strip top */
    DASHBOARD_FIELD_REGEN,           /*!< "RGN 10" */
    DASHBOARD_FIELD_TORQUE,          /*!< "TQ 6" */
    DASHBOARD_FIELD_POWER,           /*!< "POW 5" */
    DASHBOARD_FIELD_SLIP,            /*!< "SLIP ON/OFF" */

    DASHBOARD_FIELD_STATE,     /*!< Vehicle FSM state, center top */
    DASHBOARD_FIELD_HV_HEADER, /*!< "HV" header */
    DASHBOARD_FIELD_HV_SOC,    /*!< "69%" large state-of-charge value */
    DASHBOARD_FIELD_HV_TEMP,   /*!< "104°C" pack temperature */
    DASHBOARD_FIELD_INV,       /*!< "INV 22°C" inverter temperature */

    DASHBOARD_FIELD_LAP,        /*!< "LAP 9/11" lap counter */
    DASHBOARD_FIELD_LAP_DELTA,  /*!< "-0.420" lap delta in seconds */
    DASHBOARD_FIELD_TRS_HEADER, /*!< "TRS" tire-temps header */
    DASHBOARD_FIELD_TRS_FL,     /*!< Front-left tire temperature */
    DASHBOARD_FIELD_TRS_FR,     /*!< Front-right tire temperature */
    DASHBOARD_FIELD_TRS_RL,     /*!< Rear-left tire temperature */
    DASHBOARD_FIELD_TRS_RR,     /*!< Rear-right tire temperature */
    DASHBOARD_FIELD_MTR_HEADER, /*!< "MTR" motor-temps header */
    DASHBOARD_FIELD_MTR_FL,     /*!< Front-left motor temperature */
    DASHBOARD_FIELD_MTR_FR,     /*!< Front-right motor temperature */
    DASHBOARD_FIELD_MTR_RL,     /*!< Rear-left motor temperature */
    DASHBOARD_FIELD_MTR_RR,     /*!< Rear-right motor temperature */

    DASHBOARD_FIELD_COUNT, /*!< Total number of fields, also the box count */
};

/*!
 * \brief Inclusive size (including the terminator) of each per-field text buffer.
 */
#define DASHBOARD_TEXT_BUFFER_SIZE (16U)

/*!
 * \brief Total number of boxes composing the dashboard.
 *
 * \details Mirrors DASHBOARD_FIELD_COUNT for use in array sizing and raster
 *     calls without depending on the enum's underlying type.
 */
#define DASHBOARD_BOX_COUNT ((uint16_t)DASHBOARD_FIELD_COUNT)

/*!
 * \brief Self-contained storage for the dashboard interface.
 *
 * \details Owns the value buffers, labels and boxes. Labels point at the
 *     buffers inside this handler, so the whole layout can be embedded in
 *     a parent struct (e.g. ScreenHandler) and moved/copied without breaking
 *     pointer relationships as long as the handler is not duplicated.
 */
struct DashboardHandler {
    char text[DASHBOARD_FIELD_COUNT][DASHBOARD_TEXT_BUFFER_SIZE]; /*!< Mutable backing strings */
    struct Label labels[DASHBOARD_FIELD_COUNT];                   /*!< One label per field */
    struct Box boxes[DASHBOARD_FIELD_COUNT];                      /*!< Boxes mounted on the raster */
};

#endif // DASHBOARD_H
