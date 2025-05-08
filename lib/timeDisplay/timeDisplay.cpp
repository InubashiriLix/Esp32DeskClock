#include <Arduino.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <oled.hpp>

int year = 2025;
int month = 1;
int day = 1;
int hour = 0;
int minute = 0;
int second = 0;

void updateDateTimeItqTask(void *pvParameters) // Task to update date and time
{
  // Get the current time from the NTP server
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  // NOTE: this task must be previous than the time display task
  for (;;)
  {
    Serial.println("update date and time");
    if (WiFi.status() == WL_CONNECTED)
    {

      struct tm timeinfo;

      if (!getLocalTime(&timeinfo))
      {
        Serial.println("Failed to obtain time");
        vTaskDelay(pdMS_TO_TICKS(5 * 1000));
        continue;
      }
      year = timeinfo.tm_year + 1900;
      month = timeinfo.tm_mon + 1;
      day = timeinfo.tm_mday;
      hour = timeinfo.tm_hour;
      minute = timeinfo.tm_min;
      second = timeinfo.tm_sec;
    }
    else
    {
      Serial.println("WiFi not connected. Cannot update date and time.");
    }
    Serial.printf("Current time: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
    vTaskDelay(pdMS_TO_TICKS(60 * 60 * 1000)); // Delay for 1 hour
  }
}

void displayTimeTask(void *pvParameters)
{

  TickType_t lastWake = xTaskGetTickCount();
  const TickType_t delayTime = pdMS_TO_TICKS(1000);

  for (;;)
  {
    if (currDispMode == DISPLAY_MODE::TIME)
    {
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.setTextColor(SSD1306_WHITE);
      display.printf("%04d-%02d-%02d\n", year, month, day);
      display.setCursor(0, 32);
      display.printf(" %02d:%02d:%02d", hour, minute, second);
      display.setCursor(0, 56);
      display.setTextSize(1);
      display.printf(" %s", WiFi.status() == WL_CONNECTED ? "Online" : "Offline");
      display.display();
    }

    if (second + 1 > 59)
    {
      second = 0;
      minute++;
      if (minute + 1 > 59)
      {
        minute = 0;
        hour++;
        if (hour + 1 > 23)
          hour = 0;
      }
    }
    else
    {
      second++;
    }

    Serial.println("timer display iterrupt");
    vTaskDelayUntil(&lastWake, delayTime);
  }
}