#ifndef _BLUE_TOOTH_H
#define _BLUE_TOOTH_H

#include "gattc.h"
#include "gatts.h"

class blue_tooth
{
private:
    uint8_t adv_config_done;

public:
    blue_tooth(/* args */);
    ~blue_tooth();

public:
    /// @brief 初始化蓝牙
    void init_blue_tooth();
    /// @brief 释放蓝牙
    void destory_blue_tooth();

private:
    void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
};

#endif