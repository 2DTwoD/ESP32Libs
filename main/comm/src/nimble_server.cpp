#include <host/util/util.h>
#include "nimble_server.h"

NimBleServer::NimBleServer():
uuid1{
    {BLE_UUID_TYPE_16},
    UUID_READ
},
uuid2{
    {BLE_UUID_TYPE_16},
    UUID_WRITE
},
uuid3{
    {BLE_UUID_TYPE_16},
    UUID_PRIMARY
},
bleGatt1{(ble_uuid_t*)&uuid1,
         device_read,
         {},
         {},
         BLE_GATT_CHR_F_READ,
         {},
         {},
         {}
},
bleGatt2{(ble_uuid_t*)&uuid2,
         device_write,
         {},
         {},
         BLE_GATT_CHR_F_WRITE,
         {},
         {},
         {}
}{
    bleGatts[0] = bleGatt1;
    bleGatts[1] = bleGatt2;
    bleGatts[2] = {};
    gattSvcs[0] = {
            BLE_GATT_SVC_TYPE_PRIMARY,
            (ble_uuid_t*)&uuid3,
            {},
            bleGatts
    };
    gattSvcs[1] = {};
    //Initialize NVS flash using
    nvs_flash_init();
    //Initialize the host stack
    nimble_port_init();
    //Initialize NimBLE configuration - server name
    ble_svc_gap_device_name_set("esp32nimble");
    // Initialize NimBLE configuration - gap service
    ble_svc_gap_init();
    //Initialize NimBLE configuration - gatt service
    ble_svc_gatt_init();
    //Initialize NimBLE configuration - config gatt services
    ble_gatts_count_cfg(gattSvcs);
    //Initialize NimBLE configuration - queues gatt services.
    ble_gatts_add_svcs(gattSvcs);
    //Initialize application
    ble_hs_cfg.sync_cb = ble_app_on_sync;
    //Run the thread
    nimble_port_freertos_init(host_task);
}

int NimBleServer::device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    os_mbuf_append(ctxt->om, "Data from the server", strlen("Data from the server"));
    return 0;
}

int NimBleServer::device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    printf("Data from the client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

void NimBleServer::ble_app_on_sync()  {
    // Determines the best address type automatically
    ble_hs_id_infer_auto(0, &ble_addr_type);
    // Define the BLE connection
    ble_app_advertise();
}

void NimBleServer::host_task(void *param)  {
    // This function will return only when nimble_port_stop() is executed
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void NimBleServer::ble_app_advertise() {
    // GAP - device name definition
    ble_hs_adv_fields fields{};
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    // Read the BLE device name
    device_name = ble_svc_gap_device_name();
    fields.name = (uint8_t *) device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.uuids16 = (ble_uuid16_t[]) {
            BLE_UUID16_INIT(UUID_PRIMARY)
    };
    fields.num_uuids16 = 1;
    fields.uuids16_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    ble_gap_adv_params adv_params{};
    memset(&adv_params, 0, sizeof(adv_params));
    // connectable or non-connectable
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    // discoverable or non-discoverable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    ble_gap_adv_start(ble_addr_type, nullptr, BLE_HS_FOREVER,
                      &adv_params, ble_gap_event, nullptr);

}

int NimBleServer::ble_gap_event(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        // Advertise if connected
        case BLE_GAP_EVENT_CONNECT:
            ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
            if (event->connect.status != 0) {
                ble_app_advertise();
            }
            break;
        // Advertise again after completion of the event
        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGI("GAP", "BLE GAP EVENT");
            ble_app_advertise();
            break;
    }
    return 0;
}
