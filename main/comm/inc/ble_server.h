#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <cstdint>
#include <host/ble_gatt.h>

#include "i_comm_driver.h"
#include "array_list.h"

enum BleAccess{
    BLE_READ = 0,
    BLE_WRITE = 1,
    BLE_RW = 2
};

struct BleAttribute{
    char name[8];
    ble_uuid16_t uuid;
    BleAccess access;
    uint8_t *data;
    uint8_t len;
    uint16_t handle;
};

class BleServer: public ICommRW{
private:
    inline static ArrayList<BleAttribute*> attributes{nullptr};
    const char* name;

    inline static uint8_t count{0};
    ble_uuid16_t primaryUUID{};
    ble_gatt_chr_def *bleGatts{};
    ble_gatt_svc_def gattSvcs[2]{};
    inline static uint8_t own_addr_type{0};
    inline static uint8_t addr_val[6]{0};

    static void on_stack_reset(int reason);
    static void on_stack_sync();
    static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
    static void start_advertising();
    static int gap_event_handler(struct ble_gap_event *event, void *arg);
    static void nimble_host_task(void *param);
    static void gatt_svr_subscribe_cb(struct ble_gap_event *event);
    static int attributeAccess(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);
public:
    BleServer(const char* name);

    virtual ~BleServer();

    void start();

    void addAttribute(const char* name,uint16_t uuid, BleAccess access, uint8_t dataLen);

    CommStatus read(uint8_t *bytes, uint16_t len) override;

    CommStatus write(uint8_t *const bytes, uint16_t len) override;
};

#endif //BLE_SERVER_H
