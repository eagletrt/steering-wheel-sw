#include "inputs-shared-api.h"

void inputs_shared_api_read_and_process_all(struct IPCInputQueue *queue, void (*callback)(struct InputEvent *ev)) {
    while (queue->read_idx != queue->write_idx) {
        struct InputEvent ev =
            queue->events[queue->read_idx];

        queue->read_idx =
            (queue->read_idx + 1) % IPC_INPUT_QUEUE_SIZE;

        callback(&ev);
    }
}
