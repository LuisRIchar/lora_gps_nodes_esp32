#pragma once

#include "driver/gpio.h"
#include "stdbool.h"

typedef enum {
    IO_BAT      = GPIO_NUM_1,
    IO_DIO0     = GPIO_NUM_2,
    IO_RESET    = GPIO_NUM_3,
    IO_NSS      = GPIO_NUM_4,
    IO_pull_up  = GPIO_NUM_5,
    IO_buzzer   = GPIO_NUM_6,
    IO_CLK      = GPIO_NUM_7,
    IO_MISO     = GPIO_NUM_8,
    IO_MOSI     = GPIO_NUM_9,
    IO_LED1     = GPIO_NUM_21,
    IO_UART_TX  = GPIO_NUM_43,
    IO_UART_RX  = GPIO_NUM_44,

} io_e;

void io_init();
void toggle_led();
bool get_io_num();
void buzzer_init();
void buzzer_sound();