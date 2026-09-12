idf_component_register(
    SRCS "send_receive.c" "send_helper.c" "receive_helper.c" "geofence_measure.c"
    INCLUDE_DIRS "."
    REQUIRES drivers util
    )