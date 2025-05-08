#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <dht.hpp>

// the Temperature and humidity sensor
DHT dht(DHTPIN, DHTTYPE);
float dht_tempe = 0.0;
float dht_humi = 0.0;

bool dhtSuccess = false; // Flag to indicate if DHT sensor is initialized successfully

void dhtSetup()
{
  dht.begin();
}

void dhtUpdateTask(void *pvParameters) // Task to update temperature and humidity
{
  for (;;)
  {
    dht_tempe = dht.readTemperature();
    dht_humi = dht.readHumidity();
    if (isnan(dht_tempe) || isnan(dht_humi))
    {
      Serial.println("Failed to read from DHT sensor!");
      vTaskDelay(pdMS_TO_TICKS(2 * 1000)); // Delay for 5 seconds
      dhtSuccess = false; // Set the flag to false if reading fails
      continue;
    }

    Serial.println("DHT update: ");
    Serial.printf("Temperature: %.1f °C\n", dht_tempe);

    vTaskDelay(pdMS_TO_TICKS(2 * 1000)); // Delay for 5 seconds
  }
}