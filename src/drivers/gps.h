#pragma once

#include "stdbool.h"

typedef struct {
    double lon, lat;
    bool valid;
}gps_data_t;

void gps_init();
bool gps_result(gps_data_t* gps_data);