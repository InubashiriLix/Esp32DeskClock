#include <Arduino.h>
#include <Wire.h>
#include "oled.hpp"
#include "modeLogic.hpp"
#include "tomatoDisplay.hpp"
#include "config_display.hpp"
#include "buzzer.hpp"

inline void nextMode() {
  using U = std::underlying_type<DISPLAY_MODE>::type;
  U v = static_cast<U>(currDispMode);
  v = (v + 1) % static_cast<U>(DISPLAY_MODE::COUNT);
  currDispMode = static_cast<DISPLAY_MODE>(v);
}

inline void prevMode()
{
  using U = std::underlying_type<DISPLAY_MODE>::type;
  U v = static_cast<U>(currDispMode);
  v = (v + static_cast<U>(DISPLAY_MODE::COUNT) - 1) % static_cast<U>(DISPLAY_MODE::COUNT);
  currDispMode = static_cast<DISPLAY_MODE>(v);
}

static void leftPressedLogic(bool longPress = false) {
    switch (currDispMode) {
        case DISPLAY_MODE::CONFIG_PAGE: {
            if (clk_time_curs_enable) {
                switch (clk_time_curs)
                {
                case clock_time_setting_cursor_E::HOUR:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].hour_ = (clocks[static_cast<uint8_t>(clk_idx_curs)].hour_ + 23) % 24;
                    break;
                }
                case clock_time_setting_cursor_E::MIN:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].min_ = (clocks[static_cast<uint8_t>(clk_idx_curs)].min_ + 59) % 60;
                    break;
                }
                case clock_time_setting_cursor_E::ENABLE:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].disable();
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            else
            {
                Serial.println("LEFT pressed -> prev mode");
                prevMode();
            }
            break;
        }
        default:
        {
            Serial.println("LEFT pressed -> prev mode");
            prevMode();
            break;
        }
    }
}

static void rightPressedLogic(bool longPress = false) {
    switch (currDispMode) {
        case DISPLAY_MODE::CONFIG_PAGE: {
            if (clk_time_curs_enable) {
                switch (clk_time_curs)
                {
                case clock_time_setting_cursor_E::HOUR:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].hour_ = (clocks[static_cast<uint8_t>(clk_idx_curs)].hour_ + 1) % 24;
                    break;
                }
                case clock_time_setting_cursor_E::MIN:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].min_ = (clocks[static_cast<uint8_t>(clk_idx_curs)].min_ + 1) % 60;
                    break;
                }
                case clock_time_setting_cursor_E::ENABLE:
                {
                    clocks[static_cast<uint8_t>(clk_idx_curs)].enable();
                    break;
                }
                default:
                {
                    break;
                }
                }
            }
            else
            {
                Serial.println("RIGHT pressed -> next mode");
                nextMode();
            }
            break;
        }
        default:
        {
            Serial.println("RIGHT pressed -> next mode");
            nextMode();
            break;
        }
    }
}

void midPressedLogic(bool longPress = false)
{
    Serial.println("[Mode] mid pressed");
    if (currDispMode == DISPLAY_MODE::TOMATO_CLOCK)
    {
        if (tomatoStatus == tomatoStatus_e::RUNNING || tomatoStatus == tomatoStatus_e::RELAXATION)
        {
            Serial.println("[Tomato] reset");
            tomatoStatus = tomatoStatus_e::STOPPED;
        }
        else
        {
            Serial.println("[Tomato] start");
            startTomato();
        }
    }
    else if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && !longPress && !clk_time_curs_enable)
    {
        Serial.println("[Mode] config page -> change clock");
        nextClock();
    }
    else if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && longPress && !clk_time_curs_enable)
    {
        Serial.println("[Mode] config page -> start change time item");
        clk_time_curs_enable = true;
    }
    else if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && !longPress && clk_time_curs_enable)
    {
        Serial.println("[Mode] config page -> change time item");
        nextTimeItem();
    }
    else if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && longPress && clk_time_curs_enable)
    {
        Serial.println(
            "[Mode] config page -> change time itme done, saving");
        saveClockSetting();
        clk_time_curs_enable = false;
    }
    else if (currDispMode == DISPLAY_MODE::TIME)
    {
        Serial.println("[Mode] reset to TIME");
        buzzer_shout_mode = shout_type::shut_up;
        currDispMode = DISPLAY_MODE::TIME;
    }
    else
    {
        currDispMode = DISPLAY_MODE::TIME;
        Serial.println("[Mode] reset to TIME");
    }
}

void modeChangeTask(void *pvParameters)
{
    bool lastLeft = LOW;
    bool lastRight = LOW;
    bool lastMid = LOW;

    TickType_t midDownTick = 0;

    for (;;)
    {
        // 读取当前状态
        bool currLeft = digitalRead(PIN_BUT_LEFT);
        bool currRight = digitalRead(PIN_BUT_RIGHT);
        bool currMid = digitalRead(PIN_BUT_MID);

        // 左键上升沿 => 下一模式
        if (currLeft && !lastLeft)
        {
            vTaskDelay(pdMS_TO_TICKS(50)); // 消抖
            if (digitalRead(PIN_BUT_LEFT))
            {
                leftPressedLogic();
            }
            // 等待松开
            while (digitalRead(PIN_BUT_LEFT))
                vTaskDelay(pdMS_TO_TICKS(50));
        }

        // 右键上升沿 => 上一模式
        if (currRight && !lastRight)
        {
            vTaskDelay(pdMS_TO_TICKS(50));
            if (digitalRead(PIN_BUT_RIGHT))
            {
                rightPressedLogic();
            }
            while (digitalRead(PIN_BUT_RIGHT))
                vTaskDelay(pdMS_TO_TICKS(50));
        }

        //   // 中键上升沿 => 重置到 TIME 模式
        //   if (currMid && !lastMid) {
        //       vTaskDelay(pdMS_TO_TICKS(50));
        //       if (digitalRead(PIN_BUT_MID)) {
        //           Serial.println("MID pressed -> reset to TIME");
        //           midPressedLogic();
        //       }
        //       while (digitalRead(PIN_BUT_MID))
        //           vTaskDelay(pdMS_TO_TICKS(50));
        //   }
        if (currMid && !lastMid)
        {
            midDownTick = xTaskGetTickCount(); // 记录开始时间
            vTaskDelay(pdMS_TO_TICKS(10));     // 简单消抖
        }
        if (!currMid && lastMid)
        {
            TickType_t dt = xTaskGetTickCount() - midDownTick;

            if (dt >= LONG_PRESS_TICKS)
                midPressedLogic(true); // 长按
            else
                midPressedLogic(); // 短按

            vTaskDelay(pdMS_TO_TICKS(10)); // 再消抖一次
        }

        // 更新上一次状态
        lastLeft = currLeft;
        lastRight = currRight;
        lastMid = currMid;

        // 空转，避免 100% 占用
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}
