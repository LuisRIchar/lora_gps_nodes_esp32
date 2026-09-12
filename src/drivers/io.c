#include "io.h"

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void init_on_board_led();

void io_init() {
    init_on_board_led();
}

static void init_on_board_led() {
    gpio_reset_pin(IO_LED1);
    gpio_set_direction(IO_LED1, GPIO_MODE_OUTPUT);
    gpio_set_level(IO_LED1, 0);
}

void toggle_led() {
    static bool led_state = false;
    led_state = !led_state;
    gpio_set_level(IO_LED1, led_state);
}

bool get_io_num() {
    return gpio_get_level(IO_BAT); 
}

void buzzer_init() {
    
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_12_BIT,
        .freq_hz = 2000, // Set frequency to 2 kHz
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .gpio_num = IO_buzzer,
        .duty = 0, // Start with the buzzer off
        .hpoint = 0
    };
    ledc_channel_config(&ledc_channel);
}

void buzzer_sound() {

    uint16_t note_freq_song[] = {138, 110, 330, 349, 392, 440}; // Frequencies for C4, D4, E4, F4, G4, A4, B4
    // Set the buzzer frequency to 2 kHz
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 2048); // 50% duty cycle
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

    for (int i = 0; i < sizeof(note_freq_song) / sizeof(note_freq_song[0]); i++) {
        ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, note_freq_song[i]);
        vTaskDelay(pdMS_TO_TICKS(100)); // Play each note for 200 ms
    }

    // Turn off the buzzer
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}