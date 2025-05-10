#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C // 0x78>>1


enum class DISPLAY_MODE
{
  TIME,
  WEATHER_1,
  WEATHER_2,
  ENVIRONMENT,
  TOMATO_CLOCK,
  CONFIG_PAGE,
  COUNT,
  ITR_CLOCK_TIME_UP, // shit mode
};

extern DISPLAY_MODE currDispMode ;
extern Adafruit_SSD1306 display;

void oledSetup();