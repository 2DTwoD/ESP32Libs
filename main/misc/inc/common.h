#ifndef COMMON_H
#define COMMON_H

#define OsDelay(TIME_MS) vTaskDelay(TIME_MS / portTICK_PERIOD_MS)
#define OsDelayUntil(LAST_WAKE_TIME, TIME_MS) vTaskDelayUntil(LAST_WAKE_TIME, TIME_MS / portTICK_PERIOD_MS)

#endif //COMMON_H
