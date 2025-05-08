#include <Arduino.h>
#include <Wire.h>
#include "oled.hpp"
#include "modeLogic.hpp"
#include "tomatoDisplay.hpp"

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

void midPressedLogic() {
    Serial.println("[Mode] mid pressed");
    if (currDispMode == DISPLAY_MODE::TOMATO_CLOCK) {
        if (tomatoStatus == tomatoStatus_e::RUNNING || tomatoStatus == tomatoStatus_e::RELAXATION) {
            Serial.println("[Tomato] reset");
            tomatoStatus = tomatoStatus_e::STOPPED;
        } else {
            Serial.println("[Tomato] start");
            startTomato();
        }
    } else {
        Serial.println("[Mode] reset to TIME");
        currDispMode = DISPLAY_MODE::TIME;
    }
}

void modeChangeTask(void *pvParameters) {
  bool lastLeft  = LOW;
  bool lastRight = LOW;
  bool lastMid   = LOW;

  for (;;) {
      // 读取当前状态
      bool currLeft  = digitalRead(PIN_BUT_LEFT);
      bool currRight = digitalRead(PIN_BUT_RIGHT);
      bool currMid   = digitalRead(PIN_BUT_MID);

      // 左键上升沿 => 下一模式
      if (currLeft  && !lastLeft) {
          vTaskDelay(pdMS_TO_TICKS(50));       // 消抖
          if (digitalRead(PIN_BUT_LEFT)) {
              nextMode();
              Serial.println("LEFT pressed -> next mode");
          }
          // 等待松开
          while (digitalRead(PIN_BUT_LEFT))
              vTaskDelay(pdMS_TO_TICKS(50));
      }

      // 右键上升沿 => 上一模式
      if (currRight && !lastRight) {
          vTaskDelay(pdMS_TO_TICKS(50));
          if (digitalRead(PIN_BUT_RIGHT)) {
              prevMode();
              Serial.println("RIGHT pressed -> prev mode");
          }
          while (digitalRead(PIN_BUT_RIGHT))
              vTaskDelay(pdMS_TO_TICKS(50));
      }

      // 中键上升沿 => 重置到 TIME 模式
      if (currMid && !lastMid) {
          vTaskDelay(pdMS_TO_TICKS(50));
          if (digitalRead(PIN_BUT_MID)) {
              Serial.println("MID pressed -> reset to TIME");
              midPressedLogic();
          }
          while (digitalRead(PIN_BUT_MID))
              vTaskDelay(pdMS_TO_TICKS(50));
      }

      // 更新上一次状态
      lastLeft  = currLeft;
      lastRight = currRight;
      lastMid   = currMid;

      // 空转，避免 100% 占用
      vTaskDelay(pdMS_TO_TICKS(50));
  }
}
