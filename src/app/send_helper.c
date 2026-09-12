#include "send_helper.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "string.h"

#include "lora_spi.h"
#include "io.h"
#include "gps.h"
#include "crypto.h"
#include "geofence_measure.h"

#define MESSSAGE_LEN 128

static void print_helper();


static gps_data_t gps_data = {0};
static uint32_t s_tx_counter = 0;

void send_message() {

    if (!gps_get_latest(&gps_data)) {
        // If GPS data is not valid, set default values
        gps_data.lat = 0.0;
        gps_data.lon = 0.0;

        return; // Exit the function if GPS data is not valid
    }

    // Prepare the message to be sent
    char message[MESSSAGE_LEN];
    snprintf(message, sizeof(message),
     "Node: %d, Lat: %.6f, Lon: %.6f, On Target: %d",
     get_io_num()+1, gps_data.lat, gps_data.lon, geofence_on_target());

    // Encrypt the message before sending
    uint8_t encrypted_message[MESSSAGE_LEN+MESSSAGE_LEN]; // Ensure enough space for encrypted data

    encrypted_message[0] = (uint8_t)(s_tx_counter >> 24) & 0xFF;
    encrypted_message[1] = (uint8_t)(s_tx_counter >> 16) & 0xFF;
    encrypted_message[2] = (uint8_t)(s_tx_counter >> 8) & 0xFF;
    encrypted_message[3] = (uint8_t)(s_tx_counter & 0xFF);

    esp_err_t err = encrypt_data((const uint8_t*)message, strlen(message), &encrypted_message[CRYPTO_COUNTER_SIZE], s_tx_counter);
    if (err != ESP_OK) {
        printf("Failed to encrypt data: %s\n", esp_err_to_name(err));
        return;
    }
    s_tx_counter++; // Increment the counter for the next message

    // Send the message via LoRa
    lora_send(encrypted_message, (size_t)(CRYPTO_COUNTER_SIZE + strlen(message)));

    print_helper();
}

static void print_helper() {
    if(gps_data.valid && gps_data.lat != 0.0 && gps_data.lon != 0.0) {
        printf("GPS Data: Latitude: %.6f, Longitude: %.6f\n", gps_data.lat, gps_data.lon);
    } 
}