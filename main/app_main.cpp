#include <esp_log.h>
#include <esp_adc/adc_continuous.h>
#include <hal/uart_types.h>
#include <driver/uart.h>
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
#include "ble_server.h"


//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);
//OnDelay onDelay(100);
//OnDelay onDelay2(100000);
//Coil coil(2);

extern "C" void app_main(void) {
    BleServer bleServer("esp32ble");
    while(1){
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
