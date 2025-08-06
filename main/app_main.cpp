#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "on_delay.h"

#include "common.h"
#include "buffer.h"
#include "wifi_tcp_server.h"


OnDelay onDelay(100);
OnDelay onDelay2(1000);
Coil coil(2);

extern "C" void app_main(void) {
    onDelay = true;
    onDelay2 = true;
    Buffer buffer(10);
//    uint8_t count = 0;
    WiFiTcpServer wifiServer(WIFI_AP_TYPE, "virRout", "12345678", 3333);
    wifiServer.start();
    while(true){
        if(onDelay.get()){
            coil.toggle();
            onDelay.again();
        }
//        if(onDelay2.get()){
//            buffer.addByte(count++);
//            ESP_LOGI("OPA!", "Время пришло!");
//            onDelay2.again();
//            ESP_LOGI("Buffer", "value: %d", buffer.getAndClearByte());
//        }
        OsDelay(1);
    }
}
