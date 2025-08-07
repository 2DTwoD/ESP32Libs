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
WiFiTcpServer wifiServer(WIFI_STA_AP_TYPE, "virRout", "12345678", 3333);
extern "C" void app_main(void) {
    onDelay = true;
    onDelay2 = true;
    while(true){
        if(onDelay.get()){
            coil.toggle();
            onDelay.again();
        }
        if(onDelay2.get()){
            ESP_LOGI("OPA!", "Время пришло!");
            onDelay2.again();
            wifiServer.printAPipMask();
            wifiServer.printSTAipMask();
        }
        OsDelay(1);
    }
}
