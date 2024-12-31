
#include "gatts.h"


#ifdef GATTS_TABLE_TAG
#undef GATTS_TABLE_TAG
#define GATTS_TABLE_TAG "tfy:gatts"
#else
#define GATTS_TABLE_TAG "tfy:gatts"
#endif

#ifdef GATTS_LOGI
#undef GATTS_LOGI
#define GATTS_LOGI "TFY:LOGI"
#else
#define GATTS_LOGI "TFY:LOGI"
#endif

#define APP_ID 0x55

gatts::gatts()
{
    this->heart_rate_profile_tab[0] = {
        .gatts_cb = this->gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    };
    this->adv_data=
    {
        .set_scan_rsp =false,   // false 普通广播包  true 扫描响应包
        .include_name = true,   // is have  deivce name
        //.include_txpower =  // TX Power 发射功率
        .min_interval = 0x0006, // slave connection min interval, Time = min_interval * 1.25 msec
        .max_interval = 0x0010, // slave connection max interval, Time = max_interval * 1.25 msec
        .appearance = 0x00,     // device 外观 type
        .manufacturer_len =0,   // user data length
        .p_manufacturer_data=NULL, //user data
        .service_data_len  =0, // service data length
        .p_service_data = NULL, // service data
        //TODO
    }
}

void gatts::_init_gatts()
{
    esp_err_t ret = esp_ble_gatts_register_callback(gatts_event_handler);
    if (ret)
    {
        ESP_LOGE(GATTS_TABLE_TAG, "gatts register callback error error_code = %x", ret);
    }

    ret = esp_ble_gatts_app_register(APP_ID);

    if (ret)
    {
        ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error,error_code = %x", ret);
    }
}

void gatts::gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    if (event == ESP_GATTS_REG_EVT)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            heart_rate_profile_tab[0].gatts_if = gatts_if;
        }
        else
        {
            ESP_LOGE(GATTS_TABLE_TAG, "reg app failed, app_id %04x, status %d",
                     param->reg.app_id,
                     param->reg.status);
            return;
        }
    }
    do
    {
        int idx = 0;
        for (idx = 0; idx < 1; idx++)
        {
            if (gatts_if == ESP_GATT_IF_NONE || gatts_if == heart_rate_profile_tab[idx].gatts_if)
            {
                if (heart_rate_profile_tab[idx].gatts_cb)
                {
                    heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

void gatts::gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        // 注册应用程序事件
        // 当应用程序通过esp_ble_gatts_register_callback()注册gatt服务时,会收到此事件
        ESP_LOGI(GATTS_LOGI, "ESP_GATTS_REG_EVT : ---- change blue name");
        char device_name[20] = {0};
        memccpy(device_name, "TFY_", 4);
        ArrayToHexString(esp_bt_dev_get_address(), 5, &device_name[4]);
        ESP_LOGI(GATTS_LOGI, "MAC  ---  %s", device_name);
        esp_err_t ret = esp_ble_gap_set_device_name(device_name);

        if(ret)
        {
            ESP_LOGI(GATTS_TABLE_TAG, "set device name failed, error code = %x", ret);
        }

        ret = esp_ble_gap_config_adv_data();
        break;
    case ESP_GATTS_READ_EVT:
        // GATT客户端读取请求时间
        // 当GATT客户端请求读取一个特性值时,触发
        break;
    case ESP_GATTS_WRITE_EVT:
        // GATT客户端写入请求事件
        // 当GATT客户端请求向服务器写入一个特性值触发
        break;
    case ESP_GATTS_EXEC_WRITE_EVT:
        // GATT客户端执行写入请求事件
        // 此事件在客户端发送执行写入请求时触发,通常用于处理队列中的多个写操作
        break;
    case ESP_GATTS_MTU_EVT:
        // MTU设置完成事件
        // 当客户端和服务器之间的MTU(最大传输单元)协商完成时触发,MTU的增大会提升数据传输效率
        break;
    case ESP_GATTS_CONF_EVT:
        // 接收到确认事件
        // 当客户端发送确认时触发,通常用于确认已经写入的数据
        break;
    case ESP_GATTS_UNREG_EVT:
        // 注销应用程序ID事件
        // 当应用程序注销其GATT服务时触发此事件
        break;
    case ESP_GATTS_CREATE_EVT:
        // 创建服务完成事件
        // 当GATT服务创建成功时触发此事件,通常在注册服务后调用
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        // 添加包含服务完成事件
        // 当向GATT服务中添加包含服务(包括其它服务作为特性的一部分)时触发
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        // 添加特性完成事件
        // 当GATT服务中的特性添加成功时触发
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        // 添加特性描述符
        // 当特性描述符成功添加到GATT服务时触发
        break;
    case ESP_GATTS_DELETE_EVT:
        // 删除服务完成事件
        // 当服务中GATT数据库中删除时触发
        break;
    case ESP_GATTS_START_EVT:
        // 启动服务完成事件
        // 当GATTS服务成功启动时触发
        break;
    case ESP_GATTS_STOP_EVT:
        // 停止服务完成事件
        // 当GATT服务停止时触发
        break;
    case ESP_GATTS_CONNECT_EVT:
        // GATT客户端连接事件
        // 当GATT客户端成功连接到服务器时触发
        break;
    case ESP_GATTS_DISCONNECT_EVT:
        // GATT客户端断开连接事件
        // 当GATT客户端断开与服务器的连接时触发
        break;
    case ESP_GATTS_OPEN_EVT:
        // 连接到对等设备事件
        // 当GATT服务器成功连接到一个设备时会触发此事件
        break;
    case ESP_GATTS_CANCEL_OPEN_EVT:
        // 断开连接时间
        // 当GATT服务器断开与对等设备的连接时,会触发此事件
        break;
    case ESP_GATTS_CLOSE_EVT:
        // GATT服务器关闭事件
        // 当GATT服务器关闭时触发
        break;
    case ESP_GATTS_LISTEN_EVT:
        // GATT监听连接事件
        // 当GATT服务器开始监听客户端连接时触发
        break;
    case ESP_GATTS_CONGEST_EVT:
        // GATT流量拥塞事件
        // 当GATT服务器检测到通信通道发生拥塞时触发
        break;
    case ESP_GATTS_RESPONSE_EVT:
        // GATT 响应完成事件
        // 当GATT服务器发送响应时触发,通常是对客户端的读写请求做出响应
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
        // 创建属性表完成事件
        // 当GATT服务器创建属性表时触发
        break;
    case ESP_GATTS_SET_ATTR_VAL_EVT:
        // 设置属性值完成事件
        // 当设置成功特性值与描述符值时触发此事件
        break;
    case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
        // 发送服务器变更通知事件
        // 当GATT服务器向客户端发送服务变更通知时,触发
        break;
    default:
        break;
    }
}
