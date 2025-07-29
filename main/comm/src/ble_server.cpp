#include "ble_server.h"

#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include <nimble/nimble_port.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>
#include <host/ble_gatt.h>
#include <host/util/util.h>
#include "host/ble_hs.h"
#include "array_fun.h"


#define TAG "BLE_server"

extern "C" void ble_store_config_init(void);

SemaphoreHandle_t bleMutex{};

//BleAttribute

BleAttribute::BleAttribute(uint16_t uuid) : uuid({BLE_UUID_TYPE_16}, uuid) {}

uint16_t BleAttribute::getUUID() const {
    return uuid.value;
}

ble_uuid16_t *BleAttribute::getUUIDptr() {
    return &uuid;
}


//BleCharacteristic

BleCharacteristic::BleCharacteristic(uint16_t uuid, BleAccess access, uint8_t dataLen):
        BleAttribute(uuid), data(new uint8_t[dataLen]), dataLen(dataLen), handle(0){
    switch (access) {
        case BLE_READ:
            flags = BLE_GATT_CHR_F_READ;
            break;
        case BLE_WRITE:
            flags = BLE_GATT_CHR_F_WRITE;
            break;
        case BLE_RW:
            flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE;
            break;
    }
    memset(data, 0, dataLen);
}

BleCharacteristic::~BleCharacteristic() {
    delete[] data;
}

ble_gatt_chr_flags BleCharacteristic::getFlags() const {
    return flags;
}

uint16_t *BleCharacteristic::getHandlePtr() {
    return &handle;
}

uint16_t BleCharacteristic::getHandle() const {
    return handle;
}


void BleCharacteristic::getData(uint8_t *buffer, uint8_t len) {
    if(xSemaphoreTake(bleMutex, portMAX_DELAY) == pdTRUE){
        memcpy(buffer, data, len > dataLen? dataLen: len);
        xSemaphoreGive(bleMutex);
    } else {
        ESP_LOGW(TAG, "Data was not read, mutex problem");
    }
}

void BleCharacteristic::setData(uint8_t *newData, uint8_t newDataLen) {
    if(xSemaphoreTake(bleMutex, portMAX_DELAY) == pdTRUE){
        memset(data, 0, dataLen);
        memcpy(data, newData, newDataLen > dataLen? dataLen: newDataLen);
        xSemaphoreGive(bleMutex);
    } else {
        ESP_LOGW(TAG, "Data was not wrote, mutex problem");
    }
}

uint8_t *BleCharacteristic::getDataPtr() {
    return data;
}

uint8_t BleCharacteristic::getDataLen() const {
    return dataLen;
}


//BleService

BleService::BleService(uint16_t uuid, BleAttrType type, ble_gatt_access_fn *accessCB):
        BleAttribute(uuid), type(type), accessCB(accessCB) {}

BleService::BleService(uint16_t uuid, ble_gatt_access_fn *accessCB):
        BleService(uuid, BLE_TYPE_PRIMARY, accessCB) {}

BleService::~BleService() {
    characteristics.forEach([](BleCharacteristic *characteristic){
        delete characteristic;
    });
    delete[] characteristicsArray;
}

void BleService::addCharacteristic(uint16_t uuid, BleAccess access, uint8_t dataLen) {
    auto characteristic = new BleCharacteristic(uuid, access, dataLen);
    characteristics.add(characteristic);
}

BleCharacteristic *BleService::getCharacteristicByCondition(std::function<bool(BleCharacteristic *)> conditionLambda) {
    BleCharacteristic *targetCharacteristic{nullptr};
    characteristics.forEachBreak([&targetCharacteristic, &conditionLambda](BleCharacteristic *characteristic){
        if(conditionLambda(characteristic)){
            targetCharacteristic = characteristic;
            return true;
        }
        return false;
    });
    return targetCharacteristic;
}

BleCharacteristic *BleService::getCharacteristicByHandle(uint16_t handle) {
    return getCharacteristicByCondition([&handle](BleCharacteristic *characteristic){
        return characteristic->getHandle() == handle;
    });
}

BleCharacteristic *BleService::getCharacteristicByUUID(uint16_t uuid) {
    return getCharacteristicByCondition([&uuid](BleCharacteristic *characteristic){
        return characteristic->getUUID() == uuid;
    });;
}

ble_gatt_chr_def* BleService::getCharacteristicsArray() {
    if(characteristics.isEmpty()) return nullptr;
    characteristicsArray = new ble_gatt_chr_def[characteristics.size() + 1];
    uint8_t index = 0;
    characteristics.forEach([this, &index](BleCharacteristic* characteristic){
        characteristicsArray[index++] = {
                (ble_uuid_t*)characteristic->getUUIDptr(),
                accessCB,
                {},
                {},
                characteristic->getFlags(),
                {},
                characteristic->getHandlePtr(),
                {}
        };
    });
    characteristicsArray[index] = {};
    return characteristicsArray;
}

bool BleService::uuidIsExist(uint16_t uuid) {
    bool result = false;
    if(getUUID() == uuid) return true;
    characteristics.forEachBreak([&result, &uuid](BleCharacteristic *characteristic){
        if(characteristic->getUUID() == uuid){
            result = true;
        }
        return result;
    });
    return result;
}

BleAttrType BleService::getType() const {
    return type;
}


//BleServer

BleServer::BleServer(const char* name): name(name){}

BleServer::~BleServer() {
    services.forEach([](BleService *service){
        delete service;
    });
    delete[] gattSvcs;
}

void BleServer::on_stack_reset(int reason) {
    // On reset, print reset reason to console
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}

void BleServer::on_stack_sync() {
    // Make sure we have proper BT identity address set (random preferred)
    if (ble_hs_util_ensure_addr(0) != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    // Figure out BT address to use while advertising (no privacy for now)
    if (ble_hs_id_infer_auto(0, &own_addr_type) != 0) {
        ESP_LOGE(TAG, "failed to infer address type");
        return;
    }
    if (ble_hs_id_copy_addr(own_addr_type, addr_val, nullptr) != 0) {
        ESP_LOGE(TAG, "failed to copy device address");
        return;
    }
    /* Start advertising. */
    start_advertising();
}

void BleServer::start_advertising() {
    const char *name;
    ble_hs_adv_fields adv_fields{};
    ble_hs_adv_fields rsp_fields{};
    ble_gap_adv_params adv_params{};

    // Set advertising flags
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    // Set device name
    name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t *)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    // Set device tx power
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.tx_pwr_lvl_is_present = 1;

    // Set advertiement fields
    if (ble_gap_adv_set_fields(&adv_fields) != 0) {
        ESP_LOGE(TAG, "failed to set advertising data");
        return;
    }

    // Set device address
    rsp_fields.device_addr = addr_val;
    rsp_fields.device_addr_type = own_addr_type;
    rsp_fields.device_addr_is_present = 1;


    // Set advertising interval
    rsp_fields.adv_itvl = BLE_GAP_ADV_ITVL_MS(500);
    rsp_fields.adv_itvl_is_present = 1;

    // Set scan response fields
    if (ble_gap_adv_rsp_set_fields(&rsp_fields) != 0) {
        ESP_LOGE(TAG, "failed to set scan response data");
        return;
    }

    // Set non-connetable and general discoverable mode to be a beacon
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    // Set advertising interval
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(500);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(510);

    // Start advertising
    if (ble_gap_adv_start(own_addr_type, nullptr, BLE_HS_FOREVER, &adv_params,
                            gap_event_handler, nullptr) != 0) {
        ESP_LOGE(TAG, "failed to start advertising");
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

int BleServer::gap_event_handler(struct ble_gap_event *event, void *arg) {
    // Local variables
    int rc = 0;
    ble_gap_conn_desc desc{};

    // Handle different GAP event
    switch (event->type) {

        // Connect event
        case BLE_GAP_EVENT_CONNECT:
            // A new connection was established or a connection attempt failed
            ESP_LOGI(TAG, "connection %s; status=%d",
                     event->connect.status == 0 ? "established" : "failed",
                     event->connect.status);

            // Connection succeeded
            if (event->connect.status == 0) {
                // Check connection handle
                rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
                if (rc != 0) {
                    ESP_LOGE(TAG,
                             "failed to find connection by handle, error code: %d",
                             rc);
                    return rc;
                }

                // Try to update connection parameters
                ble_gap_upd_params params = {
                        desc.conn_itvl,
                        desc.conn_itvl,
                        3,
                        desc.supervision_timeout,
                        {},
                        {}
                };
                rc = ble_gap_update_params(event->connect.conn_handle, &params);
                if (rc != 0) {
                    ESP_LOGE(
                            TAG,
                            "failed to update connection parameters, error code: %d",
                            rc);
                    return rc;
                }
            } else {
                // Connection failed, restart advertising
                start_advertising();
            }
            return rc;

            // Disconnect event
        case BLE_GAP_EVENT_DISCONNECT:
            // A connection was terminated, print connection descriptor
            ESP_LOGI(TAG, "disconnected from peer; reason=%d",
                     event->disconnect.reason);

            // Restart advertising
            start_advertising();
            return rc;
            // Connection parameters update event
        case BLE_GAP_EVENT_CONN_UPDATE:
            // The central has updated the connection parameters
            ESP_LOGI(TAG, "connection updated; status=%d",
                     event->conn_update.status);

            // Print connection descriptor
            rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                         rc);
                return rc;
            }
            return rc;

            // Advertising complete event
        case BLE_GAP_EVENT_ADV_COMPLETE:
            // Advertising completed, restart advertising
            ESP_LOGI(TAG, "advertise complete; reason=%d",
                     event->adv_complete.reason);
            start_advertising();
            return rc;

            // Notification sent event
        case BLE_GAP_EVENT_NOTIFY_TX:
            if ((event->notify_tx.status != 0) &&
                (event->notify_tx.status != BLE_HS_EDONE)) {
                // Print notification info on error
                ESP_LOGI(TAG,
                         "notify event; conn_handle=%d attr_handle=%d "
                         "status=%d is_indication=%d",
                         event->notify_tx.conn_handle, event->notify_tx.attr_handle,
                         event->notify_tx.status, event->notify_tx.indication);
            }
            return rc;

            // Subscribe event
        case BLE_GAP_EVENT_SUBSCRIBE:
            // Print subscription info to log
            ESP_LOGI(TAG,
                     "subscribe event; conn_handle=%d attr_handle=%d "
                     "reason=%d prevn=%d curn=%d previ=%d curi=%d",
                     event->subscribe.conn_handle, event->subscribe.attr_handle,
                     event->subscribe.reason, event->subscribe.prev_notify,
                     event->subscribe.cur_notify, event->subscribe.prev_indicate,
                     event->subscribe.cur_indicate);

            // GATT subscribe event callback
            gatt_svr_subscribe_cb(event);
            return rc;

            // MTU update event
        case BLE_GAP_EVENT_MTU:
            // Print MTU update info to log
            ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                     event->mtu.conn_handle, event->mtu.channel_id,
                     event->mtu.value);
            return rc;
        case BLE_GAP_EVENT_REPEAT_PAIRING:
            /* We already have a bond with the peer, but it is attempting to
             * establish a new secure link.  This app sacrifices security for
             * convenience: just throw away the old bond and accept the new link.
             */

            /* Delete the old bond. */
            rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
            assert(rc == 0);
            ble_store_util_delete_peer(&desc.peer_id_addr);

            /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
             * continue with the pairing operation.
             */
            return BLE_GAP_REPEAT_PAIRING_RETRY;
    }
    return rc;
}

void BleServer::gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    // Local variables
    char buf[BLE_UUID_STR_LEN];

    // Handle GATT attributes register events
    switch (ctxt->op) {

        // Service register event
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGD(TAG, "registered service %s with handle=%d",
                     ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                     ctxt->svc.handle);
            break;

            // Characteristic register event
        case BLE_GATT_REGISTER_OP_CHR:
            ESP_LOGD(TAG,
                     "registering characteristic %s with "
                     "def_handle=%d val_handle=%d",
                     ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                     ctxt->chr.def_handle, ctxt->chr.val_handle);
            break;

            // Descriptor register event
        case BLE_GATT_REGISTER_OP_DSC:
            ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                     ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                     ctxt->dsc.handle);
            break;

            // Unknown event
        default:
            assert(0);
    }
}

void BleServer::nimble_host_task(void *param) {
    // Task entry log
    ESP_LOGI(TAG, "nimble host task has been started!");
    // This function won't return until nimble_port_stop() is executed
    nimble_port_run();
    // Clean up at exit
    vTaskDelete(nullptr);
}

void BleServer::gatt_svr_subscribe_cb(struct ble_gap_event *event)  {
    // Check connection handle
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    } else {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
    }
}


int BleServer::attributeAccess(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    switch (ctxt->op) {
        case BLE_GATT_ACCESS_OP_READ_CHR:
            services.forEachBreak([attr_handle, ctxt](BleService *service){
                BleCharacteristic *characteristic = service->getCharacteristicByHandle(attr_handle);
                if(characteristic != nullptr){
                    os_mbuf_append(ctxt->om, characteristic->getDataPtr(), characteristic->getDataLen());
                    return true;
                }
                return false;
            });
            break;
        case BLE_GATT_ACCESS_OP_WRITE_CHR:
            ESP_LOGI(TAG, "device_write: Data from the client: %.*s", ctxt->om->om_len, ctxt->om->om_data);
            services.forEachBreak([attr_handle, ctxt](BleService *service){
                BleCharacteristic *characteristic = service->getCharacteristicByHandle(attr_handle);
                if(characteristic != nullptr){
                    characteristic->setData(ctxt->om->om_data, ctxt->om->om_len);
                    return true;
                }
                return false;
            });
            break;
    }
    return 0;
}

bool BleServer::checkCondition(std::function<bool(BleService*)> conditionLambda) {
    bool matchFlag = false;
    services.forEachBreak([&conditionLambda, &matchFlag](BleService *service){
        if(conditionLambda(service)){
            matchFlag = true;
        }
        return matchFlag;
    });
    return matchFlag;
}

bool BleServer::checkUUIDisExist(uint16_t uuid) {
    if(checkCondition([&uuid](BleService* service){
        return service->uuidIsExist(uuid);
    })){
        ESP_LOGW(TAG, "Service or characteristic with UUID %X already exist", uuid);
        return true;
    }
    return false;
}

BleService *BleServer::getService(uint16_t uuid) {
    BleService* targetService{nullptr};
    if(!checkCondition([&uuid, &targetService](BleService* service){
        if(service->getUUID() == uuid){
            targetService = service;
            return true;
        }
        return false;
    })){
        ESP_LOGW(TAG, "Service with UUID %X not exist", uuid);
    }
    return targetService;
}

BleCharacteristic *BleServer::getCharecteristic(uint16_t serviceUUID, uint16_t characteristicUUID) {
    BleService* service = getService(serviceUUID);
    if(service == nullptr) return nullptr;
    BleCharacteristic* characteristic = service->getCharacteristicByUUID(characteristicUUID);
    if(characteristic == nullptr){
        ESP_LOGW(TAG, "Characteristic with UUID %X not exist", characteristicUUID);
    }
    return characteristic;
}

void BleServer::start() {
    if(startFlag){
        ESP_LOGW(TAG, "BLE server already started, can't start again");
        return;
    }
    bleMutex = xSemaphoreCreateMutex();
    startFlag = true;
    uint8_t index = 0;
    gattSvcs = new ble_gatt_svc_def[services.size() + 1];
    services.forEach([this, &index](BleService *service){
        gattSvcs[index++] = {
                (uint8_t)service->getType(),
                (ble_uuid_t*)service->getUUIDptr(),
                {},
                service->getCharacteristicsArray()
        };
    });
    gattSvcs[index] = {};
    esp_err_t ret;
    // NVS flash initialization
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }
    /* NimBLE stack initialization */
    if (nimble_port_init() != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ", ret);
        return;
    }
    /* Call NimBLE GAP initialization API */
    ble_svc_gap_init();
    /* Set GAP device name */
    if (ble_svc_gap_device_name_set(name) != 0) {
        ESP_LOGE(TAG, "failed to set device name to %s", name);
        return;
    }

    // GATT service initialization
    ble_svc_gatt_init();
    // Update GATT services counter and GATT services
    if (ble_gatts_count_cfg(gattSvcs) != 0 || ble_gatts_add_svcs(gattSvcs) != 0) {
        ESP_LOGE(TAG, "failed to initialize GATT server");
        return;
    }

    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    // Store host configuration
    ble_store_config_init();

    /* Start NimBLE host task thread and return */
    xTaskCreate(nimble_host_task, "NimBLEHost", 4*1024, nullptr, 5, nullptr);
}

void BleServer::addService(BleAttrType type, uint16_t uuid) {
    if(startFlag){
        ESP_LOGW(TAG, "Can't add service, BLE server already started");
        return;
    }
    if(checkUUIDisExist(uuid)) return;
    auto service = new BleService(uuid, type, attributeAccess);
    services.add(service);
}

void BleServer::addCharacteristic(uint16_t serviceUUID, uint16_t characteristicUUID, BleAccess access, uint8_t dataLen) {
    if(startFlag){
        ESP_LOGW(TAG, "Can't add characteristic, BLE server already started");
        return;
    }
    BleService* targetService{nullptr};
    if(!checkCondition([&serviceUUID, &targetService](BleService* service){
        if(service->getUUID() == serviceUUID){
            targetService = service;
            return true;
        }
        return false;
    })){
        ESP_LOGW(TAG, "Service with UUID %X not exist", serviceUUID);
        return;
    }
    if(checkUUIDisExist(characteristicUUID)) return;
    targetService->addCharacteristic(characteristicUUID, access, dataLen);
}

bool BleServer::read(uint16_t serviceUUID, uint16_t characteristicUUID, uint8_t *bytes, uint16_t len) {
    if(!startFlag){
        ESP_LOGW(TAG, "Can't read characteristic, the server is not running yet.");
        return false;
    }
    BleCharacteristic* characteristic = getCharecteristic(serviceUUID, characteristicUUID);
    if(characteristic == nullptr) return false;
    characteristic->getData(bytes, len);
    return true;
}

bool BleServer::write(uint16_t serviceUUID, uint16_t characteristicUUID, uint8_t *const bytes, uint16_t len) {
    if(!startFlag){
        ESP_LOGW(TAG, "Can't write characteristic, the server is not running yet.");
        return false;
    }
    BleCharacteristic* characteristic = getCharecteristic(serviceUUID, characteristicUUID);
    if(characteristic == nullptr) return false;
    characteristic->setData(bytes, len);
    return true;
}
