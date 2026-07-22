/*!
 * \file ipc-queue-api.c
 * \date 2026-01-20
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Implementation of the IPC Queue API for managing input events between CM4 and CM7.
 */

#include "ipc-queue-api.h"
#include "shared-ipc.h"
#include <stdio.h>

void ipc_queue_api_init(void) {
    ipc_input = (struct IPCInputQueue){ 0 };
}

void ipc_queue_api_read_and_process_all(ipc_process_event_callback callback) {
    while (ipc_input.read_idx != ipc_input.write_idx) {
        struct InputsSharedEvent ev =
            ipc_input.events[ipc_input.read_idx];

        ipc_input.read_idx =
            (ipc_input.read_idx + 1) % IPC_INPUT_QUEUE_SIZE;

        callback(ev);
    }
}

bool ipc_queue_api_push_event(struct InputsSharedEvent ev, ipc_critical_section_callback critical_section_callback) {
    uint32_t next = (ipc_input.write_idx + 1) % IPC_INPUT_QUEUE_SIZE;

    if (next == ipc_input.read_idx) {
        return false; // queue full
    }

    ipc_input.events[ipc_input.write_idx] = ev;

    if (critical_section_callback != NULL) {
        critical_section_callback();
    }

    ipc_input.write_idx = next;

    return true;
}
