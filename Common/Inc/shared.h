#ifndef SHARED_H
#define SHARED_H

#include "eagletrt.h"
#include "ipc.h"

/*!
 * \brief HSEM ID used by CM4 to notify CM7 about input events.
 */
#define HSEM_INPUT_ID (1U)

/*!
 * \brief IPC input queue shared between CM4 and CM7.
 */
EAGLETRT_STATIC struct IPCInputQueue ipc_input
    __attribute__((section(".shared_axi"), aligned(32)));

#endif // SHARED_H
