#include <Arduino.h>
#include <buzzer.hpp>

shout_type buzzer_shout_mode = shout_type::shut_up;

void buzzer_setup()
{
    pinMode(BUZZER_GPIO, OUTPUT);
    digitalWrite(BUZZER_GPIO, 0);
}

static void run_buzzer_4_shout()
{
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(BUZZER_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        digitalWrite(BUZZER_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

static void run_buzzer_3_shout()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(BUZZER_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(300));
        digitalWrite(BUZZER_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void runBuzzerTask(void *args)
{
    while (1)
    {
        switch (buzzer_shout_mode)
        {
        case shout_type::shut_up:
            break;
        case shout_type::three_buzz:
        {
            run_buzzer_3_shout();
            buzzer_shout_mode = shout_type::shut_up;
            break;
        }
        case shout_type::four_buzz:
        {
            run_buzzer_4_shout();
            buzzer_shout_mode = shout_type::shut_up;
            break;
        }
        case shout_type::clock:
        {
            run_buzzer_4_shout();
            // NOTE:  buzz until the mode is setted in the logic mode
            break;
        }
        default:
        {
            buzzer_shout_mode = shout_type::shut_up;
            break;
        }
        }
        vTaskDelay(300);
    }
}