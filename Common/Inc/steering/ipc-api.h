/*!
 * \file ipc-api.h
 * \date 2026-01-20
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief API for Inter-Processor Communication (IPC) to manage input events between CM4 and CM7.
 */

#ifndef IPC_API_H
#define IPC_API_H

#include <stdbool.h>
#include "ipc.h"
#include "inputs-shared.h"

/*!
 * \brief Empties the IPC input queue by resetting the whole memory to 0s.
 */
void ipc_api_reset(void);

/*!
 * \brief Read and process all input events in the IPC input queue.
 *
 * \param callback Callback function to handle each input event.
 */
void ipc_api_read_and_process_all(ipc_process_event_callback callback);

/*!
 * \brief Push an input event to the IPC input queue.
 *
 * \param ev Input event to push.
 * \param critical_section_callback Callback function to be executed before updating the index of the input event
 *        in the shared memory. This allows the caller to perform any necessary critical section operations before 
 *        the event is made available to other processes (can be NULL).
 *
 * \return true if the event was successfully pushed, false otherwise.
 */
bool ipc_api_push_event(struct InputsSharedEvent ev, ipc_critical_section_callback critical_section_callback);

#endif // IPC_API_H
