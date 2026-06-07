#ifndef SHARED_IPC_UI_H
#define SHARED_IPC_UI_H

#include "ipc-ui-data.h"
#include "eagletrt.h"

/*!
 * \brief Shared UI snapshot consumed by CM7 and written by CM4.
 *
 * \details Defined as file-static in the .shared_axi section. The linker
 *     scripts of both firmwares pin this section to the same physical AXI
 *     SRAM address, so each firmware's local static refers to the same
 *     bytes.
 *
 *     Only ipc-ui-data-api.c may include this header; other modules go
 *     through ipc_ui_data_api_get. A test-only mock in test/include shadows
 *     this declaration on host builds where the section attribute is not
 *     valid.
 */
EAGLETRT_STATIC struct IPCUIData ipc_ui_data
    __attribute__((section(".shared_axi"), aligned(32)));

#endif // SHARED_IPC_UI_H
