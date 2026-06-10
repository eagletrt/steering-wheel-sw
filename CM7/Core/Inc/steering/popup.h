/*!
 * \file popup.h
 * \date 2026-04-24
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM7_Core
 *
 * \brief Definitions for the parameter-change popup overlay.
 *
 * \details The popup is shown whenever CM4 notifies that a tunable parameter
 *     (power, regen, torque vectoring, traction/launch control) has been changed.
 *     It replaces the main interface until SCREEN_POPUP_TIMEOUT_MS milliseconds
 *     have elapsed without further updates.
 */

#ifndef POPUP_H
#define POPUP_H

#include "inputs-shared.h"
#include "box.h"
#include "label.h"
#include <stdbool.h>
#include <stdint.h>

/*!
 * \brief Return codes for popup operations.
 */
enum PopupReturnCode {
    POPUP_RC_OK,    /*!< Operation completed successfully */
    POPUP_RC_ERROR, /*!< An error occurred during the operation */
};

/*!
 * \brief Number of RasterBox entries composing the popup interface.
 */
#define POPUP_BOX_COUNT (2U)

/*!
 * \brief Maximum length (including the terminator) of the formatted
 *     value string displayed in the popup.
 */
#define POPUP_VALUE_BUFFER_SIZE (8U)

#define POPUP_COLOR_PRIMARY (0xFF1E1E1EU)   /* dark gray */
#define POPUP_COLOR_SECONDARY (0xFFFFFFFFU) /* white */

#define POPUP_NAME_FONT_SIZE (48U)
#define POPUP_VALUE_FONT_SIZE (130U)

#define POPUP_VALUE_BOX_HEIGHT (SCREEN_HEIGHT / 10U * 7U)              /* 70% of the screen height */
#define POPUP_NAME_BOX_HEIGHT (SCREEN_HEIGHT - POPUP_VALUE_BOX_HEIGHT) /* remaining 30% */

/*!
 * \brief Popup state handler structure.
 */
struct PopupHandler {
    bool active;                                    /*!< Whether the popup should currently be rendered */
    uint32_t last_event_tick;                       /*!< Tick of the last parameter change, in milliseconds */
    enum InputsSharedParameterID current_parameter; /*!< Parameter currently displayed by the popup */
    char value_buffer[POPUP_VALUE_BUFFER_SIZE];     /*!< Buffer for the formatted value label */
    struct Label labels[POPUP_BOX_COUNT];           /*!< Labels referenced by the popup boxes */
    struct Box boxes[POPUP_BOX_COUNT];              /*!< Boxes composing the popup interface */
};

#endif // POPUP_H
