/*!
 * \file ipc-ui-data-api.c
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Implementation of the IPCUIData accessor.
 *
 * \details The shared instance is declared in shared-ipc-ui.h so the same
 *     translation unit can be transparently replaced by a host-side mock
 *     during unit testing (mach-o builds can't carry the .shared_axi
 *     section attribute that production needs).
 */

#include "ipc-ui-data-api.h"
#include "shared-ipc-ui.h"
#include <string.h>

void ipc_ui_data_api_init(void) {
    memset(&ipc_ui_data, 0, sizeof(ipc_ui_data));
}

struct IPCUIData *ipc_ui_data_api_get(void) {
    return &ipc_ui_data;
}
