#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <cstdint>
#include <host/ble_gatt.h>

#define UUID_READ 0xFEF4
#define UUID_WRITE 0xDEAD
#define UUID_PRIMARY 0x180

class BleServer{
private:
    ble_uuid16_t uuid1;
    ble_uuid16_t uuid2;
    ble_uuid16_t uuid3;
    ble_gatt_chr_def bleGatt1;
    ble_gatt_chr_def bleGatt2;
    ble_gatt_chr_def bleGatts[3];
    ble_gatt_svc_def gattSvcs[2];
    inline static uint8_t own_addr_type{0};
    inline static uint8_t addr_val[6]{0};
    static void on_stack_reset(int reason);
    static void on_stack_sync();
    static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
    static void start_advertising();
    static int gap_event_handler(struct ble_gap_event *event, void *arg);
    static void nimble_host_task(void *param);
    static void gatt_svr_subscribe_cb(struct ble_gap_event *event);
    static int device_read(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
    static int device_write(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
public:
    explicit BleServer(const char* name);
};

#endif //BLE_SERVER_H
