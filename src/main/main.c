#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "send_receive.h"
#include "geofence_measure.h"

void app_main(void){

    printf("Hello, World!\n");
    
    //test_tasks();
    lora_task_init();
    start_measure_of_geofence();
    
    while (1) {
        vTaskDelay(portMAX_DELAY);
    }

    
}
