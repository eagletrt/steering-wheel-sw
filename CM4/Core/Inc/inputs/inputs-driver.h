#ifndef INPUTS_DRIVER_H
#define INPUTS_DRIVER_H

#include "shared.h"
#include "inputs.h"

extern struct InputEvent shared_data;
extern struct InputHandler input_handler;

enum InputsReturnCode mock_notify_callback(struct InputEvent event);

#endif // INPUTS_DRIVER_H
