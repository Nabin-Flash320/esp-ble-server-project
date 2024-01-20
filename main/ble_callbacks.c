#include "esp_log.h"
#include "esp_err.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "ble_callbacks.h"
#include "ble_profile_defs.h"
#include "string.h"
#include "stdlib.h"

static s_ble_profile_info_t *ble_profile_array[BLE_PROFILE_COUNT];
static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static s_ble_profile_info_t *get_profile_using_uuid(const uint16_t uuid)
{
    s_ble_profile_info_t *profile_info = NULL;
    for(int i = 0; i < BLE_PROFILE_COUNT; i++)
    {
        if(uuid == ble_profile_array[i]->service_id.id.uuid.uuid.uuid16)
        {
            profile_info = ble_profile_array[i];
            break;
        }
    }
    return profile_info;
}

void ble_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT. (event: %d) (status: %d)", __LINE__, event, param->adv_data_cmpl.status);
        ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
        break;
    }
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT. (event: %d) (status: %d)", __LINE__, event, param->scan_rsp_data_cmpl.status);
        break;
    }
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GAP_BLE_ADV_START_COMPLETE_EVT. (event: %d) (status: %d)", __LINE__, event, param->adv_start_cmpl.status);
        ESP_ERROR_CHECK(esp_ble_gap_set_device_name(BLE_DEVICE_NAME));
        break;
    }
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT. (event: %d) (status: %d)", __LINE__, event, param->scan_stop_cmpl.status);
        break;
    }
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT. (event: %d)", __LINE__, event);
        printf("{\n\tDevice Address: %02X:%02X:%02X:%02X:%02X:%02X, \n", param->update_conn_params.bda[0], param->update_conn_params.bda[1], param->update_conn_params.bda[2],
               param->update_conn_params.bda[3], param->update_conn_params.bda[4], param->update_conn_params.bda[5]);
        printf("\tMin connection interface: %d,\n", param->update_conn_params.min_int);
        printf("\tMax connection interface: %d,\n", param->update_conn_params.max_int);
        printf("\tLatency: %d,\n", param->update_conn_params.latency);
        printf("\tCurrent connection interval: %d,\n", param->update_conn_params.conn_int);
        printf("\tTimeout: %d\n}\n", param->update_conn_params.timeout);
        break;
    }
    default:
    {
        ESP_LOGE(__FILE__, "%d: Default BLE gap event. (event: %d)", __LINE__, event);
        break;
    }
    }
}

void ble_gatt_if_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_interface, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_REG_EVT. (event: %d) (Status: %d)", __LINE__, event, param->reg.status);
        if (ble_profile_array[param->reg.app_id] && ble_profile_array[param->reg.app_id]->ble_profile_cb)
        {
            ble_profile_array[param->reg.app_id]->gatts_interface = gatts_interface;
            int ret = ble_profile_array[param->reg.app_id]->ble_profile_cb(BLE_PROFILE_ACTION_INITIALIZE, ble_profile_array[param->reg.app_id], (void *)&gatts_interface);
            if (0 == ret)
            {
                ESP_ERROR_CHECK(esp_ble_gatts_create_service(gatts_interface, &ble_profile_array[param->reg.app_id]->service_id, ble_profile_array[param->reg.app_id]->number_of_handle));
            }
        }
        break;
    }
    case ESP_GATTS_READ_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_READ_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_WRITE_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_EXEC_WRITE_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_MTU_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_MTU_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_CREATE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_CREATE_EVT. (event: %d) (status: %d)", __LINE__, event, param->create.status);
        ESP_LOGI(__FILE__, "Service detials: ");
        printf("{\n\tIs primary: %s,\n", 1 == param->create.service_id.is_primary ? "true" : "false");
        printf("\tInstance ID: %d,\n", param->create.service_id.id.inst_id);
        printf("\tUUID len: %d,\n", param->create.service_id.id.uuid.len);
        printf("\tUUID: 0x%04X,\n", param->create.service_id.id.uuid.uuid.uuid16);
        printf("\tService handle: %d\n}\n", param->create.service_handle);
        ESP_ERROR_CHECK(esp_ble_gatts_start_service(param->create.service_handle));
        
        s_ble_profile_info_t *profile_info = get_profile_using_uuid(param->create.service_id.id.uuid.uuid.uuid16);
        if(profile_info)
        {
            profile_info->service_handle = param->create.service_handle;
            profile_info->characteristics->service_handle = param->create.service_handle;
            ESP_ERROR_CHECK(esp_ble_gatts_add_char(profile_info->service_handle, &profile_info->characteristics->uuid, profile_info->characteristics->permission,
                                                    profile_info->characteristics->property, &profile_info->characteristics->attribute_value, NULL));
        }
        else 
        {
            ESP_LOGE(__FILE__, "%d: Error occured while getting profile info.", __LINE__);
        }
        break;
    }
    case ESP_GATTS_START_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_START_EVT. (event: %d) (Status: %d)", __LINE__, event, param->start.status);
        printf("Service handle is %d\n", param->start.service_handle);
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_ADD_CHAR_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_STOP_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_STOP_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_CONNECT_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_CONNECT_EVT. (event: %d)", __LINE__, event);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_DISCONNECT_EVT. (event: %d)", __LINE__, event);
        printf("{\n\tDevice Address: %02X:%02X:%02X:%02X:%02X:%02X, \n", param->disconnect.remote_bda[0], param->disconnect.remote_bda[1], param->disconnect.remote_bda[2],
               param->disconnect.remote_bda[3], param->disconnect.remote_bda[4], param->disconnect.remote_bda[5]);
        printf("\tConnection ID: %d,\n", param->disconnect.conn_id);
        printf("\tLink role: %d,\n", param->disconnect.link_role);
        printf("\tReason: 0x%04X\n}\n", param->disconnect.reason);
        ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&adv_params));
        break;
    }
    case ESP_GATTS_CLOSE_EVT:
    {
        ESP_LOGI(__FILE__, "%d: ESP_GATTS_CLOSE_EVT. (event: %d)", __LINE__, event);
        break;
    }
    default:
    {
        ESP_LOGE(__FILE__, "%d: Default BLE gatt event. (event: %d)", __LINE__, event);
        break;
    }
    }
}

int ble_profile_prepare_profile_characteristics(s_ble_profile_info_t *profile_info)
{
    int ret = 0;

    if (profile_info)
    {
        profile_info->characteristics = (s_ble_profile_characteristics_t *)malloc(sizeof(s_ble_profile_characteristics_t));
        if (profile_info->characteristics)
        {
            profile_info->characteristics->permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
            profile_info->characteristics->property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
            profile_info->characteristics->uuid.len = ESP_UUID_LEN_16;
            profile_info->characteristics->uuid.uuid.uuid16 = BLE_LED_PROFILE_CHAR_UUID;
            memset(&profile_info->characteristics->control, 0, sizeof(esp_attr_control_t));
            profile_info->characteristics->attribute_value.attr_len = 2;
            profile_info->characteristics->attribute_value.attr_max_len = 2;
            profile_info->characteristics->attribute_value.attr_value = (uint8_t *)malloc(2 * sizeof(uint8_t));
            *profile_info->characteristics->attribute_value.attr_value = 0x0001;
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

int ble_profile_prepare_profile_info(ble_profile_callback_func ble_profile_callback_func, uint16_t app_id, uint16_t number_of_handle, uint16_t service_handle)
{
    int ret = 0;

    if (ble_profile_callback_func)
    {
        ble_profile_array[app_id] = (s_ble_profile_info_t *)malloc(sizeof(s_ble_profile_info_t));
        if (ble_profile_array[app_id])
        {
            memset(ble_profile_array[app_id], 0, sizeof(s_ble_profile_info_t));
            ble_profile_array[app_id]->app_id = app_id;
            ble_profile_array[app_id]->number_of_handle = number_of_handle;
            ble_profile_array[app_id]->service_handle = service_handle;
            ble_profile_array[app_id]->ble_profile_cb = ble_profile_callback_func;
            ESP_ERROR_CHECK(ble_profile_prepare_profile_characteristics(ble_profile_array[app_id]));
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
