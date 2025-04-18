#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "simple_input.h"
#include "on_delay.h"
#include "pulse.h"

extern "C" void app_main(void) {
    Coil coil(2);
    SimpleInput simpleInput(13);
    OnDelay delay(1000);
    Pulse pulse(500);
    while(1){
        delay.set(true);
        pulse.set(true);
        if(delay.get()) {
            delay.reset();
//            ESP_LOGI("Delay", "expire");
        }
        if(!pulse.get()){
            pulse.reset();
            coil.toggle();
            ESP_LOGI("Pulse", "expire");
        }
        ESP_LOGI("Pulse", "currentTime %lu", pulse.getCurrentTime());
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
