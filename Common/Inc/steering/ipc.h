/*!
 * \file ipc.h
 * \date 2026-01-20
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Inter-Processor Communication (IPC) definitions for input event sharing between CM4 and CM7.
 */

#ifndef IPC_H
#define IPC_H

#include "inputs-shared.h"

/*!
 * \brief Size of the IPC input event ring buffer.
 */
#define IPC_INPUT_QUEUE_SIZE (64U)

/*!
 * \brief Lock-free single-producer single-consumer ring buffer for input events.
 */
struct IPCInputQueue {
    uint32_t write_idx;                                    /*!< Next slot to write (owned by CM4) */
    uint32_t read_idx;                                     /*!< Next slot to read  (owned by CM7) */
    struct InputsSharedEvent events[IPC_INPUT_QUEUE_SIZE]; /*!< Ring buffer storage */
};

#endif // IPC_H
