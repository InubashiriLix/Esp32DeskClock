#include <Arduino.h>

#include "clock_time_task.hpp"
#include "timeDisplay.hpp"
#include "oled.hpp"
#include "config_display.hpp"
#include "buzzer.hpp"

void ClockMonitorTask(void *arg)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500)); // 0.5 second delay
        if (second < 3 && second > 0)
        {
            for (int i = 0; i < 3; i++)
            {
                if (clocks[i].enable_ && (clocks[i].hour_ == hour && clocks[i].min_ == minute))
                {
                    buzzer_shout_mode = shout_type::clock;
                    currDispMode = DISPLAY_MODE::ITR_CLOCK_TIME_UP;
                    break;
                }
            }
        }
    }
}

void ClockTimeUpItrDisplayTask(void *args) {
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(300));
        if (currDispMode == DISPLAY_MODE::ITR_CLOCK_TIME_UP)
        {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.printf("CLK Time Up!");
            display.setCursor(12, 32);
            display.printf("%02d:%02d:%02d", hour, minute, second);
            display.display();
        }
    }
}