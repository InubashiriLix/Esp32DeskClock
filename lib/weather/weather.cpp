#include <Arduino.h>
#include <weather.hpp>
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#include <oled.hpp>

String weather_url = "http://api.openweathermap.org/data/2.5/weather?id=1886760&appid=096929d18cfc08ebf0ec0683f6007e35&units=metric&lang=en";

weatherUpdateStatus_t weatherUpdateStatus = weatherUpdateStatus_t::WAITING;
WeatherInfo weather;

void updateWeatherTask(void *pvParameters)
{
  const TickType_t retryDelay = pdMS_TO_TICKS(10 * 1000); 

  for (;;)
  {
    // —— 1. 检查 WiFi —— 
    if (WiFi.status() != WL_CONNECTED)
    {
      weatherUpdateStatus = weatherUpdateStatus_t::WIFI_DISCONNECTED;
      Serial.println("[Weather] wifi disconnected, retrying...");
      vTaskDelay(retryDelay);
      continue;
    }

    // —— 2. 发起 HTTP GET —— 
    HTTPClient http;
    Serial.println("[Weather] sending HTTP GET request...");
    http.begin(weather_url.c_str());
    int httpCode = http.GET();
    Serial.printf("[Weather] http return code %d\n", httpCode);
    weatherUpdateStatus = weatherUpdateStatus_t::WAITING;

    if (httpCode != HTTP_CODE_OK)
    {
      Serial.printf("[Weather] HTTP fault %s\n", http.errorToString(httpCode).c_str());
      http.end();
      weatherUpdateStatus = weatherUpdateStatus_t::FAILED;
      vTaskDelay(retryDelay);
      continue;
    }

    // —— 3. 读取 Payload —— 
    String payload = http.getString();
    http.end();
    Serial.printf("[Weather] Payload len: %u bytes\n", payload.length());
    Serial.println("[Weather] Payload 200 cahrs");
    Serial.println(payload.substring(0, min((int)payload.length(), 200)));

    // —— 4. 解析 JSON —— 
    StaticJsonDocument<2048> doc;
    DeserializationError err = deserializeJson(doc, payload);
    if (err)
    {
      Serial.printf("[Weather] JSON parse failed %s\n", err.c_str());
      weatherUpdateStatus = weatherUpdateStatus_t::FAILED;
      vTaskDelay(retryDelay);
      continue;
    }
    Serial.println("[Weather] json parse success");

    // —— 5. 完整字段提取 —— 
    // coord
    weather.coord_lon = doc["coord"]["lon"].as<float>();
    weather.coord_lat = doc["coord"]["lat"].as<float>();
    Serial.printf("  coord: lon=%.3f, lat=%.3f\n", weather.coord_lon, weather.coord_lat);

    // weather[0]
    weather.weather_id = doc["weather"][0]["id"].as<int>();
    strlcpy(weather.weather_main, doc["weather"][0]["main"] | "", sizeof(weather.weather_main));
    strlcpy(weather.weather_desc, doc["weather"][0]["description"] | "", sizeof(weather.weather_desc));
    strlcpy(weather.weather_icon, doc["weather"][0]["icon"] | "", sizeof(weather.weather_icon));
    Serial.printf("  weather: %s (%d), icon=%s\n", weather.weather_main, weather.weather_id, weather.weather_icon);

    // base
    strlcpy(weather.base, doc["base"] | "", sizeof(weather.base));
    Serial.printf("  base: %s\n", weather.base);

    // main
    weather.temp       = doc["main"]["temp"].as<float>();
    weather.feels_like = doc["main"]["feels_like"].as<float>();
    weather.temp_min   = doc["main"]["temp_min"].as<float>();
    weather.temp_max   = doc["main"]["temp_max"].as<float>();
    weather.pressure   = doc["main"]["pressure"].as<int>();
    weather.humidity   = doc["main"]["humidity"].as<int>();
    weather.sea_level  = doc["main"]["sea_level"].as<int>();
    weather.grnd_level = doc["main"]["grnd_level"].as<int>();
    Serial.printf("  temps: %.1f°C (feels %.1f°C), min=%.1f max=%.1f\n",
                  weather.temp, weather.feels_like, weather.temp_min, weather.temp_max);

    // visibility
    weather.visibility = doc["visibility"].as<int>();
    Serial.printf("  visibility: %d m\n", weather.visibility);

    // wind
    weather.wind_speed = doc["wind"]["speed"].as<float>();
    weather.wind_deg   = doc["wind"]["deg"].as<int>();
    weather.wind_gust  = doc["wind"]["gust"].as<float>();
    Serial.printf("  wind: %.1f m/s at %d°, gust=%.1f\n",
                  weather.wind_speed, weather.wind_deg, weather.wind_gust);

    // clouds
    weather.cloud_all = doc["clouds"]["all"].as<int>();
    Serial.printf("  clouds: %d%%\n", weather.cloud_all);

    // timestamps
    weather.dt       = doc["dt"].as<long>();
    weather.sunrise  = doc["sys"]["sunrise"].as<long>();
    weather.sunset   = doc["sys"]["sunset"].as<long>();
    weather.timezone = doc["timezone"].as<long>();
    Serial.printf("  timestamps: dt=%ld, nz=%+ld\n", weather.dt, weather.timezone);

    // sys / location
    strlcpy(weather.country, doc["sys"]["country"] | "", sizeof(weather.country));
    Serial.printf("  country: %s\n", weather.country);

    // city
    weather.city_id = doc["id"].as<int>();
    strlcpy(weather.city_name, doc["name"] | "", sizeof(weather.city_name));
    weather.cod     = doc["cod"].as<int>();
    Serial.printf("  city: %s (id=%d, cod=%d)\n", weather.city_name, weather.city_id, weather.cod);

    time_t local = weather.dt + weather.timezone;
    struct tm *tmInfo = gmtime(&local);
    strftime(weather.update_time, sizeof(weather.update_time), "%Y-%m-%d %H:%M:%S", tmInfo);
    Serial.printf("update weather done at %s\n", weather.update_time);

    // —— 6. 完成 —— 
    weatherUpdateStatus = weatherUpdateStatus_t::SUCCESS;

    vTaskDelay(pdMS_TO_TICKS(10 * 60 * 1000)); // Delay for 10 minutes
  }
}

void updateWeatherDisplayTask(void* pvParameters) {
  const TickType_t delayTicks = pdMS_TO_TICKS(200);
  for (;;) {
    // Only show in WEATHER modes
    if (currDispMode != DISPLAY_MODE::WEATHER_1 && currDispMode != DISPLAY_MODE::WEATHER_2) {
      vTaskDelay(delayTicks);
      continue;
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);

    // Status messages
    switch (weatherUpdateStatus) {
      case weatherUpdateStatus_t::WIFI_DISCONNECTED:
        display.println("WiFi disconnected!\nretrying...");
        break;
      case weatherUpdateStatus_t::FAILED:
        display.println("Update failed!\nRetrying...");
        break;
      case weatherUpdateStatus_t::TIMEOUT:
        display.println("Update timeout\nRetrying...");
        break;
      case weatherUpdateStatus_t::WAITING:
        display.println("Updating...");
        break;
      case weatherUpdateStatus_t::SUCCESS:
        if (currDispMode == DISPLAY_MODE::WEATHER_1) {
          // First weather page
          display.printf("Time: \n%s", weather.update_time);
          display.setCursor(0, 16);
          display.printf("City: %s", weather.city_name);
          display.setCursor(0, 24);
          display.printf("Wx: %s", weather.weather_desc);
          display.setCursor(0, 40);
          display.printf("T: %.1fC", weather.temp);
          display.setCursor(0, 48);
          display.printf("H: %d%%", weather.humidity);
          display.setCursor(64, 48);
          display.printf("W: %.1fm/s", weather.wind_speed);
        } else {
          // Second weather page
          display.printf("City: %s", weather.city_name);
          display.setCursor(0, 16);
          display.printf("Feels: %.1fC", weather.feels_like);
          display.setCursor(0, 32);
          display.printf("Pres: %dhPa", weather.pressure);
          display.setCursor(0, 48);
          display.printf("Wind: %.1fm/s", weather.wind_speed);
        }
        break;
    }

    display.display();
    vTaskDelay(delayTicks);
  }
}