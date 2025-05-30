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
#include "analog_reader.h"
#include "updater.h"
#include "analog_writer_pwm.h"
#include "analog_writer_dac.h"

#include "wifi_ap_tcp_server.h"
//#include "nimble_server.h"
//#include "ble_spp_server.h"
#include "bt_classic_spp.h"


//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);
//OnDelay onDelay(100);
//OnDelay onDelay2(100000);
//Coil coil(2);

extern "C" void app_main(void) {
//    AnalogMonitor analogMonitor(ADC_UNIT_1, ADC_CHANNEL_7);
//    AnalogWriterPwmWithRamp pwmOutput(5, LEDC_CHANNEL_0, LEDC_TIMER_0);
//    AnalogWriterDacWithRamp analogWriterDac(DAC_CHAN_0);
//    onDelay = true;
//    onDelay2 = true;

//    NimBleServer nimBleServer;

//    Updater::start();
//    go_ble_spp();
    go_bt_classic_spp();
    while(1){
//        if(onDelay.get()){
//            coil.toggle();
//            onDelay.again();
//        }
//        analogWriterDac = analogMonitor.get();
//        pwmOutput = analogMonitor.get();
////        ESP_LOGI("DAC", "digits: %d", analogWriterDac.get());
//        ESP_LOGI("OnDelay2", "time: %lu", onDelay2.getCurrentTime());
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
