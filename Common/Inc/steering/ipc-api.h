#ifndef IPC_API_H
#define IPC_API_H

#include "ipc.h"
#include <stdbool.h>

/*!
 * \brief Read and process all input events in the IPC input queue.
 *
 * \param queue IPC input queue to read from.
 * \param callback Callback function to handle each input event.
 */
void ipc_api_read_and_process_all(struct IPCInputQueue *queue, void (*callback)(struct InputEvent *ev));

/*!
 * \brief Push an input event to the IPC input queue.
 *
 * \param queue IPC input queue to push to.
 * \param ev Input event to push.
 * \param dmb_callback Callback function to ensure data memory ensure
 *
 * \return true if the event was successfully pushed, false otherwise.
 */
bool ipc_api_push_event(struct IPCInputQueue *queue, struct InputEvent *ev, void (*dmb_callback)(void));

#endif // IPC_API_H
