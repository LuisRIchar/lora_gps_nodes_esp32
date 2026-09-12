#include "gps.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/uart.h"
#include "stdlib.h"

#include "io.h"

static double nmea_to_decimal(const char *nmea_val, char direction);
static void parser_gps(const char* sentence, gps_data_t* gps_data);
static void gps_reader_task(void *pvParameters);

static char line_buffer[128];           // Buffer to hold a single line of GPS data
static int line_index = 0;
static uint8_t gps_buffer[256];         // Buffer to hold incoming GPS data of uart

static gps_data_t s_latest_gps = {0};
static SemaphoreHandle_t s_gps_mutex = NULL;
static StaticSemaphore_t s_gps_mutex_buffer;

static TaskHandle_t s_gps_task_handle = NULL;
static StackType_t s_gps_task_stack[2048];
static StaticTask_t s_gps_task_buffer;

static void gps_reader_task(void *pvParameters) {
    gps_data_t temp_gps = {0};
    while (1) {
        if (gps_result(&temp_gps) && temp_gps.valid) {
            if (s_gps_mutex != NULL && xSemaphoreTake(s_gps_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
                s_latest_gps = temp_gps;
                xSemaphoreGive(s_gps_mutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

void gps_init() {
    // Initialize GPS

    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, IO_UART_TX, IO_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    if (s_gps_mutex == NULL) {
        s_gps_mutex = xSemaphoreCreateMutexStatic(&s_gps_mutex_buffer);
    }

    if (s_gps_task_handle == NULL) {
        s_gps_task_handle = xTaskCreateStatic(
            gps_reader_task,
            "gps_task",
            (uint32_t)(sizeof(s_gps_task_stack) / sizeof(StackType_t)),
            NULL,
            4,
            s_gps_task_stack,
            &s_gps_task_buffer
        );
    }
}

bool gps_get_latest(gps_data_t* dest) {
    if (dest == NULL || s_gps_mutex == NULL) {
        return false;
    }
    if (xSemaphoreTake(s_gps_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
        *dest = s_latest_gps;
        xSemaphoreGive(s_gps_mutex);
        return dest->valid;
    }
    return false;
}

bool gps_result(gps_data_t* gps_data) {
    
    int len = 0;

    len = uart_read_bytes(UART_NUM_1, gps_buffer, sizeof(gps_buffer) - 1, pdMS_TO_TICKS(1000));

    if(len > 0) {

        for(int i = 0; i < len; i++){
            char c = (char)gps_buffer[i];
            if(c == '\n') {
                line_buffer[line_index] = '\0';                              //Lines is complete, null terminate the string

                if( strncmp(line_buffer, "$GPRMC", 6) == 0 
                    || strncmp(line_buffer, "$GNRMC", 6) == 0) {             //Check if the line is a GPGGA sentence
                    parser_gps(line_buffer, gps_data);             //Parse the GPGGA sentence
                    line_index = 0;                                         //Reset the line index for the next line
                    return gps_data->valid;                                 //Return the validity of the GPS data
                }

                line_index = 0;                                              //Reset the line index if it was not a GPGGA sentence
            }else if(c != '\r' && line_index < sizeof(line_buffer) - 1) {
                line_buffer[line_index++] = c;                               //Add character to the line buffer
            }
        }
    }
    
    if(len == 0) {
        printf("No data received from GPS\n");
        
    }

    return false;
}

static void parser_gps( const char* sentence, gps_data_t* gps_data){

    char status = 'V';                                  // Default to invalid status
    int comma_count = 0;
    const char *p = sentence;

    const char *lat_str = NULL;
    char ns_dir = 'N';
    const char *lon_str = NULL;
    char ew_dir = 'E';

    while ( *p != '\0') {                               //Step through the sentence until the end, \0 null character
        if( *p == ','){
            comma_count++;

            // Pointer is at the comma and p + 1 is the start of the next field
            if(comma_count == 2) {
                status = *(p + 1);                        // Status is the 2nd field
            } else if(comma_count == 3) {
                lat_str = p + 1;                          // Latitude is the 3rd field
            } else if(comma_count == 4) {
                ns_dir = *(p + 1);                        // N/S direction is the 4th field
            } else if(comma_count == 5) {
                lon_str = p + 1;                          // Longitude is the 5th field
            } else if(comma_count == 6) {
                ew_dir = *(p + 1);                        // E/W direction is the 6th field
            }
        }

        p++;

    }   //End while loop

    if(( status == 'A') && ( lon_str != NULL) && (lat_str != NULL)){
        gps_data->lat = nmea_to_decimal(lat_str, ns_dir);
        gps_data->lon = nmea_to_decimal(lon_str, ew_dir);
        gps_data->valid = true;

    } else {
        gps_data->valid = false;

    }
    
}

/**
*
*   We convert the NMEA format to decimal degrees. The NMEA format is in the form of ddmm.mmmm for 
*   latitude and dddmm.mmmm for longitude. We extract the degrees and minutes, convert them to decimal
*   degrees, and apply the direction (N/S/E/W) to determine the sign of the result.
*   We need to separate the first four characters that are the degrees from the rest that are the minutes. 
*   We then convert the degrees to decimal degrees. Because mapping systems use decimal degrees, we convert 
*   the minutes to decimal degrees by dividing by 60.
*/
static double nmea_to_decimal(const char *nmea_val, char direction) {
    
    double raw = strtod(nmea_val, NULL);                           // Convert string to double
    int degrees = (int)(raw / 100.0);                              // Extract degrees
    double minutes = raw - (double)(degrees * 100.0);              // Extract minutes       
    double decimal = (double)degrees + (minutes / 60.0);           // Convert to decimal degrees

    if(direction == 'S' || direction == 'W') {
        decimal = -decimal;                                        // Negate for South and West
    }
    return decimal; 
}