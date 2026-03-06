#ifndef INPUTS_SHARED_API_H
#define INPUTS_SHARED_API_H

#include "inputs-shared.h"

void inputs_shared_api_read_and_process_all(struct IPCInputQueue *queue, void (*callback)(struct InputEvent *ev));
void inputs_shared_api_push_event(struct IPCInputQueue *queue, struct InputEvent *ev);

#endif // INPUTS_SHARED_API_H
