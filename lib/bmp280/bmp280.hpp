#pragma once

extern float bmp280Temperature; 
extern float bmp280Pressure;    
extern float bmp280Altitude;    

extern bool bmp280Init; 


void bmp280Setup();
void bmp280UpdateTask(void *pvParameters);