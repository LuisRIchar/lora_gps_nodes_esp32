#include "receive_helper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "string.h"
#include "esp_random.h"

#include "lora_spi.h"
#include "io.h"     
#include "crypto.h" 
#include <stdbool.h>

static lora_msg_t msg = {0};

rx_result_t receive_data() {
    // Randomize timeout between 4000 and 6000 ms to avoid synchronization
    uint32_t timeout_ms = (uint32_t)(4000 + (esp_random() % 2000));

    // Wait for a message to be received
    if (xQueueReceive(lora_get_queue(), &msg, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
        // Process the received data
        if (msg.length <= CRYPTO_COUNTER_SIZE) {
            printf("Packet too short to contain counter and data: %u\n", (unsigned int)msg.length);
            return RX_RESULT_ERROR; // Message too short to contain counter and data
        }
            uint32_t rx_counter = ((uint32_t)msg.data[0] << 24) |
                                  ((uint32_t)msg.data[1] << 16) |
                                  ((uint32_t)msg.data[2] << 8) |
                                  ((uint32_t)msg.data[3]);

            uint8_t decrypted[256];
            size_t decrypted_len = (size_t)(msg.length - CRYPTO_COUNTER_SIZE);

            if (decrypted_len >= sizeof(decrypted)) {
                printf("Decrypted buffer too small for received data\n");
                return RX_RESULT_ERROR; // Decrypted buffer too small for received data
            }

            esp_err_t err = encrypt_data(&msg.data[CRYPTO_COUNTER_SIZE], decrypted_len, decrypted, rx_counter);
            if (err != ESP_OK) {
                printf("Failed to encrypt data: %s\n", esp_err_to_name(err) );
                return RX_RESULT_ERROR; // Encryption failed
            }

            decrypted[decrypted_len] = '\0';
            printf("Counter: %lu | Decrypted: %s\n", (unsigned long)rx_counter, (char *)decrypted);
            toggle_led();
            buzzer_sound();
            return RX_RESULT_OK; // Message received and processed successfully
        }
    return RX_RESULT_TIMEOUT;
}

   

bool receive_test_data() {
    
    // Generate a random timeout between 4000 and 6000 ms to avoid lockstep synchronization
    uint32_t timeout_ms = 4000 + (esp_random() % 2000);

    // Wait for a message to be received
    if (xQueueReceive(lora_get_queue(), &msg, pdMS_TO_TICKS(timeout_ms)) == pdTRUE) {
        // Process the received data
        printf("Received test data: %d bytes\n", msg.length);
        printf("Received test message: %.*s\n", msg.length, msg.data);   // print a string with a dynamic maximum length
        toggle_led();
        buzzer_sound();
        return true;
    }

    return false;
     
}