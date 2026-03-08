#include "ipc-api.h"

void ipc_api_read_and_process_all(struct IPCInputQueue *queue, void (*callback)(struct InputEvent *ev)) {
    while (queue->read_idx != queue->write_idx) {
        struct InputEvent ev =
            queue->events[queue->read_idx];

        queue->read_idx =
            (queue->read_idx + 1) % IPC_INPUT_QUEUE_SIZE;

        callback(&ev);
    }
}

bool ipc_api_push_event(struct IPCInputQueue *queue, struct InputEvent *ev, void (*dmb_callback)(void)) {
    uint32_t next = (queue->write_idx + 1) % IPC_INPUT_QUEUE_SIZE;

    if (next == queue->read_idx) {
        return false; // queue full
    }

    queue->events[queue->write_idx] = *ev;

    dmb_callback();

    queue->write_idx = next;

    return true;
}
