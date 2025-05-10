#pragma once

#include <Arduino.h>

enum class clock_index_setting_cursor_E
{
    CLOCK_0,
    CLOCK_1,
    CLOCK_2,
    COUNT,
};

enum class clock_time_setting_cursor_E
{
    HOUR,
    MIN,
    ENABLE,
    COUNT,
};

struct preferences_datas
{
    uint8_t hour_0;
    uint8_t min_0;
    bool enable_0;

    uint8_t hour_1;
    uint8_t min_1;
    bool enable_1;

    uint8_t hour_2;
    uint8_t min_2;
    bool enable_2;
};

struct ClockSetting
{
    uint8_t hour_;
    uint8_t min_;
    bool enable_;
    ClockSetting() = default;
    ClockSetting(const uint8_t hour, const uint8_t min, const bool enable)
    {
        hour_ = hour,
        min_ = min;
        enable_ = enable;
    }
    void disable()
    {
        this->enable_ = false;
    }

    void enable()
    {
        this->enable_ = true;
    }
};

void clockInital();
void saveClockSetting();
void nextClock();
void nextTimeItem();
void dispalyConfigPageTask(void *args);

extern ClockSetting clocks[3]; // clock must be 3, if you want to change, then refactor will be hard
extern clock_index_setting_cursor_E clk_idx_curs;
extern clock_time_setting_cursor_E clk_time_curs;
extern bool clk_time_curs_enable;