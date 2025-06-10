#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <cstdint>
#include <host/ble_gatt.h>

#include "i_comm_driver.h"

struct BLEParams{
    const char* name{"BLEsp32"};
    uint16_t sendBuffSize{128};
    uint16_t receiveBuffSize{128};
};

class BleServer: public ICommRW{
private:
    ble_uuid16_t readUUID;
    ble_uuid16_t writeUUID;
    ble_uuid16_t primaryUUID;
    ble_gatt_chr_def bleGatt1;
    ble_gatt_chr_def bleGatt2;
    ble_gatt_chr_def bleGatts[3]{};
    ble_gatt_svc_def gattSvcs[2]{};
    inline static uint16_t sendBuffSize{0};
    inline static uint16_t receiveBuffSize{0};
    inline static uint8_t *sendBuff{nullptr};
    inline static uint8_t *receiveBuff{nullptr};
    uint16_t readAttrHandle;
    uint16_t writeAttrHandle;
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
    BleServer(const char* name, uint16_t sendBuffSize, uint16_t receiveBuffSize);

    virtual ~BleServer();

    CommStatus read(uint8_t *bytes, uint16_t len) override;

    CommStatus write(uint8_t *const bytes, uint16_t len) override;
};

#endif //BLE_SERVER_H
