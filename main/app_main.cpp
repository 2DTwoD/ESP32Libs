#include <esp_log.h>
#include "coil.h"
#include "simple_input.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/gptimer.h"
extern "C"{

static bool IRAM_ATTR gptimer_alarm_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_data)
{
    ESP_LOGI("from timer", "tick");
    return true;
}

void app_main(void) {
//    Coil coil(2);
//    SimpleInput simpleInput(13);
    gptimer_handle_t gptimer = nullptr;
    gptimer_config_t timer_config;
    timer_config.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    timer_config.direction = GPTIMER_COUNT_UP;
    timer_config.resolution_hz = 1000000;
    timer_config.intr_priority = 0;

    gptimer_new_timer(&timer_config, &gptimer);

    gptimer_event_callbacks_t cbs = {
            gptimer_alarm_callback,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, nullptr);

    gptimer_enable(gptimer);

    gptimer_alarm_config_t alarm_config = {
            1000000,
            0,
            {true},
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_start(gptimer);

    while(1){
//        coil.toggle();
        //printf("current input 13 state: %d\n", simpleInput.isActive());
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
}
