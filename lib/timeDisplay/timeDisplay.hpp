#pragma once

extern int year ;
extern int month;
extern int day;
extern int hour;
extern int minute;
extern int second;

void updateDateTimeItqTask(void *pvParameters); // Task to update date and time
void displayTimeTask(void *pvParameters);