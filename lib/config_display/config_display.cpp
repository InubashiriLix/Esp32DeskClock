#include <Arduino.h>
#include <LittleFS.h>

#include "config_display.hpp"
#include "oled.hpp"
#include <Preferences.h>
#include "timeDisplay.hpp"
#include "buzzer.hpp"

Preferences preferences;
preferences_datas pref_datas;

clock_index_setting_cursor_E clk_idx_curs = clock_index_setting_cursor_E::CLOCK_0;
clock_time_setting_cursor_E clk_time_curs = clock_time_setting_cursor_E::HOUR;
bool clk_time_curs_enable = false;

ClockSetting clocks[3];  // clock must be 3, if you want to change, then refactor will be hard

void saveClockSetting() {
    preferences.begin("clock", false);

    pref_datas.enable_0 = clocks[0].enable_;
    pref_datas.hour_0 = clocks[0].hour_;
    pref_datas.min_0 = clocks[0].min_;

    pref_datas.enable_1 = clocks[1].enable_;
    pref_datas.hour_1 = clocks[1].hour_;
    pref_datas.min_1 = clocks[1].min_;

    pref_datas.enable_2 = clocks[2].enable_;    
    pref_datas.hour_2 = clocks[2].hour_;
    pref_datas.min_2 = clocks[2].min_;

    preferences.putBytes("clock", &pref_datas, sizeof(pref_datas));
    preferences.end();
}

static void loadClockSetting() {
    preferences.begin("clock", true);
    if (preferences.isKey("clock")) {
        preferences.getBytes("clock", &pref_datas, sizeof(pref_datas));
        clocks[0].hour_ = pref_datas.hour_0;
        clocks[0].min_ = pref_datas.min_0;
        clocks[0].enable_ = pref_datas.enable_0;

        clocks[1].hour_ = pref_datas.hour_1;
        clocks[1].min_ = pref_datas.min_1;
        clocks[1].enable_ = pref_datas.enable_1;

        clocks[2].hour_ = pref_datas.hour_2;    
        clocks[2].min_ = pref_datas.min_2;
        clocks[2].enable_ = pref_datas.enable_2;
    } else {
        for (int i = 0; i < 3; i++) {
            clocks[i] = ClockSetting(0, 0, false);
        }
    }
    preferences.end();
}

static void clockSet(uint8_t index, uint8_t hour, uint8_t min, bool enable) {
    if (index < 3) {
        clocks[index].hour_ = hour;
        clocks[index].min_ = min;
        clocks[index].enable_ = enable;
        saveClockSetting();
    } else {
        Serial.println("Invalid clock index");
    }
}   

void clockInital() {
    loadClockSetting();
}

void dispalyConfigPageTask(void *args) {
    while (1) {
        if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && !clk_time_curs_enable) {
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.print("Clock Set");

            uint8_t clock_index = static_cast<uint8_t>(clk_idx_curs);
            display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            display.setTextSize(2);
            display.setCursor(0, 16 + ( clock_index  - 0) * 16);
            display.print(">");

            for (int i = 0; i < 3; i++) {
                if (i == clock_index) {
                    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
                } else {
                    display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
                }
                display.setTextSize(2);
                display.setCursor(20, 16 + i * 16);
                display.printf("%02d:%02d %s", clocks[i].hour_, clocks[i].min_, clocks[i].enable_ ? " ON" : "OFF");
            }
            display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);

            display.display();
        } else if (currDispMode == DISPLAY_MODE::CONFIG_PAGE && clk_time_curs_enable) {
            display.clearDisplay();
            display.setTextSize(2);
            display.setTextColor(SSD1306_WHITE);
            display.setCursor(0, 0);
            display.printf("Set Clock: No. %d", static_cast<uint8_t>(clk_idx_curs));

            display.setTextSize(2);

            if (clk_time_curs == clock_time_setting_cursor_E::HOUR) {
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            } else {
                display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            display.setTextSize(2);
            display.setCursor(12, 32);
            display.printf("%02d", clocks[static_cast<uint8_t>(clk_idx_curs)].hour_);

            if (clk_time_curs == clock_time_setting_cursor_E::MIN) {
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            } else {
                display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            display.setTextSize(2);
            display.setCursor(48, 32);
            display.printf("%02d", clocks[static_cast<uint8_t>(clk_idx_curs)].min_);

            if (clk_time_curs == clock_time_setting_cursor_E::ENABLE) {
                display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
            } else {
                display.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
            }
            display.setTextSize(2);
            display.setCursor(84, 32);
            display.printf("%s", clocks[static_cast<uint8_t>(clk_idx_curs)].enable_? " ON" : "OFF");

            display.display();
        }
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

void nextClock() {
  using U = std::underlying_type<clock_index_setting_cursor_E>::type;
  U v = static_cast<U>(clk_idx_curs);
  v = (v + 1) % static_cast<U>(clock_index_setting_cursor_E::COUNT);
  clk_idx_curs = static_cast<clock_index_setting_cursor_E>(v);
}

void nextTimeItem() {
  using U = std::underlying_type<clock_time_setting_cursor_E>::type;
  U v = static_cast<U>(clk_time_curs);
  v = (v + 1) % static_cast<U>(clock_time_setting_cursor_E::COUNT);
  clk_time_curs = static_cast<clock_time_setting_cursor_E>(v);
}