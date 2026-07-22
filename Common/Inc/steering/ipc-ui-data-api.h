/*!
 * \file ipc-ui-data-api.h
 * \date 2026-06-07
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Accessor for the shared UI snapshot in AXI SRAM.
 *
 * \details The IPCUIData instance lives behind the API so that shared-ipc.h
 *     is included from exactly one translation unit — the linker section
 *     trick that maps CM4's and CM7's storage to the same physical bytes
 *     only works when each firmware contains a single definition. Callers
 *     never include shared-ipc.h themselves; they go through
 *     ipc_ui_data_api_get and read or write the volatile fields directly.
 */

#ifndef IPC_UI_DATA_API_H
#define IPC_UI_DATA_API_H

#include "ipc-ui-data.h"

/*!
 * \brief Initialise the shared IPCUIData instance.
 *
 * \details This function is generally called by the producer core at startup.
 */
void ipc_ui_data_api_init(void);

/*!
 * \brief Return the shared IPCUIData instance.
 *
 * \details Both cores get a pointer to the same physical memory thanks to
 *     the .shared_axi linker section. The fields are individually volatile,
 *     so a read or write through the returned pointer is observed across
 *     the core boundary without further synchronisation.
 *
 * \return Pointer to the shared IPCUIData snapshot.
 */
struct IPCUIData *ipc_ui_data_api_get(void);

#endif // IPC_UI_DATA_API_H
