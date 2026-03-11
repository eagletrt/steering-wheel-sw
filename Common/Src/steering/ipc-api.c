#include "ipc-api.h"
#include "shared-ipc.h"

void ipc_api_read_and_process_all(void (*callback)(struct InputsSharedEvent *ev)) {
    while (ipc_input.read_idx != ipc_input.write_idx) {
        struct InputsSharedEvent ev =
            ipc_input.events[ipc_input.read_idx];

        ipc_input.read_idx =
            (ipc_input.read_idx + 1) % IPC_INPUT_QUEUE_SIZE;

        callback(&ev);
    }
}

bool ipc_api_push_event(struct InputsSharedEvent *ev, void (*dmb_callback)(void)) {
    uint32_t next = (ipc_input.write_idx + 1) % IPC_INPUT_QUEUE_SIZE;

    if (next == ipc_input.read_idx) {
        return false; // queue full
    }

    ipc_input.events[ipc_input.write_idx] = *ev;

    dmb_callback();

    ipc_input.write_idx = next;

    return true;
}
