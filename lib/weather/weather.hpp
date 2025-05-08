#pragma once

struct WeatherInfo {
  // coord
  float coord_lon;
  float coord_lat;
  // weather[0]
  int   weather_id;
  char  weather_main[16];
  char  weather_desc[32];
  char  weather_icon[8];
  // base
  char  base[16];
  // main
  float temp;
  float feels_like;
  float temp_min;
  float temp_max;
  int   pressure;
  int   humidity;
  int   sea_level;
  int   grnd_level;
  // visibility
  int   visibility;
  // wind
  float wind_speed;
  int   wind_deg;
  float wind_gust;
  // clouds
  int   cloud_all;
  // timestamps
  long  dt;
  long  sunrise;
  long  sunset;
  long  timezone;       // in seconds
  // system / location
  char  country[4];
  // city
  int   city_id;
  char  city_name[32];
  int   cod;
  // 计算得到的本地更新时间字符串 (东八区)
  char  update_time[32];
};

enum class weatherUpdateStatus_t {
  WAITING,
  SUCCESS,
  FAILED,
  TIMEOUT,
  WIFI_DISCONNECTED,
};
extern weatherUpdateStatus_t weatherUpdateStatus ;
extern WeatherInfo weather;

void updateWeatherTask(void *pvParameters);
void updateWeatherDisplayTask(void* pvParameters);