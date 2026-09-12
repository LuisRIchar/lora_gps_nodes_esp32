#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef struct {
    double latitude;
    double longitude;
} geofence_coordinates_t;

void start_measure_of_geofence();
QueueHandle_t get_geofence_queue();
bool geofence_on_target();