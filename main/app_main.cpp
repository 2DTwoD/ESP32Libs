#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "coil.h"
#include "on_delay.h"

#include "wifi_ap_tcp_server.h"
#include "ble_server.h"
#include "common.h"
#include "string_map.h"


//WiFiApTcpServer tcpServer("hiwifi", "12345678", 3333);
OnDelay onDelay(100);
OnDelay onDelay2(1000);
Coil coil(2);

extern "C" void app_main(void) {
    BleServer bleServer("esp32ble");
    bleServer.addService(BLE_TYPE_PRIMARY, 0xA000);
    bleServer.addCharacteristic(0xA000, 0xA001, BLE_RW, 10);
    bleServer.addCharacteristic(0xA000, 0xA002, BLE_RW, 15);
    bleServer.addService(BLE_TYPE_PRIMARY, 0xB000);
    bleServer.addCharacteristic(0xB000, 0xB001, BLE_RW, 10);
    bleServer.addCharacteristic(0xB000, 0xB002, BLE_RW, 5);
    bleServer.start();
    uint8_t dataWrite[1] = {1};
    uint8_t dataRead[1] = {1};

    StringMap<uint8_t> map(0);
    ArrayList<uint8_t> list(5);
    list.add(1);
    list.add(2);
    list.add(3);
    list.add(4);
    list.add(5);
    for (auto i : list){
        ESP_LOGI("list", "value: %d", i);
    }

    onDelay = true;
    onDelay2 = true;
    uint8_t count = 0;
    char name[10];
    while(true){
        if(onDelay.get()){
            coil.toggle();
            onDelay.again();
        }
        if(onDelay2.get()){
            ESP_LOGI("OPA!", "Время пришло!");
            onDelay2.again();

            if(map.isExist("opa4")){
                ESP_LOGI("OPA!", "OPA4 существует!");
            }
            memset(name, 0, 10);
            if(count >= 5){
                sprintf(name, "opa%d", 9 - count);
                map.remove(name);
            } else {
                sprintf(name, "opa%d", count);
                map.add(name, count);
            }
            map.forEach([](auto key, auto value){
                ESP_LOGI("OPA", "key: %s value: %d", key, value);
            });
            count++;
            if(count >= 10) {
                count = 0;
            }
        }
        bleServer.read(0xA000, 0xA001, dataRead, 1);
        dataWrite[0] = dataRead[0] + 1;
        bleServer.write(0xA000, 0xA001, dataWrite, 1);

        OsDelay(1);
    }
}
