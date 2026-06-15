#ifndef SHARED_IPC_H
#define SHARED_IPC_H

#include "ipc-queue.h"
#include "eagletrt.h"

/*!
 * \brief IPC input queue shared between CM4 and CM7.
 *
 * \details Defined as file-static in the .shared_axi section. The linker
 *     scripts of both firmwares pin this section to the same physical AXI
 *     SRAM address, so each firmware's local static refers to the same
 *     bytes.
 *
 *     Only ipc-queue-api.c may include this header; other modules go
 *     through ipc-queue-api or one of the dedicated accessors (e.g.
 *     ipc-ui-data-api.h).
 */
EAGLETRT_STATIC struct IPCInputQueue ipc_input
    __attribute__((section(".shared_axi"), aligned(32)));

#endif // SHARED_IPC_H
