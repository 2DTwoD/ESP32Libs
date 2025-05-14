#include <esp_log.h>
#include <esp_adc/adc_continuous.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "simple_input.h"
#include "on_delay.h"
#include "pulse.h"
#include "off_delay.h"
#include "simple_input_delayed.h"
#include "task_common.h"
#include "analog_monitor.h"
#include "updater.h"

#include "wifi_ap_tcp_server.h"


//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);

AnalogMonitor analogMonitor(ADC_UNIT_1, ADC_CHANNEL_7);
OnDelay onDelay(100);
Coil coil(2);

extern "C" void app_main(void) {
    onDelay = true;
    Updater::start();
    while(1){
        if(onDelay.get()){
            coil.toggle();
            onDelay.again();
        }
        ESP_LOGI("ADC1", "percent: %f", analogMonitor.get());
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


