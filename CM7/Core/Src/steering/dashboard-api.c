/*!
 * \file dashboard-api.c
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Implementation of the dashboard layout and setters.
 *
 * \details The layout table drives initialization: each entry pins one
 *     field to a screen rectangle, a font size and a text color. Setters
 *     each touch exactly one field's value buffer and flag its box as
 *     updated. Adding a new field is a row in the table + (if needed) a
 *     setter; no other module needs to change.
 */

#include "dashboard-api.h"
#include "box-api.h"
#include "label-api.h"
#include "raster-fonts.h"
#include "eagletrt-api.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* ----- layout table -----
 *
 * The 800x480 surface is split into three vertical strips:
 *   left   ( 0..220) — scenario presets and toggles
 *   center (220..520) — vehicle state and HV/INV telemetry
 *   right  (520..800) — lap counter and tire/motor temperatures
 */

struct DashboardFieldLayout {
    struct BoxRectangle rect; /*!< Where the box sits on the 800x480 canvas */
    uint16_t font_size;       /*!< Pixel height of the rendered text */
    uint32_t text_argb;       /*!< Foreground color of the label, ARGB */
    const char *initial_text; /*!< Placeholder shown until the first setter call */
};

// clang-format off

EAGLETRT_STATIC const struct DashboardFieldLayout prv_dashboard_layout[DASHBOARD_FIELD_COUNT] = {
    /* left strip */
    [DASHBOARD_FIELD_SCENARIO_HEADER] = { {   0,   0, 220,  80 }, DASHBOARD_FONT_SIZE_HEADER, DASHBOARD_COLOR_TERTIARY,    "SCENARIO" },
    [DASHBOARD_FIELD_REGEN]           = { {   0,  80, 220, 100 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "RGN --"   },
    [DASHBOARD_FIELD_TORQUE]          = { {   0, 180, 220, 100 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "TQ --"    },
    [DASHBOARD_FIELD_POWER]           = { {   0, 280, 220, 100 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "POW --"   },
    [DASHBOARD_FIELD_SLIP]            = { {   0, 380, 220, 100 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "SLIP --"  },

    /* center strip */
    [DASHBOARD_FIELD_STATE]           = { { 220,   0, 300,  80 }, DASHBOARD_FONT_SIZE_STATE,  DASHBOARD_COLOR_TERTIARY,    "----"     },
    [DASHBOARD_FIELD_HV_HEADER]       = { { 220,  80, 300,  60 }, DASHBOARD_FONT_SIZE_HEADER, DASHBOARD_COLOR_TERTIARY,    "HV"       },
    [DASHBOARD_FIELD_HV_SOC]          = { { 220, 140, 300, 180 }, DASHBOARD_FONT_SIZE_SOC,    DASHBOARD_COLOR_TERTIARY,    "--%"      },
    [DASHBOARD_FIELD_HV_TEMP]         = { { 220, 320, 300,  60 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_WARNING,     "--C"      },
    [DASHBOARD_FIELD_INV]             = { { 220, 380, 300, 100 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "INV --C"  },

    /* right strip */
    [DASHBOARD_FIELD_LAP]             = { { 520,   0, 280,  80 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "LAP -/-"  },
    [DASHBOARD_FIELD_LAP_DELTA]       = { { 520,  80, 280,  60 }, DASHBOARD_FONT_SIZE_DELTA,  DASHBOARD_COLOR_TERTIARY,    "0.000"    },
    [DASHBOARD_FIELD_TRS_HEADER]      = { { 520, 140, 280,  40 }, DASHBOARD_FONT_SIZE_HEADER, DASHBOARD_COLOR_TERTIARY,    "TRS"      },
    [DASHBOARD_FIELD_TRS_FL]          = { { 520, 180, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_TRS_FR]          = { { 660, 180, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_TRS_RL]          = { { 520, 245, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_TRS_RR]          = { { 660, 245, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_MTR_HEADER]      = { { 520, 310, 280,  40 }, DASHBOARD_FONT_SIZE_HEADER, DASHBOARD_COLOR_TERTIARY,    "MTR"      },
    [DASHBOARD_FIELD_MTR_FL]          = { { 520, 350, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_MTR_FR]          = { { 660, 350, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_MTR_RL]          = { { 520, 415, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
    [DASHBOARD_FIELD_MTR_RR]          = { { 660, 415, 140,  65 }, DASHBOARD_FONT_SIZE_VALUE,  DASHBOARD_COLOR_TERTIARY,    "--C"      },
};
// clang-format on

/*!
 * \brief Pixel offset from a box's top-left to its label anchor.
 *
 * \details With FONT_ALIGN_CENTER the X anchor is the horizontal middle of
 *     the box; the Y anchor is the top of the rendered glyphs, so we center
 *     the line vertically against the box height.
 */
EAGLETRT_STATIC int16_t prv_dashboard_api_label_offset_y(uint16_t box_height, uint16_t font_size) {
    if (box_height <= font_size) {
        return 0;
    }
    return (int16_t)((box_height - font_size) / 2U);
}

/*!
 * \brief Refresh one field's text buffer and mark its box dirty if it changed.
 *
 * \details Formats into a scratch buffer first and compares to the current
 *     content: if identical we leave the box's \c updated flag alone so the
 *     raster stays in partial mode when the consumer calls every setter each
 *     tick. The label keeps the per-field buffer pointer set at init, so
 *     updating it in place is enough to drive the next render.
 */
EAGLETRT_STATIC void prv_dashboard_api_format_field(struct DashboardHandler *handler, enum DashboardFieldId field_id, const char *fmt, ...) {
    char scratch[DASHBOARD_TEXT_BUFFER_SIZE];
    va_list args;
    va_start(args, fmt);
    if (vsnprintf(scratch, sizeof(scratch), fmt, args) < 0) {
        /* Formatting error, leave the field unchanged. */
        va_end(args);
        return;
    }
    va_end(args);

    if (strncmp(scratch, handler->text[field_id], DASHBOARD_TEXT_BUFFER_SIZE) == 0) {
        return;
    }
    memcpy(handler->text[field_id], scratch, sizeof(scratch));
    handler->boxes[field_id].updated = true;
}

enum DashboardReturnCode dashboard_api_init(struct DashboardHandler *handler) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }

    memset(handler, 0, sizeof(*handler));

    const struct Color bg_color = { .argb = DASHBOARD_COLOR_PRIMARY };

    for (uint16_t i = 0U; i < DASHBOARD_FIELD_COUNT; i++) {
        const struct DashboardFieldLayout *field_layout = &prv_dashboard_layout[i];

        (void)snprintf(handler->text[i], DASHBOARD_TEXT_BUFFER_SIZE, "%s", field_layout->initial_text);

        const int16_t offset_x = (int16_t)(field_layout->rect.width / 2U);
        const int16_t offset_y = prv_dashboard_api_label_offset_y(field_layout->rect.height, field_layout->font_size);
        const struct Color text_color = { .argb = field_layout->text_argb };

        if (label_api_init(&handler->labels[i], handler->text[i], offset_x, offset_y, &font_konexy, field_layout->font_size, FONT_ALIGN_CENTER, text_color) != RASTER_RC_OK) {
            return DASHBOARD_RC_ERROR;
        }
        if (box_api_init(&handler->boxes[i], i, field_layout->rect, bg_color, &handler->labels[i]) != RASTER_RC_OK) {
            return DASHBOARD_RC_ERROR;
        }
    }

    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_state(struct DashboardHandler *handler, const char *text) {
    if (handler == NULL || text == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_STATE, "%s", text);
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_power(struct DashboardHandler *handler, uint8_t value) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_POWER, "POW %u", (unsigned)value);
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_regen(struct DashboardHandler *handler, uint8_t value) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_REGEN, "RGN %u", (unsigned)value);
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_torque(struct DashboardHandler *handler, uint8_t value) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_TORQUE, "TQ %u", (unsigned)value);
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_slip(struct DashboardHandler *handler, bool slip_on) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_SLIP, "SLIP %s", slip_on ? "ON" : "OFF");
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_soc(struct DashboardHandler *handler, uint8_t percent) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    percent = EAGLETRT_API_CLAMP(percent, 0U, 100U);
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_HV_SOC, "%u%%", (unsigned)percent);

    if (percent <= DASHBOARD_THRESHOLD_HW_SOC_ERROR) {
        handler->labels[DASHBOARD_FIELD_HV_SOC].color.argb = DASHBOARD_COLOR_ERROR;
    } else if (percent <= DASHBOARD_THRESHOLD_HW_SOC_WARNING) {
        handler->labels[DASHBOARD_FIELD_HV_SOC].color.argb = DASHBOARD_COLOR_WARNING;
    } else {
        handler->labels[DASHBOARD_FIELD_HV_SOC].color.argb = DASHBOARD_COLOR_TERTIARY;
    }

    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_hv_temp(struct DashboardHandler *handler, int16_t celsius) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_HV_TEMP, "%dC", (int)celsius);

    if (celsius >= (int16_t)DASHBOARD_THRESHOLD_HV_TEMP_ERROR) {
        handler->labels[DASHBOARD_FIELD_HV_TEMP].color.argb = DASHBOARD_COLOR_ERROR;
    } else if (celsius >= (int16_t)DASHBOARD_THRESHOLD_HV_TEMP_WARNING) {
        handler->labels[DASHBOARD_FIELD_HV_TEMP].color.argb = DASHBOARD_COLOR_WARNING;
    } else {
        handler->labels[DASHBOARD_FIELD_HV_TEMP].color.argb = DASHBOARD_COLOR_TERTIARY;
    }

    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_inv_temp(struct DashboardHandler *handler, int16_t celsius) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_INV, "INV %dC", (int)celsius);

    if (celsius >= (int16_t)DASHBOARD_THRESHOLD_INV_TEMP_ERROR) {
        handler->labels[DASHBOARD_FIELD_INV].color.argb = DASHBOARD_COLOR_ERROR;
    } else if (celsius >= (int16_t)DASHBOARD_THRESHOLD_INV_TEMP_WARNING) {
        handler->labels[DASHBOARD_FIELD_INV].color.argb = DASHBOARD_COLOR_WARNING;
    } else {
        handler->labels[DASHBOARD_FIELD_INV].color.argb = DASHBOARD_COLOR_TERTIARY;
    }

    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_lap(struct DashboardHandler *handler, uint8_t current, uint8_t total) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_LAP, "LAP %u/%u", (unsigned)current, (unsigned)total);
    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_lap_delta_ms(struct DashboardHandler *handler, int32_t delta_ms) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    const char sign = (delta_ms < 0) ? '-' : '+';
    int32_t magnitude = (delta_ms < 0) ? -delta_ms : delta_ms;
    int32_t whole = magnitude / 1000;
    int32_t millis = magnitude % 1000;
    prv_dashboard_api_format_field(handler, DASHBOARD_FIELD_LAP_DELTA, "%c%ld.%03ld", sign, (long)whole, (long)millis);
    return DASHBOARD_RC_OK;
}

/*!
 * \brief Shared helper for the TRS / MTR 4-temperature blocks.
 *
 * \param[in,out] handler         Dashboard storage.
 * \param[in]     front_left_id   Field ID for the front-left temperature.
 * \param[in]     front_right_id  Field ID for the front-right temperature.
 * \param[in]     rear_left_id    Field ID for the rear-left temperature.
 * \param[in]     rear_right_id   Field ID for the rear-right temperature.
 * \param[in]     front_left      Front-left temperature in °C.
 * \param[in]     front_right     Front-right temperature in °C.
 * \param[in]     rear_left       Rear-left temperature in °C.
 * \param[in]     rear_right      Rear-right temperature in °C.
 */
EAGLETRT_STATIC void prv_dashboard_api_set_temp_quad(
    struct DashboardHandler *handler,
    enum DashboardFieldId front_left_id,
    enum DashboardFieldId front_right_id,
    enum DashboardFieldId rear_left_id,
    enum DashboardFieldId rear_right_id,
    int16_t front_left,
    int16_t front_right,
    int16_t rear_left,
    int16_t rear_right) {
    prv_dashboard_api_format_field(handler, front_left_id, "%dC", (int)front_left);
    prv_dashboard_api_format_field(handler, front_right_id, "%dC", (int)front_right);
    prv_dashboard_api_format_field(handler, rear_left_id, "%dC", (int)rear_left);
    prv_dashboard_api_format_field(handler, rear_right_id, "%dC", (int)rear_right);
}

/*!
 * \brief Helper to determine the color for a tire temperature value based on thresholds.
 *
 * \param temp Tire temperature in °C.
 *
 * \return ARGB color code corresponding to the temperature thresholds:
 */
EAGLETRT_STATIC uint32_t prv_dashboard_api_tire_temp_color(int16_t temp) {
    if (temp <= (int16_t)DASHBOARD_THRESHOLD_TIRE_TEMP_LOW) {
        return DASHBOARD_COLOR_COLD_TIRES;
    }
    if (temp >= (int16_t)DASHBOARD_THRESHOLD_TIRE_TEMP_ERROR) {
        return DASHBOARD_COLOR_ERROR;
    }
    if (temp >= (int16_t)DASHBOARD_THRESHOLD_TIRE_TEMP_WARNING) {
        return DASHBOARD_COLOR_WARNING;
    }
    return DASHBOARD_COLOR_TERTIARY;
}

/*!
 * \brief Helper to determine the color for a motor temperature value based on thresholds.
 *
 * \param temp Motor temperature in °C.
 *
 * \return ARGB color code corresponding to the temperature thresholds:
 */
EAGLETRT_STATIC uint32_t prv_dashboard_api_motor_temp_color(int16_t temp) {
    if (temp >= (int16_t)DASHBOARD_THRESHOLD_MTR_TEMP_ERROR) {
        return DASHBOARD_COLOR_ERROR;
    }
    if (temp >= (int16_t)DASHBOARD_THRESHOLD_MTR_TEMP_WARNING) {
        return DASHBOARD_COLOR_WARNING;
    }
    return DASHBOARD_COLOR_TERTIARY;
}

enum DashboardReturnCode dashboard_api_set_tire_temps(struct DashboardHandler *handler, int16_t front_left, int16_t front_right, int16_t rear_left, int16_t rear_right) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_set_temp_quad(handler, DASHBOARD_FIELD_TRS_FL, DASHBOARD_FIELD_TRS_FR, DASHBOARD_FIELD_TRS_RL, DASHBOARD_FIELD_TRS_RR, front_left, front_right, rear_left, rear_right);

    handler->labels[DASHBOARD_FIELD_TRS_FL].color.argb = prv_dashboard_api_tire_temp_color(front_left);
    handler->labels[DASHBOARD_FIELD_TRS_FR].color.argb = prv_dashboard_api_tire_temp_color(front_right);
    handler->labels[DASHBOARD_FIELD_TRS_RL].color.argb = prv_dashboard_api_tire_temp_color(rear_left);
    handler->labels[DASHBOARD_FIELD_TRS_RR].color.argb = prv_dashboard_api_tire_temp_color(rear_right);

    return DASHBOARD_RC_OK;
}

enum DashboardReturnCode dashboard_api_set_motor_temps(struct DashboardHandler *handler, int16_t front_left, int16_t front_right, int16_t rear_left, int16_t rear_right) {
    if (handler == NULL) {
        return DASHBOARD_RC_NULL_POINTER;
    }
    prv_dashboard_api_set_temp_quad(handler, DASHBOARD_FIELD_MTR_FL, DASHBOARD_FIELD_MTR_FR, DASHBOARD_FIELD_MTR_RL, DASHBOARD_FIELD_MTR_RR, front_left, front_right, rear_left, rear_right);

    handler->labels[DASHBOARD_FIELD_MTR_FL].color.argb = prv_dashboard_api_motor_temp_color(front_left);
    handler->labels[DASHBOARD_FIELD_MTR_FR].color.argb = prv_dashboard_api_motor_temp_color(front_right);
    handler->labels[DASHBOARD_FIELD_MTR_RL].color.argb = prv_dashboard_api_motor_temp_color(rear_left);
    handler->labels[DASHBOARD_FIELD_MTR_RR].color.argb = prv_dashboard_api_motor_temp_color(rear_right);

    return DASHBOARD_RC_OK;
}
