/*!
 * \file test_ipc.c
 * \date 2026-03-14
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Test IPC (Inter-Process Communication) functionality.
 */

#include "inputs-shared.h"
#include "ipc-api.h"
#include "ipc.h"
#include "shared-ipc.h"
#include "unity.h"

int called_count = 0;
static struct InputsSharedEvent last_event;

void setUp() {
    called_count = 0;
    last_event = (struct InputsSharedEvent){ 0 };
    ipc_input.read_idx = 0;
    ipc_input.write_idx = 0;
}

void read_and_process_callback(struct InputsSharedEvent *ev) {
    last_event = *ev;
    called_count++;
}

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * @{
 */

void test_ipc_input_queue_read_and_process_all_empty(void) {
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(0, called_count);
}

void test_ipc_input_queue_read_and_process_all_non_empty(void) {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(1, called_count);
}

void test_ipc_input_queue_read_and_process_all_multiple(void) {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(&ev, NULL);
    ipc_api_push_event(&ev, NULL);
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(3, called_count);
}

void test_ipc_input_queue_read_and_process_read_event(void) {
    struct InputsSharedEvent ev = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
        .button = { .button_id = INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT }
    };
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(last_event.type, ev.type, "Event type mismatch");
    TEST_ASSERT_EQUAL_INT_MESSAGE(last_event.button.button_id, ev.button.button_id, "Button ID mismatch");
}

/*! @} */

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * @{
 */

void test_ipc_input_queue_push_event_null(void) {
    struct InputsSharedEvent *ev = NULL;
    bool rc = ipc_api_push_event(ev, NULL);
    TEST_ASSERT_EQUAL(false, rc);
}

void test_ipc_input_queue_push_event_success(void) {
    struct InputsSharedEvent ev = { 0 };
    bool rc = ipc_api_push_event(&ev, NULL);
    TEST_ASSERT_EQUAL(true, rc);
}

void test_ipc_input_queue_push_event_full(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < IPC_INPUT_QUEUE_SIZE - 1; i++) {
        bool rc = ipc_api_push_event(&ev, NULL);
        TEST_ASSERT_EQUAL_MESSAGE(true, rc, "Failed to push event to queue");
    }
    bool rc = ipc_api_push_event(&ev, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(false, rc, "Expected queue to be full, but push succeeded");
}

/*! @} */

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * @{
 */

void test_ipc_input_queue_producer_consumer(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < IPC_INPUT_QUEUE_SIZE - 1; i++) {
        bool rc = ipc_api_push_event(&ev, NULL);
        TEST_ASSERT_EQUAL_MESSAGE(true, rc, "Failed to push event to queue");
    }
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(IPC_INPUT_QUEUE_SIZE - 1, called_count, "Expected to process all events in the queue");
}

void test_ipc_input_queue_producer_consumer_wrap(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < 100; i++) {
        bool rc = ipc_api_push_event(&ev, NULL);
        TEST_ASSERT_EQUAL(true, rc);
        if ((i + 1) % 10 == 0) {
            ipc_api_read_and_process_all(read_and_process_callback);
        }
    }
}

/*! @} */


int main() {
    UNITY_BEGIN();
        
    RUN_TEST(test_ipc_input_queue_read_and_process_all_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_non_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_multiple);
    RUN_TEST(test_ipc_input_queue_read_and_process_read_event);

    RUN_TEST(test_ipc_input_queue_push_event_null);
    RUN_TEST(test_ipc_input_queue_push_event_success);
    RUN_TEST(test_ipc_input_queue_push_event_full);

    RUN_TEST(test_ipc_input_queue_producer_consumer);
    RUN_TEST(test_ipc_input_queue_producer_consumer_wrap);

    return UNITY_END();
}
