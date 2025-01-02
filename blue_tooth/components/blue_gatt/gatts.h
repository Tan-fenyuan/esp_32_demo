#ifndef _GATTS_H
#define _GATTS_H

#include "../utils/utils.h"
#include "esp_bt_device.h"
#include "esp_gatt_common_api.h"
#include "esp_bt_main.h"
#include "esp_log.h"
#include "esp_gap_ble_api.h"
#include "esp_gap_bt_api.h"
#include "ble_gap.h"
#include "esp_gattc_api.h"
#include "esp_gatts_api.h"
#include "esp_bt.h"

struct gatts_profile_inst
{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
    esp_bd_addr_t remote_bda;
};



class gatts
{
private:
    gatts_profile_inst heart_rate_profile_tab[1]; // 蓝牙配置回调
    esp_ble_adv_data_t adv_data;                  // 广播设置
    uint8_t service_uuid[16];                     // 服务器配置UUID


public:
    gatts(/* args */);
    ~gatts();

public:
    void _init_gatts();
    void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

private:
    void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
 
};

#endif