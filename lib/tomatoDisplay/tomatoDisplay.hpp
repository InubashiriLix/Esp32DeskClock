#pragma once

void tomatoTimerTask(void *pvParameters);
void tomatoDisplayTask(void *pvParameters);

enum class tomatoStatus_e {
  STOPPED,
  RUNNING,
  RELAXATION,
};

void startTomato();
extern volatile tomatoStatus_e tomatoStatus;
extern volatile bool time_up;
extern volatile int tomato_cmp_cnt;