/*!
 * \file can-communications-api.c
 * \date 2026-06-12
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Implementation of the CAN-bus communication module.
 *
 * \details One file-static handler owns a PAL handler per network plus a
 *     shared arena. The wire format used between the API and PAL's byte
 *     queues stays inside this translation unit. libcan never appears
 *     here: serialisation/deserialisation lives in the application layer
 *     that calls the public API.
 */

#include "can-communications-api.h"
#include "arena-allocator-api.h"
#include "pal-api.h"
#include "eagletrt.h"

#include <string.h>

/*!
 * \brief Wire-format size pushed through PAL.
 *
 * \details Each queued frame is encoded as
 *     [id (4 bytes, little-endian)][length (1 byte)][data (8 bytes)]
 *     so PAL can treat the queue as a flat byte stream regardless of the
 *     CAN frame shape. Bytes past \c length are still copied through
 *     untouched (they end up undefined on the consumer side, which is what
 *     CanCommunicationFrame already documents).
 */
/*!
 * \brief Byte offset, inside the wire buffer, where the encoded frame id begins.
 */
#define CAN_COMMUNICATIONS_WIRE_ID_OFFSET (0U)

/*!
 * \brief Byte offset, inside the wire buffer, where the encoded length byte sits.
 */
#define CAN_COMMUNICATIONS_WIRE_LENGTH_OFFSET ((uint32_t)sizeof(uint32_t))

/*!
 * \brief Byte offset, inside the wire buffer, where the payload bytes start.
 */
#define CAN_COMMUNICATIONS_WIRE_DATA_OFFSET (CAN_COMMUNICATIONS_WIRE_LENGTH_OFFSET + (uint32_t)sizeof(uint8_t))

#define CAN_COMMUNICATIONS_WIRE_SIZE (CAN_COMMUNICATIONS_WIRE_DATA_OFFSET + CAN_COMMUNICATIONS_FRAME_DATA_SIZE)

EAGLETRT_STATIC struct CanCommunicationsHandler handler;

/* -------------------------------------------------------------------------- */
/* helpers                                                                    */
/* -------------------------------------------------------------------------- */

/*!
 * \brief Whether \p network is a valid index into the per-network arrays.
 *
 * \param[in] network Network ID to check.
 *
 * \retval true  if \p network is in range.
 * \retval false otherwise.
 */
EAGLETRT_STATIC bool prv_network_is_valid(enum CanCommunicationNetwork network) {
    return network < CAN_COMMUNICATION_NETWORK_COUNT;
}

/*!
 * \brief Encode a CanCommunicationFrame into the PAL wire buffer.
 *
 * \param[in]  frame Source frame.
 * \param[out] wire  Destination buffer, at least CAN_COMMUNICATIONS_WIRE_SIZE bytes long.
 */
EAGLETRT_STATIC void prv_encode_frame(const struct CanCommunicationFrame *frame, uint8_t *wire) {
    wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 0U] = (uint8_t)(frame->id & 0xFFU);
    wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 1U] = (uint8_t)((frame->id >> 8U) & 0xFFU);
    wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 2U] = (uint8_t)((frame->id >> 16U) & 0xFFU);
    wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 3U] = (uint8_t)((frame->id >> 24U) & 0xFFU);
    wire[CAN_COMMUNICATIONS_WIRE_LENGTH_OFFSET] = frame->length;
    (void)memcpy(&wire[CAN_COMMUNICATIONS_WIRE_DATA_OFFSET], frame->data, CAN_COMMUNICATIONS_FRAME_DATA_SIZE);
}

/*!
 * \brief Decode a wire buffer back into a CanCommunicationFrame.
 *
 * \details Mirrors prv_encode_frame and is used both by the PAL send shim
 *     (TX path) and the PAL deserialiser (RX path).
 *
 * \param[in]  wire  Source buffer, at least CAN_COMMUNICATIONS_WIRE_SIZE bytes long.
 * \param[out] frame Destination frame.
 */
EAGLETRT_STATIC void prv_decode_frame(const uint8_t *wire, struct CanCommunicationFrame *frame) {
    frame->id = (uint32_t)wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 0U] |
                ((uint32_t)wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 1U] << 8U) |
                ((uint32_t)wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 2U] << 16U) |
                ((uint32_t)wire[CAN_COMMUNICATIONS_WIRE_ID_OFFSET + 3U] << 24U);
    frame->length = wire[CAN_COMMUNICATIONS_WIRE_LENGTH_OFFSET];
    if (frame->length > CAN_COMMUNICATIONS_FRAME_DATA_SIZE) {
        frame->length = CAN_COMMUNICATIONS_FRAME_DATA_SIZE;
    }
    (void)memcpy(frame->data, &wire[CAN_COMMUNICATIONS_WIRE_DATA_OFFSET], CAN_COMMUNICATIONS_FRAME_DATA_SIZE);
}

/* -------------------------------------------------------------------------- */
/* PAL glue                                                                   */
/* -------------------------------------------------------------------------- */

/*!
 * \brief Shared body of the per-network PAL send shims.
 *
 * \details PAL's pal_send_callback type has no user-data slot, so the
 *     module installs a one-per-network static shim that thunks into this
 *     helper with the right network ID.
 *
 * \param[in] network Network ID of the slot whose user send to dispatch to.
 * \param[in] msg     PAL message carrying the wire-encoded frame.
 *
 * \retval PAL_RC_OK on success.
 * \retval PAL_RC_NULL_POINTER if \p msg or the user send callback is NULL.
 * \retval PAL_RC_INVALID_ARGUMENT if the wire size is wrong.
 * \retval PAL_RC_IO_ERROR if the user send callback reported failure.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_dispatch(enum CanCommunicationNetwork network, const struct PalMessage *msg) {
    if (msg == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (msg->size != CAN_COMMUNICATIONS_WIRE_SIZE) {
        return PAL_RC_INVALID_ARGUMENT;
    }

    struct CanCommunicationFrame frame;
    prv_decode_frame(msg->payload, &frame);

    const can_communications_send_callback user_send = handler.networks[network].send;
    if (user_send == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (user_send(&frame) != CAN_COMMUNICATION_RC_OK) {
        return PAL_RC_IO_ERROR;
    }
    return PAL_RC_OK;
}

/*!
 * \brief PAL send shim for the primary network.
 *
 * \param[in] msg PAL message carrying the wire-encoded frame.
 *
 * \return Whatever prv_pal_send_dispatch returns.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_primary(const struct PalMessage *msg) {
    return prv_pal_send_dispatch(CAN_COMMUNICATION_NETWORK_PRIMARY, msg);
}

/*!
 * \brief PAL send shim for the secondary network.
 *
 * \param[in] msg PAL message carrying the wire-encoded frame.
 *
 * \return Whatever prv_pal_send_dispatch returns.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_secondary(const struct PalMessage *msg) {
    return prv_pal_send_dispatch(CAN_COMMUNICATION_NETWORK_SECONDARY, msg);
}

/*!
 * \brief Lookup table for the per-network PAL send shims.
 *
 * \details Extending the module to a new network is "add an enum entry +
 *     add the matching shim + add it here". The size of the array is
 *     pinned to CAN_COMMUNICATION_NETWORK_COUNT, so the compiler flags any
 *     mismatch.
 */
EAGLETRT_STATIC const pal_send_callback prv_pal_send_shims[CAN_COMMUNICATION_NETWORK_COUNT] = {
    [CAN_COMMUNICATION_NETWORK_PRIMARY] = prv_pal_send_primary,
    [CAN_COMMUNICATION_NETWORK_SECONDARY] = prv_pal_send_secondary,
};

/*!
 * \brief PAL deserialiser used on the RX path.
 *
 * \details Converts a wire buffer back into a CanCommunicationFrame.
 *     Shared across networks because the wire format is network-agnostic.
 *
 * \param[in]  message     PAL message carrying the wire-encoded frame.
 * \param[out] frame_out   Pointer to a CanCommunicationFrame to populate.
 *
 * \retval PAL_RC_OK on success.
 * \retval PAL_RC_NULL_POINTER if \p message or \p frame_out is NULL.
 * \retval PAL_RC_DESERIALIZATION_ERROR if the wire size is wrong.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_deserialize(const struct PalMessage *message, void *frame_out) {
    if (message == NULL || frame_out == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (message->size != CAN_COMMUNICATIONS_WIRE_SIZE) {
        return PAL_RC_DESERIALIZATION_ERROR;
    }
    prv_decode_frame(message->payload, (struct CanCommunicationFrame *)frame_out);
    return PAL_RC_OK;
}

/*!
 * \brief Ensure the arena allocator is up before the first PAL init.
 *
 * \details Idempotent: subsequent network inits reuse the existing arena.
 *     Tracked via a static flag rather than a sentinel inside the arena
 *     struct so we don't depend on arena-allocator's internals.
 */
EAGLETRT_STATIC void prv_ensure_arena_ready(void) {
    EAGLETRT_STATIC bool arena_initialized = false;
    if (!arena_initialized) {
        arena_allocator_api_init(&handler.arena);
        arena_initialized = true;
    }
}

/* -------------------------------------------------------------------------- */
/* public API                                                                 */
/* -------------------------------------------------------------------------- */

enum CanCommunicationReturnCode can_communications_api_init(enum CanCommunicationNetwork network, can_communications_send_callback send, can_communications_critical_section_callback cs_enter, can_communications_critical_section_callback cs_exit) {
    if (send == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (handler.networks[network].initialized) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }

    prv_ensure_arena_ready();

    handler.networks[network].send = send;

    if (pal_api_init(
            &handler.networks[network].pal,
            CAN_COMMUNICATIONS_RX_QUEUE_CAPACITY,
            CAN_COMMUNICATIONS_TX_QUEUE_CAPACITY,
            CAN_COMMUNICATIONS_WIRE_SIZE,
            prv_pal_deserialize,
            prv_pal_send_shims[network],
            cs_enter,
            cs_exit,
            &handler.arena) != PAL_RC_OK) {
        return CAN_COMMUNICATION_RC_ERROR;
    }

    handler.networks[network].initialized = true;
    return CAN_COMMUNICATION_RC_OK;
}

/*!
 * \brief Shared body of can_communications_api_send and
 *     can_communications_api_add_to_rx_buffer.
 *
 * \param[in] network Target network.
 * \param[in] frame   Frame to push.
 * \param[in] to_tx   true  → push to the TX queue (send path),
 *                    false → push to the RX queue (ISR path).
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if \p frame->length is too large.
 * \retval CAN_COMMUNICATION_RC_NOT_INITIALIZED if \p network has not been
 *     initialised yet.
 * \retval CAN_COMMUNICATION_RC_QUEUE_FULL if the target queue is saturated.
 * \retval CAN_COMMUNICATION_RC_ERROR on a PAL-internal failure.
 */
EAGLETRT_STATIC enum CanCommunicationReturnCode prv_enqueue(enum CanCommunicationNetwork network, const struct CanCommunicationFrame *frame, bool to_tx) {
    if (frame == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (frame->length > CAN_COMMUNICATIONS_FRAME_DATA_SIZE) {
        return CAN_COMMUNICATION_RC_INVALID_LENGTH;
    }
    if (!handler.networks[network].initialized) {
        return CAN_COMMUNICATION_RC_NOT_INITIALIZED;
    }

    uint8_t wire[CAN_COMMUNICATIONS_WIRE_SIZE];
    prv_encode_frame(frame, wire);

    const enum PalReturnCode return_code = to_tx
                                               ? pal_api_add_to_tx_queue(&handler.networks[network].pal, wire, CAN_COMMUNICATIONS_WIRE_SIZE)
                                               : pal_api_add_to_rx_queue(&handler.networks[network].pal, wire, CAN_COMMUNICATIONS_WIRE_SIZE);

    switch (return_code) {
        case PAL_RC_OK:
            return CAN_COMMUNICATION_RC_OK;
        case PAL_RC_QUEUE_FULL:
            return CAN_COMMUNICATION_RC_QUEUE_FULL;
        case PAL_RC_NULL_POINTER:
            return CAN_COMMUNICATION_RC_NULL_POINTER;
        default:
            return CAN_COMMUNICATION_RC_ERROR;
    }
}

enum CanCommunicationReturnCode can_communications_api_send(enum CanCommunicationNetwork network, const struct CanCommunicationFrame *frame) {
    return prv_enqueue(network, frame, true);
}

enum CanCommunicationReturnCode can_communications_api_add_to_rx_buffer(enum CanCommunicationNetwork network, const struct CanCommunicationFrame *frame) {
    return prv_enqueue(network, frame, false);
}

enum CanCommunicationReturnCode can_communications_api_process_tx(enum CanCommunicationNetwork network) {
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (!handler.networks[network].initialized) {
        return CAN_COMMUNICATION_RC_NOT_INITIALIZED;
    }

    switch (pal_api_process_tx(&handler.networks[network].pal)) {
        case PAL_RC_OK:
            return CAN_COMMUNICATION_RC_OK;
        case PAL_RC_QUEUE_EMPTY:
            return CAN_COMMUNICATION_RC_QUEUE_EMPTY;
        case PAL_RC_IO_ERROR:
            return CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
        default:
            return CAN_COMMUNICATION_RC_ERROR;
    }
}

enum CanCommunicationReturnCode can_communications_api_process_rx(enum CanCommunicationNetwork network, struct CanCommunicationFrame *frame_out) {
    if (frame_out == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (!handler.networks[network].initialized) {
        return CAN_COMMUNICATION_RC_NOT_INITIALIZED;
    }

    switch (pal_api_process_rx(&handler.networks[network].pal, frame_out)) {
        case PAL_RC_OK:
            return CAN_COMMUNICATION_RC_OK;
        case PAL_RC_QUEUE_EMPTY:
            return CAN_COMMUNICATION_RC_QUEUE_EMPTY;
        default:
            return CAN_COMMUNICATION_RC_ERROR;
    }
}
