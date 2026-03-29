/*!
 * \file ipc-api.c
 * \date 2026-01-20
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup Shared
 *
 * \brief Implementation of the IPC API for managing input events between CM4 and CM7.
 */

#include "ipc-api.h"
#include "shared-ipc.h"
#include <stdio.h>

void ipc_api_reset(void) {
    ipc_input = (struct IPCInputQueue){ 0 };
}

void ipc_api_read_and_process_all(void (*callback)(struct InputsSharedEvent ev)) {
    while (ipc_input.read_idx != ipc_input.write_idx) {
        struct InputsSharedEvent ev =
            ipc_input.events[ipc_input.read_idx];

        ipc_input.read_idx =
            (ipc_input.read_idx + 1) % IPC_INPUT_QUEUE_SIZE;

        callback(ev);
    }
}

bool ipc_api_push_event(struct InputsSharedEvent ev, void (*critical_section_callback)(void)) {
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
