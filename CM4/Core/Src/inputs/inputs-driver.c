#include "inputs-driver.h"

struct InputEvent shared_data
    __attribute__((section(".shared_axi"), aligned(32)));

struct InputHandler input_handler = { 0 };

enum InputsReturnCode mock_notify_callback(struct InputEvent event) {
    // Mock implementation: simply print the event details
    // In a real scenario, this would notify the CM7 core about the event
    return INPUTS_RC_OK;
}
