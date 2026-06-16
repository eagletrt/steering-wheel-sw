#include "can-communications-router.h"

enum CanCommunicationReturnCode can_communications_router_receive_primary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    // TODO: add libcan deserialization and dispatch logic here

    return CAN_COMMUNICATION_RC_OK;
}

enum CanCommunicationReturnCode can_communications_router_receive_secondary(const struct CanCommunicationFrame *frame) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }

    // TODO: add libcan deserialization and dispatch logic here

    return CAN_COMMUNICATION_RC_OK;
}
