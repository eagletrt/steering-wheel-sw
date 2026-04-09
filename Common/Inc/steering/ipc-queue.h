/*!
 * \file ipc-queue.h
 * \date 2026-01-20
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Inter-Processor Communication (IPC) Queue definitions for input event sharing between CM4 and CM7.
 */

#ifndef IPC_QUEUE_H
#define IPC_QUEUE_H

#include "inputs-shared.h"

/*!
 * \brief Size of the IPC input event ring buffer.
 */
#define IPC_INPUT_QUEUE_SIZE (64U)

/*!
 * \brief Callback type for processing input events read from the IPC input queue.
 *
 * This callback is invoked for each input event read from the queue, allowing the caller to handle the event as needed.
 *
 * \param ev The input event to process.
 */
typedef void (*ipc_process_event_callback)(struct InputsSharedEvent ev);

/*!
 * \brief Callback type for critical section operations when pushing an event to the IPC input queue.
 *
 * This callback is executed before updating the index of the input event in the shared memory, allowing the caller to perform any necessary critical section operations before the event is made available to other processes.
 */
typedef void (*ipc_critical_section_callback)(void);

/*!
 * \brief Lock-free single-producer single-consumer ring buffer for input events.
 */
struct IPCInputQueue {
    uint32_t write_idx;                                    /*!< Next slot to write (owned by CM4) */
    uint32_t read_idx;                                     /*!< Next slot to read  (owned by CM7) */
    struct InputsSharedEvent events[IPC_INPUT_QUEUE_SIZE]; /*!< Ring buffer storage */
};

#endif // IPC_QUEUE_H
