#include "send_receive.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "string.h"

#include "io.h"
#include "lora_spi.h"
#include "gps.h"
#include "crypto.h"

#include "send_helper.h"
#include "receive_helper.h"

static TaskHandle_t lora_task_handle, test_handle;
static StackType_t lora_task_stack[4098], test_stack[4098];
static StaticTask_t lora_task_buffer, test_buffer;

static void lora_tsk(void *pv);
static void test_tsk(void *pv);

void lora_task_init() {

    io_init();
    lora_spi_init();
    gps_init();
    crypto_init();
    buzzer_init();

    lora_receive(NULL, 0); // Start receiving data
    // Initialize the LoRa task

    lora_task_handle = xTaskCreateStatic(
        lora_tsk,          // Task function
        "lora_task",       // Name of the task
        sizeof(lora_task_stack) / sizeof(StackType_t), // Stack size
        NULL,              // Task parameters
        5,  // Task priority
        lora_task_stack,   // Stack buffer
        &lora_task_buffer  // Task control block
    );
}

void test_tasks() {
    // Run test tasks

    io_init();
    lora_spi_init();
    gps_init();
    crypto_init();
    buzzer_init();

    lora_receive(NULL, 0); // Start receiving data

    test_handle = xTaskCreateStatic(
        test_tsk,                // Task function
        "test_task",                 // Name of the task
        sizeof(test_stack) / sizeof(StackType_t), // Stack size
        NULL,                  // Task parameters
        5,                       // Task priority
        test_stack,          // Stack buffer
        &test_buffer           // Task control block
    );
}

static void lora_tsk(void *pv){

    while(1){

        rx_result_t result = receive_data();

        if (result == RX_RESULT_TIMEOUT) {

            send_message();   // If no data is received, send a message

            if(wait_tx_done(pdMS_TO_TICKS(5000))) {     // More time out for more spreading factor, less time for less spreading factor
                printf("Data sent successfully\n");
            } else {
                printf("Transmission timeout\n");
            }

            lora_receive(NULL, 0); // Start receiving data again

        }//End of if result == RX_RESULT_TIMEOUT
    }
}

static void test_tsk(void *pv){

    while(1){

        if(receive_test_data()) {
        }else{
            // If no data is received, send a message
            send_message();

            if(wait_tx_done(pdMS_TO_TICKS(5000))) {     // More time otu for more spreading factor, less time for less spreading factor
                printf("Data sent successfully\n");
            } else {
                printf("Transmission timeout\n");
            }

            lora_receive(NULL, 0); // Start receiving data again
        }//End of if receive data
    }
}