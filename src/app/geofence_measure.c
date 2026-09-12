#include "geofence_measure.h"

#include "freertos/idf_additions.h"
#include "math.h"

#include "io.h"
#include "gps.h"

#define EARTH_RADIUS 6371000.0 // in meters
#define DEG_TO_RAD (M_PI / 180.0)
#define GEOFENCE_THRESHOLD_METERS 10.0 // 10 meters threshold
#define QUEUE_LENGTH 10

static TaskHandle_t geofence_task_handle;
static StackType_t geofence_task_stack[4098];
static StaticTask_t geofence_task_buffer;

static void geofence_measure_tsk(void *pv);
static double calculated_distance_meters(double lat1, double lon1, double lat2, double lon2);

static StaticQueue_t geofence_queue_struct;
static uint8_t geofence_queue_buffer[QUEUE_LENGTH * sizeof(geofence_coordinates_t)];

static QueueHandle_t geofence_queue = NULL;

void start_measure_of_geofence() {
    if (geofence_queue == NULL) {
        geofence_queue = xQueueCreateStatic(
            QUEUE_LENGTH,
            (UBaseType_t)sizeof(geofence_coordinates_t),
            geofence_queue_buffer,
            &geofence_queue_struct
        );
    }

    geofence_task_handle = xTaskCreateStatic(
        geofence_measure_tsk,          
        "geofence_measure_task",
        sizeof(geofence_task_stack) / sizeof(StackType_t),
        NULL,
        5,
        geofence_task_stack,
        &geofence_task_buffer
    );
}

static void geofence_measure_tsk(void *pv) {
    // Implement the geofence measurement logic here

    geofence_coordinates_t coordinates = {0};
    bool has_target_coordinates = false;
    gps_data_t gps_data = {0};

    while (1) {
        // Perform geofence measurement

        geofence_coordinates_t rx_target_coordinates;

        if(xQueueReceive(geofence_queue, &rx_target_coordinates, pdMS_TO_TICKS(1000)) == pdTRUE) {
            printf("Geofence coordinates received: %f, %f\n", rx_target_coordinates.latitude, rx_target_coordinates.longitude);
            coordinates = rx_target_coordinates;
            has_target_coordinates = true;
        }

        if (has_target_coordinates) {
            if (gps_result(&gps_data) && gps_data.valid) {
                double distance = calculated_distance_meters(gps_data.lat, gps_data.lon, coordinates.latitude, coordinates.longitude);
                printf("Current GPS: %.6f, %.6f | Target: %.6f, %.6f | Distance: %.2f meters\n",
                       gps_data.lat, gps_data.lon, coordinates.latitude, coordinates.longitude, distance);

                if (distance > GEOFENCE_THRESHOLD_METERS) { 
                    double excess = distance - GEOFENCE_THRESHOLD_METERS;
                    uint32_t loudness_level = (uint32_t)((excess / 50.0) * 100.0); 
                    if (loudness_level > 100U) loudness_level = 100U;
                    if (loudness_level < 20U) loudness_level = 20U;
                    buzzer_loudness(loudness_level);
                } else {
                    buzzer_loudness(0);
                }
            } else {
                printf("Waiting for valid GPS fix...\n");
            }
        } 
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 seconds
    }
}

QueueHandle_t get_geofence_queue() {
    // Return the queue handle for geofence measurements
    if (geofence_queue == NULL) {
        geofence_queue = xQueueCreateStatic(
            QUEUE_LENGTH,
            (UBaseType_t)sizeof(geofence_coordinates_t),
            geofence_queue_buffer,
            &geofence_queue_struct
        );
    }
    return geofence_queue; 
}

/***************************************************************************************************
     * Function: calculated_distance_meters
     *
     * Description:
     *   Computes the surface distance in meters between two geographic coordinates using the
     *   Equirectangular Flat-Surface Approximation (Pythagorean theorem on spherical coordinates).
     *
     * Why this formula?
     *   - Coordinates (lat/lon) are angles, not meters.
     *   - Full spherical formulas (Haversine) require multiple trigonometric evaluations (sin, cos,
     *     asin, atan2), which are computationally expensive on embedded MCUs.
     *   - For local perimeters (< 1 km), Earth curvature is negligible, making a tangent-plane
     *     projection accurate to within fractions of a percent (< 0.1% error).
     *
     * How it works:
     *   1. Converts angular degrees to radians (angle * PI / 180).
     *   2. Y-axis (North/South): delta_lat is constant everywhere on Earth.
     *   3. X-axis (East/West): delta_lon narrows as you move away from the Equator toward the poles.
     *      It is corrected by multiplying by cos(mean_latitude).
     *   4. Pythagoras: distance = EARTH_RADIUS * sqrt(x^2 + y^2).
     *
     * Parameters:
     *   lat1, lon1 - Local node coordinates (decimal degrees)
     *   lat2, lon2 - Target geofence center coordinates (decimal degrees)
     *
     * Returns:
     *   Distance in meters (double).
     ***************************************************************************************************/
static double calculated_distance_meters(double lat1, double lon1, double lat2, double lon2) {
    // Convert degrees to radians
    double phi1 = lat1 * DEG_TO_RAD;
    double phi2 = lat2 * DEG_TO_RAD;
    double delta_phi = (lat2 - lat1) * DEG_TO_RAD;
    double delta_lambda = (lon2 - lon1) * DEG_TO_RAD;

    double x = delta_lambda * cos((phi1 + phi2) / 2.0);
    double y = delta_phi;

    return EARTH_RADIUS * sqrt(( x * x ) +( y * y )); // Distance in meters
}