#ifndef NIMBLE_SERVER_H
#define NIMBLE_SERVER_H

#include <cstdio>

#include <cstring>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "sdkconfig.h"

#define BLE_UUID16_INIT_OWN(uuid16)     \
{                                       \
    {                                   \
        BLE_UUID_TYPE_16                \
    },                                  \
    uuid16                              \
}

class NimBleServer{
private:
    // Array of pointers to other service definitions
    // UUID - Universal Unique Identifier
    ble_uuid16_t uuid1;
    ble_uuid16_t uuid2;
    ble_uuid16_t uuid3;
    ble_gatt_chr_def bleGatt1;
    ble_gatt_chr_def bleGatt2;
    ble_gatt_chr_def bleGatts[3];
    ble_gatt_svc_def gattSvcs[2];
    static inline uint8_t ble_addr_type{0};
    // Read data from ESP32 defined as server
    static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
    // Write data to ESP32 defined as server
    static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
    // The application
    static void ble_app_on_sync();
    // The infinite task
    static void host_task(void *param);
    // Define the BLE connection
    static void ble_app_advertise();
    // BLE event handling
    static int ble_gap_event(struct ble_gap_event *event, void *arg);
public:
    NimBleServer();
};

#endif //NIMBLE_SERVER_H
