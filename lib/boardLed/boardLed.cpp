#include <Arduino.h>
#include <Wire.h>
#include <boardLed.hpp>

#define BOARD_LED 2
const int boardLedPwmChann = 1;
const int boardLedPwmFreq = 1000;
const int boardLedPwmResolution = 8; // 8 bits resolution

void boardLedSetup() {
  ledcSetup(boardLedPwmChann, boardLedPwmFreq, boardLedPwmResolution);
  ledcAttachPin(BOARD_LED, boardLedPwmChann);
}

void boardLedRunTask(void *pvParameters)
{
  int delayTime = pdMS_TO_TICKS(10);
  for (;;)
  {
    for (int d = 0; d < (1 << boardLedPwmResolution); d += 4)
    {
      ledcWrite(boardLedPwmChann, d);
      vTaskDelay(delayTime);
    }

    for (int d = (1 << boardLedPwmResolution); d >= 0; d -= 4)
    {
      ledcWrite(boardLedPwmChann, d);
      vTaskDelay(delayTime);
    }
  }
}

