#ifndef IPC_API_H
#define IPC_API_H

#include <stdbool.h>
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
void ipc_api_read_and_process_all(void (*callback)(struct InputsSharedEvent *ev));

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
bool ipc_api_push_event(struct InputsSharedEvent *ev, void (*critical_section_callback)(void));

#endif // IPC_API_H
