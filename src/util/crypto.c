#include "crypto.h"

#include "esp_err.h"
#include "string.h"
#include "aes/esp_aes.h"

static const unsigned char aes_key[CRYPTO_KEY_SIZE] = {0x1C, 0x21, 0x02, 0x03, 
                                    0x14, 0xA5, 0x1E, 0x07,
                                    0x00, 0x09, 0x0A, 0x1B,
                                    0x17, 0xBD, 0x3E, 0x9F};

// 12-Bytes fixed matching for pi zero w
static const uint8_t s_base_nonce[12] = {0x11, 0x21, 0x3E, 22, 
                                    0x14, 0xA5, 26, 0x07,
                                    36, 0xF9, 0x0A, 0xBB};

static esp_aes_context s_aes_ctx;

void crypto_init() {
    esp_aes_init(&s_aes_ctx);                                   // Initialize the AES context
    esp_aes_setkey(&s_aes_ctx, aes_key, 128);      // Key size in bits
}

esp_err_t encrypt_data(const uint8_t *input, size_t input_len, uint8_t *output, uint32_t counter) {

    if (input == NULL || output == NULL || input_len == 0) {
        return ESP_ERR_INVALID_ARG; // Input length must be a multiple of 16 bytes
    }

    uint8_t nonce_counter[CRYPTO_NONCE_SIZE];
    memcpy(nonce_counter, s_base_nonce, sizeof(s_base_nonce));

    nonce_counter[12] = (uint8_t)(counter >> 24) & 0xFF;
    nonce_counter[13] = (uint8_t)(counter >> 16) & 0xFF;
    nonce_counter[14] = (uint8_t)(counter >> 8) & 0xFF;
    nonce_counter[15] = (uint8_t)(counter & 0xFF);

    size_t nc_off = 0;
    uint8_t stream_block[CRYPTO_NONCE_SIZE] = {0};

    int ret = esp_aes_crypt_ctr(&s_aes_ctx, input_len, &nc_off, nonce_counter, stream_block, input, output);
    if (ret != 0) {
        return ESP_FAIL; // Encryption failed
    }

    return ESP_OK;
}