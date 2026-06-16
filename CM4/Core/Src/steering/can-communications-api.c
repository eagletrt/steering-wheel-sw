/*!
 * \file can-communications-api.c
 * \date 2026-06-12
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Implementation of the CAN-bus communication module.
 *
 * \details One file-static handler owns a PAL handler per network plus a
 *     shared arena. The raw_frame format used between the API and PAL's byte
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
 * \brief Byte offset, inside the raw_frame buffer, where the encoded length byte sits.
 */
#define CAN_COMMUNICATIONS_RAW_FRAME_LENGTH_OFFSET (sizeof(uint32_t))

/*!
 * \brief Byte offset, inside the raw_frame buffer, where the payload bytes start.
 */
#define CAN_COMMUNICATIONS_RAW_FRAME_DATA_OFFSET (CAN_COMMUNICATIONS_RAW_FRAME_LENGTH_OFFSET + sizeof(uint8_t))

/*!
 * \brief RawFrame-format size pushed through PAL.
 *
 * \details Each queued frame is encoded as
 *     [id (4 bytes, little-endian)][length (1 byte)][data (8 bytes)]
 *     so PAL can treat the queue as a flat byte stream regardless of the
 *     CAN frame shape. Bytes past \c length are still copied through
 *     untouched (they end up undefined on the consumer side, which is what
 *     CanCommunicationFrame already documents).
 */
#define CAN_COMMUNICATIONS_RAW_FRAME_SIZE (CAN_COMMUNICATIONS_RAW_FRAME_DATA_OFFSET + CAN_COMMUNICATIONS_FRAME_DATA_SIZE)

EAGLETRT_STATIC struct CanCommunicationsHandler handler;

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
 * \brief Encode a CanCommunicationFrame into the PAL raw_frame buffer.
 *
 * \param[in]  frame Source frame.
 * \param[out] raw_frame  Destination buffer, at least CAN_COMMUNICATIONS_RAW_FRAME_SIZE bytes long.
 */
EAGLETRT_STATIC void prv_encode_frame(const struct CanCommunicationFrame *frame, uint8_t *raw_frame) {
    raw_frame[0U] = (uint8_t)(frame->id & 0xFFU);
    raw_frame[1U] = (uint8_t)((frame->id >> 8U) & 0xFFU);
    raw_frame[2U] = (uint8_t)((frame->id >> 16U) & 0xFFU);
    raw_frame[3U] = (uint8_t)((frame->id >> 24U) & 0xFFU);
    raw_frame[CAN_COMMUNICATIONS_RAW_FRAME_LENGTH_OFFSET] = frame->length;
    (void)memcpy(&raw_frame[CAN_COMMUNICATIONS_RAW_FRAME_DATA_OFFSET], frame->data, CAN_COMMUNICATIONS_FRAME_DATA_SIZE);
}

/*!
 * \brief Decode a raw_frame buffer back into a CanCommunicationFrame.
 *
 * \details Mirrors prv_encode_frame and is used both by the PAL send shim
 *     (TX path) and the PAL deserialiser (RX path).
 *
 * \param[in]  raw_frame  Source buffer, at least CAN_COMMUNICATIONS_RAW_FRAME_SIZE bytes long.
 * \param[out] frame Destination frame.
 */
EAGLETRT_STATIC void prv_decode_frame(const uint8_t *raw_frame, struct CanCommunicationFrame *frame) {
    frame->id = (uint32_t)raw_frame[0U] |
                ((uint32_t)raw_frame[1U] << 8U) |
                ((uint32_t)raw_frame[2U] << 16U) |
                ((uint32_t)raw_frame[3U] << 24U);
    frame->length = raw_frame[CAN_COMMUNICATIONS_RAW_FRAME_LENGTH_OFFSET];
    if (frame->length > CAN_COMMUNICATIONS_FRAME_DATA_SIZE) {
        frame->length = CAN_COMMUNICATIONS_FRAME_DATA_SIZE;
    }
    (void)memcpy(frame->data, &raw_frame[CAN_COMMUNICATIONS_RAW_FRAME_DATA_OFFSET], CAN_COMMUNICATIONS_FRAME_DATA_SIZE);
}

/*!
 * \brief Shared body of the per-network PAL send shims.
 *
 * \details PAL's pal_send_callback type has no user-data slot, so the
 *     module installs a one-per-network static shim that thunks into this
 *     helper with the right network ID.
 *
 * \param[in] network Network ID of the slot whose user send to dispatch to.
 * \param[in] msg     PAL message carrying the raw_frame-encoded frame.
 *
 * \retval PAL_RC_OK on success.
 * \retval PAL_RC_NULL_POINTER if \p msg or the user send callback is NULL.
 * \retval PAL_RC_INVALID_ARGUMENT if the raw_frame size is wrong.
 * \retval PAL_RC_IO_ERROR if the user send callback reported failure.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_dispatch(enum CanCommunicationNetwork network, const struct PalMessage *msg) {
    if (msg == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (msg->size != CAN_COMMUNICATIONS_RAW_FRAME_SIZE) {
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
 * \param[in] msg PAL message carrying the raw_frame-encoded frame.
 *
 * \return Whatever prv_pal_send_dispatch returns.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_send_primary(const struct PalMessage *msg) {
    return prv_pal_send_dispatch(CAN_COMMUNICATION_NETWORK_PRIMARY, msg);
}

/*!
 * \brief PAL send shim for the secondary network.
 *
 * \param[in] msg PAL message carrying the raw_frame-encoded frame.
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
 * \details Converts a raw_frame buffer back into a CanCommunicationFrame.
 *     Shared across networks because the raw_frame format is network-agnostic.
 *
 * \param[in]  message     PAL message carrying the raw_frame-encoded frame.
 * \param[out] frame_out   Pointer to a CanCommunicationFrame to populate.
 *
 * \retval PAL_RC_OK on success.
 * \retval PAL_RC_NULL_POINTER if \p message or \p frame_out is NULL.
 * \retval PAL_RC_DESERIALIZATION_ERROR if the raw_frame size is wrong.
 */
EAGLETRT_STATIC enum PalReturnCode prv_pal_deserialize(const struct PalMessage *message, void *frame_out) {
    if (message == NULL || frame_out == NULL) {
        return PAL_RC_NULL_POINTER;
    }
    if (message->size != CAN_COMMUNICATIONS_RAW_FRAME_SIZE) {
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

enum CanCommunicationReturnCode can_communications_api_init(enum CanCommunicationNetwork network, const struct CanCommunicationsNetworkConfig *config) {
    if (config == NULL || config->send == NULL || config->on_receive == NULL) {
        return CAN_COMMUNICATION_RC_NULL_POINTER;
    }
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    memset(&handler.networks[network], 0, sizeof(handler.networks[network]));

    prv_ensure_arena_ready();

    handler.networks[network].send = config->send;
    handler.networks[network].on_receive = config->on_receive;

    if (pal_api_init(
            &handler.networks[network].pal,
            CAN_COMMUNICATIONS_RX_QUEUE_CAPACITY,
            CAN_COMMUNICATIONS_TX_QUEUE_CAPACITY,
            CAN_COMMUNICATIONS_RAW_FRAME_SIZE,
            prv_pal_deserialize,
            prv_pal_send_shims[network],
            config->cs_enter,
            config->cs_exit,
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

    uint8_t raw_frame[CAN_COMMUNICATIONS_RAW_FRAME_SIZE];
    prv_encode_frame(frame, raw_frame);

    const enum PalReturnCode return_code = to_tx
                                               ? pal_api_add_to_tx_queue(&handler.networks[network].pal, raw_frame, CAN_COMMUNICATIONS_RAW_FRAME_SIZE)
                                               : pal_api_add_to_rx_queue(&handler.networks[network].pal, raw_frame, CAN_COMMUNICATIONS_RAW_FRAME_SIZE);

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

enum CanCommunicationReturnCode can_communications_api_add_to_tx_buffer(enum CanCommunicationNetwork network, const struct CanCommunicationFrame *frame) {
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

    enum CanCommunicationReturnCode result = CAN_COMMUNICATION_RC_OK;
    while (true) {
        const enum PalReturnCode return_code = pal_api_process_tx(&handler.networks[network].pal);
        if (return_code == PAL_RC_QUEUE_EMPTY) {
            break;
        }
        if (return_code == PAL_RC_IO_ERROR) {
            /* User send callback reported failure on this frame; keep draining. */
            result = CAN_COMMUNICATION_RC_TRANSMISSION_ERROR;
            continue;
        }
        if (return_code != PAL_RC_OK) {
            return CAN_COMMUNICATION_RC_ERROR;
        }
    }
    return result;
}

enum CanCommunicationReturnCode can_communications_api_process_rx(enum CanCommunicationNetwork network) {
    if (!prv_network_is_valid(network)) {
        return CAN_COMMUNICATION_RC_INVALID_NETWORK;
    }
    if (!handler.networks[network].initialized) {
        return CAN_COMMUNICATION_RC_NOT_INITIALIZED;
    }

    const can_communications_receive_callback dispatcher = handler.networks[network].on_receive;
    enum CanCommunicationReturnCode result = CAN_COMMUNICATION_RC_OK;
    while (true) {
        struct CanCommunicationFrame frame;
        const enum PalReturnCode return_code = pal_api_process_rx(&handler.networks[network].pal, &frame);
        if (return_code == PAL_RC_QUEUE_EMPTY) {
            break;
        }
        if (return_code != PAL_RC_OK) {
            return CAN_COMMUNICATION_RC_ERROR;
        }
        if (dispatcher(&frame) != CAN_COMMUNICATION_RC_OK) {
            /* Surface the failure but keep draining the queue. */
            result = CAN_COMMUNICATION_RC_RECEIVE_HANDLER_ERROR;
        }
    }
    return result;
}
