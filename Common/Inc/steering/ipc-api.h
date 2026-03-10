#ifndef IPC_API_H
#define IPC_API_H

#include <stdbool.h>
#include "inputs-shared.h"

/*!
 * \brief Read and process all input events in the IPC input queue.
 *
 * \param callback Callback function to handle each input event.
 */
void ipc_api_read_and_process_all(void (*callback)(struct InputsSharedEvent *ev));

/*!
 * \brief Push an input event to the IPC input queue.
 *
 * \param ev Input event to push.
 * \param dmb_callback Callback function to ensure data memory ensure
 *
 * \return true if the event was successfully pushed, false otherwise.
 */
bool ipc_api_push_event(struct InputsSharedEvent *ev, void (*dmb_callback)(void));

#endif // IPC_API_H
