#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "simple_input.h"
#include "on_delay.h"
#include "pulse.h"
#include "off_delay.h"
#include "simple_input_delayed.h"
#include "task_common.h"

#include "wifi_ap_tcp_server.h"

extern "C" void app_main(void) {
    WiFiApTcpServer tcpServer("hiwifi", "12345678");
    Coil coil(2);
    SimpleInputDelayed simpleInput(13, 1000);
    Pulse pulse(2000);
    xTaskCreate(testTask, "testTask", configMINIMAL_STACK_SIZE * 20, nullptr,
                tskIDLE_PRIORITY + 1, nullptr);
    while(1){
        pulse = simpleInput.isActive();
        coil = pulse.get();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
