/*!
 * \file can-communications-api.h
 * \date 2026-06-12
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 * \ingroup CM4_Core
 *
 * \brief Public API for the steering-wheel CAN-bus communication module.
 *
 * \details The module owns one PAL handler per network. The user provides
 *     three callbacks at init time:
 *       - a HAL_FDCAN_AddMessageToTxFifoQ wrapper (per network);
 *       - an enter-critical-section primitive (shared across networks);
 *       - an exit-critical-section primitive (shared across networks).
 *     The critical-section callbacks come from the application's POST
 *     layer (same pattern leds_transmit and parameters_on_change follow);
 *     pass NULL on both if the consumer can guarantee single-context
 *     access — but the FDCAN ISR push / main-loop pop pattern this module
 *     is designed for needs them.
 *
 *     Lifecycle (per network):
 *       1. can_communications_api_init(network, send, cs_enter, cs_exit).
 *       2. From the FDCAN RX ISR, build a CanCommunicationFrame from the
 *          HAL header + payload and push it with
 *          can_communications_api_add_to_rx_buffer.
 *       3. From the main loop, call
 *          can_communications_api_process_rx repeatedly until it returns
 *          CAN_COMMUNICATION_RC_QUEUE_EMPTY; deserialise each popped frame
 *          with the matching libcan can_*_api_deserialize_from_id and
 *          dispatch.
 *       4. To send a frame, serialise with the matching libcan
 *          can_*_api_serialize_from_id, fill in a CanCommunicationFrame,
 *          and queue it with can_communications_api_send. Call
 *          can_communications_api_process_tx from the main loop to drain
 *          the queue onto the wire.
 */

#ifndef CAN_COMMUNICATIONS_API_H
#define CAN_COMMUNICATIONS_API_H

#include "can-communications.h"

/*!
 * \brief Initialize one CAN network.
 *
 * \details Stands up the PAL handler, the per-network ring buffers and the
 *     arena allocator the first time it is called. Subsequent calls (for
 *     different networks) reuse the existing arena. Calling init twice on
 *     the same network is rejected with
 *     CAN_COMMUNICATION_RC_INVALID_NETWORK so the user does not silently
 *     overwrite the active send callback.
 *
 * \param[in] network  The CAN bus to initialize.
 * \param[in] send     Callback that wraps HAL_FDCAN_AddMessageToTxFifoQ for
 *     \p network. Must be non-NULL.
 * \param[in] cs_enter Enter-critical-section callback. NULL disables CS
 *     protection on the ring buffers (only safe in single-context tests).
 * \param[in] cs_exit  Exit-critical-section callback. NULL disables CS
 *     protection on the ring buffers (only safe in single-context tests).
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p send is NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of
 *     range or already initialized.
 * \retval CAN_COMMUNICATION_RC_ERROR if PAL or the arena fail to set up.
 */
enum CanCommunicationReturnCode can_communications_api_init(
    enum CanCommunicationNetwork network,
    can_communications_send_callback send,
    can_communications_critical_section_callback cs_enter,
    can_communications_critical_section_callback cs_exit);

/*!
 * \brief Queue a frame for transmission on the given network.
 *
 * \details The frame is copied into the per-network TX queue and does not
 *     hit the wire until can_communications_api_process_tx runs.
 *
 * \param[in] network The CAN bus the frame is destined for.
 * \param[in] frame   The frame to send.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if \p frame->length exceeds
 *     CAN_COMMUNICATIONS_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_NOT_INITIALIZED if \p network has not been
 *     initialized yet.
 * \retval CAN_COMMUNICATION_RC_QUEUE_FULL if the TX queue is saturated.
 * \retval CAN_COMMUNICATION_RC_ERROR on a PAL-internal failure.
 */
enum CanCommunicationReturnCode can_communications_api_send(
    enum CanCommunicationNetwork network,
    const struct CanCommunicationFrame *frame);

/*!
 * \brief Push a frame into the RX queue.
 *
 * \details Intended to be called from the FDCAN RX ISR with whatever the
 *     HAL hands over; the frame is copied so the caller may reuse the
 *     source buffer immediately. The queue is drained by
 *     can_communications_api_process_rx in the main loop — this call does
 *     no deserialisation of its own, which keeps the ISR path short.
 *
 * \param[in] network The CAN bus the frame arrived on.
 * \param[in] frame   The freshly-received frame.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame is NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_INVALID_LENGTH if \p frame->length exceeds
 *     CAN_COMMUNICATIONS_FRAME_DATA_SIZE.
 * \retval CAN_COMMUNICATION_RC_NOT_INITIALIZED if \p network has not been
 *     initialized yet.
 * \retval CAN_COMMUNICATION_RC_QUEUE_FULL if the RX queue is saturated.
 * \retval CAN_COMMUNICATION_RC_ERROR on a PAL-internal failure.
 */
enum CanCommunicationReturnCode can_communications_api_add_to_rx_buffer(
    enum CanCommunicationNetwork network,
    const struct CanCommunicationFrame *frame);

/*!
 * \brief Drain one TX-queued frame and send it via the user callback.
 *
 * \details Intended to be called from the main loop, typically inside a
 *     while-loop that runs until CAN_COMMUNICATION_RC_QUEUE_EMPTY is
 *     returned. Each call pops at most one frame, so the caller controls
 *     how much CPU budget the TX pump gets per iteration.
 *
 * \param[in] network The CAN bus whose TX queue to drain.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_NOT_INITIALIZED if \p network has not been
 *     initialized yet.
 * \retval CAN_COMMUNICATION_RC_QUEUE_EMPTY if there was nothing to send.
 * \retval CAN_COMMUNICATION_RC_TRANSMISSION_ERROR if the send callback
 *     reported failure.
 * \retval CAN_COMMUNICATION_RC_ERROR on a PAL-internal failure.
 */
enum CanCommunicationReturnCode can_communications_api_process_tx(
    enum CanCommunicationNetwork network);

/*!
 * \brief Pop one frame off the RX queue.
 *
 * \details Intended to be called from the main loop in a loop until
 *     CAN_COMMUNICATION_RC_QUEUE_EMPTY is returned. After a successful
 *     call \p frame_out contains the freshly-received frame and the
 *     application should run the matching libcan deserialiser on it.
 *
 * \param[in]  network   The CAN bus whose RX queue to drain.
 * \param[out] frame_out Destination of the popped frame.
 *
 * \retval CAN_COMMUNICATION_RC_OK on success; \p frame_out is populated.
 * \retval CAN_COMMUNICATION_RC_NULL_POINTER if \p frame_out is NULL.
 * \retval CAN_COMMUNICATION_RC_INVALID_NETWORK if \p network is out of range.
 * \retval CAN_COMMUNICATION_RC_NOT_INITIALIZED if \p network has not been
 *     initialized yet.
 * \retval CAN_COMMUNICATION_RC_QUEUE_EMPTY if there was nothing to dispatch.
 * \retval CAN_COMMUNICATION_RC_ERROR on a PAL-internal failure.
 */
enum CanCommunicationReturnCode can_communications_api_process_rx(
    enum CanCommunicationNetwork network,
    struct CanCommunicationFrame *frame_out);

#endif // CAN_COMMUNICATIONS_API_H
