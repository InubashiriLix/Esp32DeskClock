#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <oled.hpp>

DISPLAY_MODE currDispMode = DISPLAY_MODE::TIME;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void oledSetup()
{
  // ESP32: Wire.begin(SDA, SCL)
  Wire.begin(26 /* SDA */, 25 /* SCL */);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR))
  {
    Serial.println("SSD1306 init failed");
    while (1)
      ;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Hello ESP32 + OLED");
  display.display();
}