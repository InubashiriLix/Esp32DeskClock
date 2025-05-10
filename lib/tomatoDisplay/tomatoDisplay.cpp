#include <Arduino.h>
#include <tomatoDisplay.hpp>
#include <oled.hpp>
#include <timeDisplay.hpp>
#include "buzzer.hpp"

volatile tomatoStatus_e tomatoStatus = tomatoStatus_e::STOPPED;
volatile bool time_up = false;
volatile int  tomato_cmp_cnt = 0;

// 25 分钟工作；5 分钟休息
static const int TOMATO_WORK_MIN = 1;
static const int TOMATO_BREAK_MIN = 1;

int tomato_minute = TOMATO_WORK_MIN;
int tomato_second = 0;

//—— 启动番茄钟（只能从 STOPPED 启动） ——
void startTomato() {
  if (tomatoStatus == tomatoStatus_e::STOPPED) {
    tomatoStatus   = tomatoStatus_e::RUNNING;
    time_up        = false;
    tomato_minute  = TOMATO_WORK_MIN;
    tomato_second  = 0;
    Serial.println("[Tomato] Started 25min session");
  }
}

void tomatoTimerTask(void *pvParameters)
{
    TickType_t lastWake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(1000); // 1s

    for (;;)
    {
        switch (tomatoStatus)
        {
        case tomatoStatus_e::STOPPED:
            break;

        case tomatoStatus_e::RUNNING:
            if (tomato_second == 0)
            {
                if (tomato_minute == 0)
                {
                    buzzer_shout_mode = shout_type::three_buzz;
                    time_up = true;
                    tomatoStatus = tomatoStatus_e::RELAXATION;
                    tomato_cmp_cnt++;
                    tomato_minute = TOMATO_BREAK_MIN;
                    tomato_second = 0;
                    Serial.println("[Tomato] Work done! Starting 5min break");
                }
                else
                {
                    tomato_minute--;
                    tomato_second = 59;
                }
            }
            else
            {
                tomato_second--;
            }
            break;

        case tomatoStatus_e::RELAXATION:
            // 休息倒计时
            if (tomato_second == 0)
            {
                if (tomato_minute == 0)
                {
                    buzzer_shout_mode = shout_type::four_buzz;
                    // 休息结束，停止并重置
                    Serial.println("[Tomato] Break over! Resetting");
                    tomatoStatus = tomatoStatus_e::STOPPED;
                    tomato_minute = TOMATO_WORK_MIN;
                    tomato_second = 0;
                    time_up = false;
                }
                else
                {
                    tomato_minute--;
                    tomato_second = 59;
                }
            }
            else
            {
                tomato_second--;
            }
            break;
        }

        vTaskDelayUntil(&lastWake, period);
    }
}

void tomatoDisplayTask(void *pvParameters) {
  const TickType_t period = pdMS_TO_TICKS(200); // 0.2s

  for (;;) {
    if (currDispMode == DISPLAY_MODE::TOMATO_CLOCK) {

      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Tomato Clock");

      if (tomatoStatus == tomatoStatus_e::RUNNING) {
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.println("Working...");
        display.setTextSize(2);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 26);
        display.printf("%02d:%02d", tomato_minute, tomato_second);
      } else if (tomatoStatus == tomatoStatus_e::RELAXATION) {
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.println("Relaxation...");
        display.setTextSize(2);
        display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
        display.setCursor(0, 26);
        display.printf("%02d:%02d", tomato_minute, tomato_second);
      } else if (tomatoStatus == tomatoStatus_e::STOPPED) {
        display.setTextSize(2);
        display.setCursor(0, 26);
        display.println(" Stopped");
        display.setTextSize(1);
      }

        display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        display.setTextSize(1);
        display.setCursor(0, 56);
        display.printf("time: %02d:%02d:%02d", hour, minute, second);
        display.setCursor(64, 8);
        display.printf("cnt: %d", tomato_cmp_cnt);

      display.display();
    }
    vTaskDelay(period);
  }
}