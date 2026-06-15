#ifndef MOCK_SHARED_IPC_H
#define MOCK_SHARED_IPC_H

#include "ipc-queue.h"

/*!
 * \brief Host-side stand-in for shared-ipc.h.
 *
 * \details Mirrors the production declaration without the .shared_axi
 *     section attribute.
 */
struct IPCInputQueue ipc_input;

#endif // MOCK_SHARED_IPC_H
