
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

#define DESC_VAL_MAX 500

static const uint16_t GATTS_SERVICE_MAIN_UUID = 0xFFE0;                    // 服务声明 ,逻辑容器,通过UUID表示此服务
static const uint16_t GATTS_MAIN_UUID_ARRAY[3] = { 0xFFE1, 0xFFE2, 0xFFE3 }; // GATTS_SERVICE_MAIN_UUID 服务的特征数组
enum
{
    MAIN_SERV = 0,

    MAIN_CHAR_A,
    MAIN_CHAR_VAL_A,

    MAIN_CHAR_B,
    MAIN_CHAR_VAL_B,
    MAIN_CHAR_CFG_B,

    MAIN_CHAR_C,
    MAIN_CHAR_VALUE_C,

    MAIN_AHR_SIZE,
};

static const uint16_t main_service_uuid = ESP_GATT_UUID_PRI_SERVICE; // 主要服务
static const uint16_t character_declation_uuid = ESP_GATT_UUID_CHAR_DECLARE;// 字符特征定义
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;// 客户端字符特征配置

static const uint8_t only_write_no_response = ESP_GATT_CHAR_PROP_BIT_WRITE_NR; // 只写但是服务器不响应
static const uint8_t notify_prop = ESP_GATT_CHAR_PROP_BIT_NOTIFY; // 服务器可以主动向客户端发送特性值的更新，无需客户端确认。


static const uint8_t char_value[4] = { 0x11,0x22,0x33,0x44 };


static esp_gatts_attr_db_t gatt_bd[MAIN_AHR_SIZE] =
{
    [MAIN_SERV] = // 服务声明
    {
        {   ESP_GATT_AUTO_RSP   },
        {
            ESP_UUID_LEN_16,
            (uint8_t*)&main_service_uuid,
            ESP_GATT_PERM_READ,
            sizeof(uint16_t),
            sizeof(GATTS_SERVICE_MAIN_UUID),
            (uint8_t*)&GATTS_SERVICE_MAIN_UUID
        }
    },
    [MAIN_CHAR_A] =  // 特征声明  - 读
    {
        {   ESP_GATT_AUTO_RSP   },
        {
            ESP_UUID_LEN_16,
            (uint8_t*)&character_declation_uuid,
            ESP_GATT_PERM_READ,
            sizeof(uint8_t),
            sizeof(uint8_t),
            (uint8_t*)&only_write_no_response
        }
    },
    [MAIN_CHAR_VAL_A] =  // 特征值
    {
        {   ESP_GATT_AUTO_RSP   },
        {
            ESP_UUID_LEN_16,
            (uint8_t*)&GATTS_MAIN_UUID_ARRAY[0],
            ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
            DESC_VAL_MAX, // esp 最大值 512字节
            sizeof(char_value),
            (uint8_t*)char_value
        }
    },

    [MAIN_CHAR_B] = // 特征声明  - 读
    {
        {   ESP_GATT_AUTO_RSP   },
        {
            ESP_UUID_LEN_16,
            (uint8_t*)&character_declation_uuid,
            ESP_GATT_PERM_READ,
            sizeof(uint8_t),
            sizeof(uint8_t),
            (uint8_t*)notify_prop
        }

    },

    [MAIN_CHAR_VAL_B] = // 特征值
    {
        {   ESP_GATT_AUTO_RSP   },
        {
            ESP_UUID_LEN_16,
            (uint8_t*)&GATTS_MAIN_UUID_ARRAY[1],
            ESP_GATT_PERM_READ,
            DESC_VAL_MAX,
            sizeof(char_value),
            (uint8_t*)char_value
        }
    },

    [MAIN_CHAR_CFG_B]=
    {
        {   ESP_GATT_AUTO_RSP   },
    },



};

gatts::gatts()
{
    service_uuid = { 0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0xE0, 0x00, 0x00 };
    this->heart_rate_profile_tab[0] = {
        .gatts_cb = this->gatts_profile_event_handler,
        .gatts_if = ESP_GATT_IF_NONE,
    };
    this->adv_data =
    {
        .set_scan_rsp = false, // false 普通广播包  true 扫描响应包
        .include_name = true,  // is have  deivce name
        //.include_txpower =  // TX Power 发射功率
        .min_interval = 0x0006,                   // slave connection min interval, Time = min_interval * 1.25 msec
        .max_interval = 0x0010,                   // slave connection max interval, Time = max_interval * 1.25 msec
        .appearance = 0x00,                       // device 外观 type
        .manufacturer_len = 0,                    // user data length
        .p_manufacturer_data = NULL,              // user data
        .service_data_len = 0,                    // service data length
        .p_service_data = NULL,                   // service data
        .service_uuid_len = sizeof(service_uuid), // uuid length
        .p_service_uuid = service_uuid,           // uuid data
        // ESP_BLE_ADV_FLAG_GEN_DISC 设备处于可被通用发现模式
        // 不支持经典蓝牙
        .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT), // 不支持经典蓝牙
    };
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

void gatts::gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param)
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

void gatts::gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t* param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    // 注册应用程序事件
    // 当应用程序通过esp_ble_gatts_register_callback()注册gatt服务时,会收到此事件
    ESP_LOGI(GATTS_LOGI, "ESP_GATTS_REG_EVT : ---- change blue name");
    char device_name[20] = { 0 };
    memccpy(device_name, "TFY_", 4);
    ArrayToHexString(esp_bt_dev_get_address(), 5, &device_name[4]);
    ESP_LOGI(GATTS_LOGI, "MAC  ---  %s", device_name);
    esp_err_t ret = esp_ble_gap_set_device_name(device_name);

    if (ret)
    {
        ESP_LOGI(GATTS_TABLE_TAG, "set device name failed, error code = %x", ret);
    }

    ret = esp_ble_gap_config_adv_data(&this->adv_data);

    if (ret)
    {
        ESP_LOGE(GATTS_LOGI, "config adv data failed ,error_code %x", ret);
    }
    ret = esp_ble_gatts_create_attr_tab();
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
