#pragma once

#include "esp_err.h"
#include "stdint.h"

#define CRYPTO_KEY_SIZE 16 // AES-128 key size in bytes
#define CRYPTO_NONCE_SIZE 16 // AES block size in bytes
#define CRYPTO_COUNTER_SIZE 4 // Size of the counter in bytes

void crypto_init();
esp_err_t encrypt_data(const uint8_t *input, size_t input_len, uint8_t *output, uint32_t counter);