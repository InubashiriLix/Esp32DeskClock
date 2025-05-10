# pragma once

#define BUZZER_GPIO 0

enum class shout_type {
    shut_up,
    three_buzz,
    four_buzz,
    clock,
};


extern shout_type buzzer_shout_mode; 
void buzzer_setup();
void runBuzzerTask(void *args);
