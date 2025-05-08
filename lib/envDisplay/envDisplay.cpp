#include <Arduino.h>
#include <oled.hpp>
#include <dht.hpp>
#include <bmp280.hpp>

void envDisplayTask(void *pvParameters)
{
  for (;;)
  {
    if (currDispMode == DISPLAY_MODE::ENVIRONMENT)
    {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.printf("Enviroment Info\n");
      display.setCursor(0, 8);
      display.printf(String(dhtSuccess ? "dht: OK" : "dht: FAIL").c_str());
      display.setCursor(64, 8);
      display.printf("bmp: %s\n", bmp280Init ? "OK" : "FAIL");

      // dht not avaliable not
      // display.setCursor(0, 16);
      // display.printf("T: %.1f C\n", dht_tempe);
      // display.setCursor(64, 16);
      // display.printf("H: %.1f %%\n", dht_humi);

      display.setCursor(0, 24);
      display.printf("T: %.1f C\n", bmp280Temperature);
      display.setCursor(64, 24);
      display.printf("P: %.1f hPa\n", bmp280Pressure / 100.0f);
      display.setCursor(0, 32);
      display.printf("Alti: %.1f %%\n", bmp280Altitude);
      display.setCursor(64, 32);
      display.printf("H: %.1f %%\n", dht_humi);
      display.display();
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}