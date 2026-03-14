/*!
 * \file test_ipc.c
 * \date 2026-03-14
 * \authors Alessandro Bridi [ale.bridi15@gmail.com]
 *
 * \brief Test IPC (Inter-Process Communication) functionality.
 */

#include "inputs-shared.h"
#include "ipc-api.h"
#include "unity.h"

int called_count = 0;
static struct InputsSharedEvent last_event;

void read_and_process_callback(struct InputsSharedEvent *ev) {
    last_event = *ev;
    called_count++;
}

void test_ipc_input_queue_read_and_process_all_empty() {
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(0, called_count);
    called_count = 0;
}

void test_ipc_input_queue_read_and_process_all_non_empty() {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(1, called_count);
    called_count = 0;
}

void test_ipc_input_queue_read_and_process_all_multiple() {
    struct InputsSharedEvent ev = { 0 };
    ipc_api_push_event(&ev, NULL);
    ipc_api_push_event(&ev, NULL);
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT(3, called_count);
    called_count = 0;
}

void test_ipc_input_queue_read_and_process_read_event() {
    struct InputsSharedEvent ev = {
        .type = INPUTS_SHARED_EVENT_TYPE_BUTTON_LONG_PRESS,
        .button = { .button_id = INPUTS_SHARED_BUTTON_ID_BOTTOM_LEFT }
    };
    ipc_api_push_event(&ev, NULL);
    ipc_api_read_and_process_all(read_and_process_callback);
    TEST_ASSERT_EQUAL_INT_MESSAGE(last_event.type, ev.type, "Event type mismatch");
    TEST_ASSERT_EQUAL_INT_MESSAGE(last_event.button.button_id, ev.button.button_id, "Button ID mismatch");
    called_count = 0;
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_ipc_input_queue_read_and_process_all_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_non_empty);
    RUN_TEST(test_ipc_input_queue_read_and_process_all_multiple);
    RUN_TEST(test_ipc_input_queue_read_and_process_read_event);
    return UNITY_END();
}
