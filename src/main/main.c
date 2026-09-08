#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "send_receive.h"

void app_main(void){

    printf("Hello, World!\n");
    
    //test_tasks();
    lora_task_init();
    
    while (1) {
        vTaskDelay(portMAX_DELAY);
    }

    
}
