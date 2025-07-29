#include <esp_log.h>
#include "updater.h"
#include "common.h"

////Deprecated
//void Updater::initTimer() {
//    gptimer_handle_t timer{nullptr};//make as field
//    updateList = new ArrayList<IUpdated1ms*>(nullptr);
//    //Настройка таймера
//    gptimer_config_t timerConfig;
//    timerConfig.clk_src = GPTIMER_CLK_SRC_DEFAULT;
//    timerConfig.direction = GPTIMER_COUNT_UP;
//    timerConfig.resolution_hz = 1000000;
//    timerConfig.intr_priority = 0;
//    gptimer_new_timer(&timerConfig, &timer);
//
//    //Событие переполнения
//    gptimer_event_callbacks_t cbs;
//    cbs.on_alarm = timerCallBack;
//    gptimer_register_event_callbacks(timer, &cbs, nullptr);
//
//    gptimer_enable(timer);
//
//    gptimer_alarm_config_t alarm_config;
//    alarm_config.alarm_count = 1000;
//    alarm_config.reload_count = 0;
//    alarm_config.flags.auto_reload_on_alarm = true;
//    gptimer_set_alarm_action(timer, &alarm_config);
//    //Запуск таймера
//    start();
//}
//
////Deprecated
//bool IRAM_ATTR Updater::timerCallBack(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *userData) {
//    if(Updater::updateList == nullptr) return false;
//    Updater::updateList->forEach([](auto obj) {
//        obj->update1ms();
//    });
//    return true;
//}
//void Updater::start() {
//    gptimer_start(timer);
//}
//
//void Updater::stop() {
//    gptimer_stop(timer);
//}
//
//void Updater::reset() {
//    gptimer_set_raw_count(timer, 0);
//}

IUpdated1ms::IUpdated1ms() {
    Updater::addObj(this);
}

void Updater::addObj(IUpdated1ms *obj) {
    if(xSemaphoreTake(updaterMutex, portMAX_DELAY) == pdTRUE){
        if(updateList->isEmpty()){
            if(!start()) {
                ESP_LOGI("Updater", "Updater was not started! Task creation problem");
                return;
            }
            ESP_LOGI("Updater", "Updater started!");
        }
        updateList->add(obj);
        xSemaphoreGive(updaterMutex);
    }
}

void Updater::update() {
    Updater::updateList->forEach([](auto obj) {
        obj->update1ms();
    });
}

bool Updater::start() {
    return xTaskCreate(Updater::updaterTask, "updTask", UPDATER_STACK_SIZE,
                       nullptr, UPDATER_TASK_PRIORITY, nullptr) == pdPASS;
}

void Updater::updaterTask(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while(true){
        if(xSemaphoreTake(updaterMutex, portMAX_DELAY) == pdTRUE){
            Updater::update();
            xSemaphoreGive(updaterMutex);
        }
        OsDelayUntil(&xLastWakeTime, 1);
    }
}
