idf_component_register(
    SRCS "io.c" "lora_spi.c" "gps.c" 
    INCLUDE_DIRS "."
    REQUIRES esp_driver_gpio esp_driver_spi dernasherbrezon__sx127x esp_driver_uart esp_driver_ledc
    )