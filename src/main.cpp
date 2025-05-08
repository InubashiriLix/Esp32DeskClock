#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>
#include <DS18B20.h>
#include <Adafruit_BMP280.h>

#include "boardLed.hpp"
#include "dht.hpp"
#include "oled.hpp"
#include "timeDisplay.hpp"
#include "modeLogic.hpp"
#include "weather.hpp"
#include "envDisplay.hpp"
#include "tomatoDisplay.hpp"
#include "bmp280.hpp"
#include "rgb.hpp"

// If your module has no reset pin wired, pass -1 as the last arg:

const char *ssid = "your_ssid";
const char *password = "your_password";

void wifiSetup()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // update the screen display
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.printf("Connecting to %s\n", ssid);
  display.setTextSize(1);
  display.setCursor(0, 32);
  display.printf("[                   ]");
  display.display();

  Serial.println("Connecting to WiFi...");

  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    if (cnt > 16)
    {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(2);
      display.printf("WIFI FAILED");

      Serial.println("Failed to connect to WiFi. Please check your credentials.");
      cnt == 0;
      display.display();
      delay(1000);
      return;
    }
    display.setCursor(7 + 7 * cnt, 32);
    display.printf("#");
    display.display();

    Serial.print(".");

    cnt += 1;
    delay(500);
  }

  Serial.println("Connected to WiFi!");
  display.clearDisplay();
  display.setCursor(0, 10);
  display.setTextSize(2);
  display.printf("WIFI DONE");
  display.display();
  delay(1000);
}

void pinSetup()
{
  pinMode(PIN_BUT_LEFT, INPUT_PULLDOWN);
  pinMode(PIN_BUT_RIGHT, INPUT_PULLDOWN);
  pinMode(PIN_BUT_MID, INPUT_PULLDOWN);
}


void setup()
{
  Serial.begin(115200);

  pinSetup();
  boardLedSetup();
  rgbSetup();
  dhtSetup();
  bmp280Setup();

  oledSetup();
  wifiSetup();

  xTaskCreate(dhtUpdateTask, "dht update", 2048, NULL, 1, NULL);

  xTaskCreate(boardLedRunTask, "board led run", 2048, NULL, 1, NULL);

  xTaskCreate(rgbRunTask, "rgb run task", 2048, NULL, 1, NULL);

  xTaskCreate(bmp280UpdateTask, "bmp280 update", 2048, NULL, 1, NULL);

  xTaskCreate(modeChangeTask, "mode change", 1024, NULL, 1, NULL);  

  xTaskCreate(updateDateTimeItqTask, "update time", 2048, NULL, 1, NULL);
  xTaskCreate(displayTimeTask, "DisplayTime", 2048, NULL, 1, NULL);

  xTaskCreate(updateWeatherTask, "update weather", 4096, NULL, 1, NULL);
  xTaskCreate(updateWeatherDisplayTask, "update weather display", 2048, NULL, 1, NULL);

  xTaskCreate(envDisplayTask, "env display", 2048, NULL, 1, NULL);

  xTaskCreate(tomatoTimerTask, "tomato timer", 2048, NULL, 1, NULL);
  xTaskCreate(tomatoDisplayTask, "tomato display", 2048, NULL, 1, NULL);

}

void loop()
{
  // your drawing code...
}
