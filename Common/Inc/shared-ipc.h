#ifndef SHARED_IPC_H
#define SHARED_IPC_H

#include "ipc-queue.h"
#include "ipc-ui-data.h"
#include "eagletrt.h"

/*!
 * \brief IPC input queue shared between CM4 and CM7.
 */
EAGLETRT_STATIC struct IPCInputQueue ipc_input
    __attribute__((section(".shared_axi"), aligned(32)));

EAGLETRT_STATIC struct IPCUIData ipc_ui_data
    __attribute__((section(".shared_axi"), aligned(32)));

#endif // SHARED_IPC_H
