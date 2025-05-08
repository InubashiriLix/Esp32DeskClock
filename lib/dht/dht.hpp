#pragma once

#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 15
#define DHTTYPE DHT11

extern DHT dht;
extern float dht_tempe;
extern float dht_humi;
extern bool dhtSuccess; // Flag to indicate if DHT sensor is initialized successfully

void dhtSetup();
void dhtUpdateTask(void *pvParameters); // Task to update temperature and humidity