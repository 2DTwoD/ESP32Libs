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
#include "adc_reader.h"

#include "wifi_ap_tcp_server.h"


//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);


extern "C" void app_main(void) {
    OnDelay onDelay(100);
    Coil coil(2);
    onDelay = true;
    AdcReader adcReader(ADC_UNIT_1, ADC_CHANNEL_7);

    while(1){
        if(onDelay.get()){
            coil.toggle();
            onDelay.again();
        }
        adcReader.updateSomewhere();
        ESP_LOGI("ADC1", "voltage: %d", adcReader.getDigits());
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}


