#ifndef SCREEN_H
#define SCREEN_H

#include "raster.h"
#include <stddef.h>

#define SCREEN_WIDTH (800U)
#define SCREEN_HEIGHT (480U)

#define SCREEN_POPUP_TIMEOUT_MS (2500U)

/*!
 * \brief Return codes for screen operations
 */
enum ScreenReturnCode {
    SCREEN_RC_OK,    /*!< Operation completed successfully */
    SCREEN_RC_ERROR, /*!< An error occurred during the operation */
};

/*!
 * \brief Main screen handler structure
 */
struct ScreenHandler {
    uint32_t last_event_tick; /*!< Timestamp of the last screen event, in milliseconds */

    struct RasterHandler raster; /*!< Handler for raster operations */
};

#endif // SCREEN_H
