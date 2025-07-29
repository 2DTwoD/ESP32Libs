#ifndef UPDATER_H
#define UPDATER_H

#include <esp_attr.h>
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "array_list.h"

#define UPDATER_STACK_SIZE (1024)
#define UPDATER_TASK_PRIORITY 10

class IUpdated1ms {
public:
    IUpdated1ms();

public:
    virtual void update1ms() = 0;
};

class IUpdatedSomewhere {
public:
    virtual void updateSomewhere() = 0;
};

class Updater{
private:
    inline static auto updateList = new ArrayList<IUpdated1ms*>(nullptr);

    inline static SemaphoreHandle_t updaterMutex = xSemaphoreCreateMutex();

    static void updaterTask(void *pvParameters);

    static bool start();

    static void update();

public:
    static void addObj(IUpdated1ms* obj);

};



#endif //UPDATER_H
