#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <cstdint>
#include <host/ble_gatt.h>

#include "array_list.h"

enum BleAccess{
    BLE_READ = 0,
    BLE_WRITE = 1,
    BLE_RW = 2
};

enum BleAttrType{
    BLE_TYPE_END =  BLE_GATT_SVC_TYPE_END,
    BLE_TYPE_PRIMARY = BLE_GATT_SVC_TYPE_PRIMARY,
    BLE_TYPE_SECONDARY = BLE_GATT_SVC_TYPE_SECONDARY
};

class BleAttribute{
private:
    ble_uuid16_t uuid;

public:
    explicit BleAttribute(uint16_t uuid);

    uint16_t getUUID() const;

    ble_uuid16_t* getUUIDptr();

};

class BleCharacteristic: public BleAttribute{
private:
    uint8_t *data;

    uint8_t dataLen;

    ble_gatt_chr_flags flags;

    uint16_t handle;

public:
    BleCharacteristic(uint16_t uuid, BleAccess access, uint8_t dataLen);

    virtual ~BleCharacteristic();

    ble_gatt_chr_flags getFlags() const;

    uint16_t getHandle() const;

    uint16_t* getHandlePtr();

    void getData(uint8_t* buffer, uint8_t len);

    void setData(uint8_t* newData, uint8_t newDataLen);

    uint8_t* getDataPtr();

    uint8_t getDataLen() const;
};

class BleService: public BleAttribute{
private:
    BleAttrType type;

    ble_gatt_access_fn *accessCB;

    ArrayList<BleCharacteristic*> characteristics{nullptr, 2};

    ble_gatt_chr_def *characteristicsArray{nullptr};

    BleCharacteristic *getCharacteristicByCondition(std::function<bool(BleCharacteristic*)> conditionLambda);
public:
    BleService(uint16_t uuid, BleAttrType type, ble_gatt_access_fn *accessCB);

    explicit BleService(uint16_t uuid, ble_gatt_access_fn *accessCB);

    virtual ~BleService();

    void addCharacteristic(uint16_t uuid, BleAccess access, uint8_t dataLen);

    BleCharacteristic* getCharacteristicByHandle(uint16_t handle);

    BleCharacteristic* getCharacteristicByUUID(uint16_t uuid);

    ble_gatt_chr_def* getCharacteristicsArray();

    bool uuidIsExist(uint16_t uuid);

    BleAttrType getType() const;
};

class BleServer{
private:
    inline static ArrayList<BleService*> services{nullptr, 1};

    const char* name;

    bool startFlag{false};

    ble_gatt_svc_def *gattSvcs{};

    inline static uint8_t own_addr_type{0};

    inline static uint8_t addr_val[6]{0};

    static void on_stack_reset(int reason);

    static void on_stack_sync();

    static void start_advertising();

    static int gap_event_handler(struct ble_gap_event *event, void *arg);

    static void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

    static void nimble_host_task(void *param);

    static void gatt_svr_subscribe_cb(struct ble_gap_event *event);

    static int attributeAccess(uint16_t entity, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

    static bool checkCondition(std::function<bool(BleService*)> conditionLambda);

    static bool checkUUIDisExist(uint16_t uuid);

    static BleService* getService(uint16_t uuid);

    static BleCharacteristic* getCharecteristic(uint16_t serviceUUID, uint16_t characteristicUUID);

public:
    explicit BleServer(const char* name);

    virtual ~BleServer();

    void start();

    void addService(BleAttrType type, uint16_t uuid);

    void addCharacteristic(uint16_t serviceUUID, uint16_t characteristicUUID, BleAccess access, uint8_t dataLen);

    static bool read(uint16_t serviceUUID, uint16_t characteristicUUID, uint8_t *bytes, uint16_t len);

    static bool write(uint16_t serviceUUID, uint16_t characteristicUUID, uint8_t *const bytes, uint16_t len);
};

#endif //BLE_SERVER_H
