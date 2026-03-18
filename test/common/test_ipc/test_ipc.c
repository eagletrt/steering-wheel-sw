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
#include "fff.h"
#include "unity.h"

#include <stdio.h>

DEFINE_FFF_GLOBALS;
FAKE_VOID_FUNC(read_and_process_callback, struct InputsSharedEvent);

void setUp() {
    ipc_api_reset();
    RESET_FAKE(read_and_process_callback);
    FFF_RESET_HISTORY();
}

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * \{
 */

void test_ipc_input_queue_read_and_process_all_empty(void) {
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(0, read_and_process_callback_fake.call_count);
}

void test_ipc_input_queue_read_and_process_all_non_empty(void) {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(1, read_and_process_callback_fake.call_count);
}

void test_ipc_input_queue_read_and_process_all_multiple(void) {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(ev, NULL);
    ipc_api_push_event(ev, NULL);
    ipc_api_push_event(ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(3, read_and_process_callback_fake.call_count);
}

void test_ipc_input_queue_read_and_process_read_event(void) {
    struct InputsSharedEvent ev = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
        .button = { .button_id = INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT }
    };
    ipc_api_push_event(ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    struct InputsSharedEvent last_event = read_and_process_callback_fake.arg0_val;
    printf("Last event type: %d, button ID: %d\n", last_event.type, last_event.button.button_id);
    TEST_ASSERT_EQUAL_INT_MESSAGE(1, read_and_process_callback_fake.call_count, "Expected callback to be called once");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ev.type, last_event.type, "Event type mismatch");
    TEST_ASSERT_EQUAL_INT_MESSAGE(ev.button.button_id, last_event.button.button_id, "Button ID mismatch");
}

/*! \} */

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * \{
 */

void test_ipc_input_queue_push_event_success(void) {
    struct InputsSharedEvent ev = { 0 };
    bool rc = ipc_api_push_event(ev, NULL);
    TEST_ASSERT_EQUAL(true, rc);
}

void test_ipc_input_queue_push_event_full(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < IPC_INPUT_QUEUE_SIZE - 1; i++) {
        bool rc = ipc_api_push_event(ev, NULL);
        TEST_ASSERT_EQUAL_MESSAGE(true, rc, "Failed to push event to queue");
    }
    bool rc = ipc_api_push_event(ev, NULL);
    TEST_ASSERT_EQUAL_MESSAGE(false, rc, "Expected queue to be full, but push succeeded");
}

/*! \} */

/*!
 * \defgroup ipc_api_read_and_process_all Tests for ipc_api_read_and_process_all function
 * \{
 */

void test_ipc_input_queue_write_all_read_all(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < IPC_INPUT_QUEUE_SIZE - 1; i++) {
        bool rc = ipc_api_push_event(ev, NULL);
        TEST_ASSERT_EQUAL_MESSAGE(true, rc, "Failed to push event to queue");
    }
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(IPC_INPUT_QUEUE_SIZE - 1, read_and_process_callback_fake.call_count, "Expected to process all events in the queue");
}

void test_ipc_input_queue_write_100_read_100(void) {
    struct InputsSharedEvent ev = { 0 };
    for (int i = 0; i < 100; i++) {
        bool rc = ipc_api_push_event(ev, NULL);
        TEST_ASSERT_EQUAL(true, rc);
        if ((i + 1) % 10 == 0) {
            ipc_api_read_and_process_all(read_and_process_callback);
        }
    }
    TEST_ASSERT_EQUAL_INT_MESSAGE(100, read_and_process_callback_fake.call_count, "Expected to process all events in the queue across multiple wraps");
}

/*! \} */


int main() {
    UNITY_BEGIN();
        
    RUN_TEST(test_ipc_input_queue_read_and_process_all_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_non_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_multiple);
    RUN_TEST(test_ipc_input_queue_read_and_process_read_event);

    RUN_TEST(test_ipc_input_queue_push_event_success);
    RUN_TEST(test_ipc_input_queue_push_event_full);

    RUN_TEST(test_ipc_input_queue_write_all_read_all);
    RUN_TEST(test_ipc_input_queue_write_100_read_100);

    return UNITY_END();
}
