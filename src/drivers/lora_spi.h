#pragma once

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct{
    uint8_t data[256];
    uint16_t length;
}lora_msg_t;

void lora_spi_init(void);

void lora_send(uint8_t *data, size_t len);
void lora_receive(uint8_t *data, size_t len);
QueueHandle_t lora_get_queue();
bool wait_tx_done(TickType_t timeout_ms);
