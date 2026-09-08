#include "lora_spi.h"

#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "stdbool.h"
#include "string.h"

#include "hal/spi_types.h"
#include "io.h"
#include "sx127x.h"

#define FREQ 433000000ULL
#define SYNC_WORD 0x12

static void tx_callback(void *arg); 
static void rx_callback(void *arg, uint8_t *data, uint16_t length);
static void rst_init();
static void lora_DIO0_isr_handler(void *arg);
static void interrupt_init();
static void lora_driver_task(void *pv);

static spi_device_handle_t lora_spi;
static sx127x lora;
static SemaphoreHandle_t lora_semaphore;
static SemaphoreHandle_t lora_mutex;
static SemaphoreHandle_t lora_tx_done_semaphore;
static StaticSemaphore_t lora_semaphore_buffer;
static QueueHandle_t lora_queue;

void lora_spi_init(void) {
    
    lora_semaphore = xSemaphoreCreateBinary();
    lora_mutex = xSemaphoreCreateMutex();
    lora_queue = xQueueCreate(10, sizeof(lora_msg_t));
    lora_tx_done_semaphore = xSemaphoreCreateBinaryStatic(&lora_semaphore_buffer);

    interrupt_init();

    spi_bus_config_t buscfg = {
        .miso_io_num = IO_MISO,
        .mosi_io_num = IO_MOSI,
        .sclk_io_num = IO_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,

    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 4000000,           // Clock out at 4 MHz
        .mode = 0,                          // SPI mode 0
        .spics_io_num = IO_NSS,
        .queue_size = 7,
        .address_bits = 8,                  //Required for sx127x lib
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &lora_spi));
    

    rst_init();

    ESP_ERROR_CHECK(sx127x_create(lora_spi,&lora));

    //Mode of lora
    ESP_ERROR_CHECK(sx127x_set_opmod(SX127X_MODE_SLEEP, SX127X_MODULATION_FSK ,&lora));  
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(sx127x_set_opmod(SX127X_MODE_SLEEP, SX127X_MODULATION_LORA ,&lora));
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_ERROR_CHECK(sx127x_set_opmod(SX127X_MODE_STANDBY, SX127X_MODULATION_LORA ,&lora)); // Works as normal lora, nos fsk or ood
    vTaskDelay(pdMS_TO_TICKS(10));  // Works as normal lora, nos fsk or ood
    ESP_ERROR_CHECK(sx127x_set_frequency(FREQ, &lora));                                      // Set the frequency to 433 MHz
    ESP_ERROR_CHECK(sx127x_lora_set_implicit_header(NULL, &lora));                              // Tell receiver to espect packet header
    ESP_ERROR_CHECK(sx127x_rx_set_lna_gain(SX127X_LNA_GAIN_AUTO, &lora));                         // Set the LNA gain to auto, maximize recieve sensibility
    ESP_ERROR_CHECK(sx127x_tx_set_pa_config(SX127X_PA_PIN_BOOST, 10, &lora));                // Route the PA to the boost pin, and set the power to 10 dBm
    ESP_ERROR_CHECK(sx127x_lora_reset_fifo(&lora));                                                    // Delete all data in the FIFO
    ESP_ERROR_CHECK(sx127x_lora_set_bandwidth(SX127X_BW_125000, &lora));                    // Set the bandwidth to 125 kHz, means faster data
    ESP_ERROR_CHECK(sx127x_lora_set_spreading_factor(SX127X_SF_12, &lora));           // The freq to represent a bit, 7 fast but shorter range, 12 slower but longer range   
    ESP_ERROR_CHECK(sx127x_lora_set_syncword(SYNC_WORD, &lora));                                // Set the sync word to 0x12, means only devices with the same sync word can communicate

    xTaskCreate(lora_driver_task, "lora_driver", 4096, NULL, 10, NULL);
}

void lora_send(uint8_t *data, size_t len) {

    if(xSemaphoreTake(lora_mutex, portMAX_DELAY) == pdTRUE) {

        sx127x_set_opmod(SX127X_MODE_STANDBY, SX127X_MODULATION_LORA ,&lora);

        sx127x_tx_header_t header = {
            .enable_crc = true,
            .coding_rate = SX127X_CR_4_5,
        };
        sx127x_lora_tx_set_explicit_header(&header, &lora);

        xSemaphoreTake(lora_tx_done_semaphore, 0);                                          // Clear the semaphore before starting transmission
        sx127x_tx_set_callback(tx_callback, NULL, &lora);                       // Set the callback for transmission complete, a safe way to know when the transmission is complete, and the FIFO is empty

        sx127x_lora_tx_set_for_transmission(data, len, &lora);
        sx127x_set_opmod(SX127X_MODE_TX, SX127X_MODULATION_LORA ,&lora);

        xSemaphoreGive(lora_mutex);

    }


}

void lora_receive(uint8_t *data, size_t len) {

    if(xSemaphoreTake(lora_mutex, portMAX_DELAY) == pdTRUE) {

        sx127x_rx_set_callback(rx_callback, NULL, &lora);
        sx127x_set_opmod(SX127X_MODE_RX_CONT, SX127X_MODULATION_LORA ,&lora);
        xSemaphoreGive(lora_mutex);

    }
}

QueueHandle_t lora_get_queue() {
    return lora_queue;
}

bool wait_tx_done(TickType_t timeout_ms) {
    
    return xSemaphoreTake(lora_tx_done_semaphore, (timeout_ms)) == pdTRUE;
}

static void tx_callback(void *arg) {
    // Transmission complete callback
    (void)arg;                                      // Unused parameter
    (void)xSemaphoreGive(lora_tx_done_semaphore);   // Signal that transmission is complete
}

static void rx_callback(void *arg, uint8_t *data, uint16_t length) {
    // Handle received data
    lora_msg_t msg = {0};

    if(length > 256) length = 256; // Ensure we don't overflow the buffer

    memcpy(msg.data, data, length);

    msg.length = length;
    
    xQueueSend(lora_queue, &msg, portMAX_DELAY);
}


static void rst_init(){

    gpio_reset_pin(IO_RESET);
    gpio_set_direction(IO_RESET, GPIO_MODE_OUTPUT);

    gpio_set_level(IO_RESET, 0);
    vTaskDelay(pdMS_TO_TICKS(1) );
    gpio_set_level(IO_RESET, 1);
    vTaskDelay(pdMS_TO_TICKS(5) );
}

static void IRAM_ATTR lora_DIO0_isr_handler(void *arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xSemaphoreGiveFromISR(lora_semaphore, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

static void interrupt_init() {
    gpio_reset_pin(IO_DIO0);
    gpio_set_direction(IO_DIO0, GPIO_MODE_INPUT);
    gpio_set_intr_type(IO_DIO0, GPIO_INTR_POSEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(IO_DIO0, lora_DIO0_isr_handler, &lora);
}

static void lora_driver_task(void *pv) {

    while(1) {
        if(xSemaphoreTake(lora_semaphore, portMAX_DELAY) == pdTRUE) {
            if(xSemaphoreTake(lora_mutex, portMAX_DELAY) == pdTRUE) {

                sx127x_handle_interrupt(&lora);
                xSemaphoreGive(lora_mutex);

            }   //if mutex
        }       //if semaphore
    }
}

