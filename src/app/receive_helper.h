#pragma once

#include "stdbool.h"

typedef enum {
    RX_RESULT_TIMEOUT = 0,      // Queue receive timed out without receiving a message
    RX_RESULT_OK,           // Message received and processed successfully
    RX_RESULT_ERROR         // An error occurred during message processing (e.g., decryption failed, bad size, etc.)
} rx_result_t;

rx_result_t receive_data();
bool receive_test_data();