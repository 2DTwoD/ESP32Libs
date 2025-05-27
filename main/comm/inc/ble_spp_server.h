#ifndef BLE_SPP_SERVER_H
#define BLE_SPP_SERVER_H
#include "store/config/ble_store_config.h"
#ifdef __cplusplus
extern "C" {
#endif

/* 16 Bit SPP Service UUID */
#define BLE_SVC_SPP_UUID16                                  0xABF0

/* 16 Bit SPP Service Characteristic UUID */
#define BLE_SVC_SPP_CHR_UUID16                              0xABF1

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;
void
go_ble_spp(void);
#ifdef __cplusplus
}
#endif
#endif //BLE_SPP_SERVER_H
