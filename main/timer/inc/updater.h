#ifndef UPDATER_H
#define UPDATER_H

#include <esp_attr.h>
#include "driver/gptimer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "array_list.h"

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
    static void updaterTask(void *pvParameters);
    static inline SemaphoreHandle_t updaterMutex = xSemaphoreCreateMutex();
    static bool start();
    static bool update();
public:
    static void addObj(IUpdated1ms* obj);
};



#endif //UPDATER_H
