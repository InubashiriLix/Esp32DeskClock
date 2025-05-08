#include <Arduino.h>
#include "rgb.hpp"

#define PWM_CH_R 2
#define PWM_CH_G 3
#define PWM_CH_B 4
const int PWM_FREQ = 1000;
const int PWM_RES = 8; // 0–255

bool rgbON = true;

void rgbSetup()
{
    

    ledcSetup(PWM_CH_G, PWM_FREQ, PWM_RES);
    ledcAttachPin(RGB_CHAN_G, PWM_CH_G);

    ledcSetup(PWM_CH_B, PWM_FREQ, PWM_RES);
    ledcAttachPin(RGB_CHAN_B, PWM_CH_B);
}

void rgbRunTask(void *pv)
{
    const TickType_t dt = pdMS_TO_TICKS(10);
    const uint16_t M = (1 << PWM_RES) - 1; // 255
    for (;;)
    {
        if (rgbON)
        {
            for (int d = 0; d < (1 << PWM_RES); d += 4)
            {
                ledcWrite(PWM_CH_R, d);
                vTaskDelay(dt);
            }
            for (int d = 0; d < (1 << PWM_RES); d += 4)
            {
                ledcWrite(PWM_CH_G, d);
                vTaskDelay(dt);
            }
            for (int d = 0; d < (1 << PWM_RES); d += 4)
            {
                ledcWrite(PWM_CH_B, d);
                vTaskDelay(dt);
            }

            for (int d = (1 << PWM_RES); d >= 0; d -= 4)
            {
                ledcWrite(PWM_CH_R, d);
                vTaskDelay(dt);
            }
            for (int d = (1 << PWM_RES); d >= 0; d -= 4)
            {
                ledcWrite(PWM_CH_G, d);
                vTaskDelay(dt);
            }
            for (int d = (1 << PWM_RES); d >= 0; d -= 4)
            {
                ledcWrite(PWM_CH_B, d);
                vTaskDelay(dt);
            }
        }
        else
        {
            ledcWrite(PWM_CH_R, 0);
            ledcWrite(PWM_CH_G, 0);
            ledcWrite(PWM_CH_B, 0);
        }
    }
}