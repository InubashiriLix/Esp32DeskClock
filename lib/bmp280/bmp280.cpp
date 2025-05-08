#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp; // I2C

float bmp280Temperature = 0.0f;
float bmp280Pressure = 0.0f;
float bmp280Altitude = 0.0f;

bool bmp280Init = false; 

void bmp280Setup()
{
    Wire.begin(26, 25); // SDA=21, SCL=22
    if (!bmp.begin(0x76))
    {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        bmp280Init = false;
        return;
    }
    bmp.setSampling(
        Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
        Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
        Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
        Adafruit_BMP280::FILTER_X16,      /* Filtering. */
        Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    bmp280Init = true;
    Serial.println("BMP280 initialization done!");
}

void bmp280UpdateTask(void *pvParameters)
{
    for (;;)
    {
        if (bmp280Init)
        {
            bmp280Temperature = bmp.readTemperature();
            bmp280Pressure = bmp.readPressure() / 100.0F;
            bmp280Altitude = bmp.readAltitude(1013.25F);

            Serial.printf("Temp: %.2f °C\n", bmp280Temperature);
            Serial.printf("Pressure: %.2f hPa\n", bmp280Pressure);
            Serial.printf("Approx. Altitude: %.2f m\n\n", bmp280Altitude);
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
