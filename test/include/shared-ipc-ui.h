#ifndef MOCK_SHARED_IPC_UI_H
#define MOCK_SHARED_IPC_UI_H

#include "ipc-ui-data.h"

/*!
 * \brief Host-side stand-in for shared-ipc-ui.h.
 *
 * \details Mirrors the production declaration without the .shared_axi
 *     section attribute.
 */
struct IPCUIData ipc_ui_data;

#endif // MOCK_SHARED_IPC_UI_H
