
#ifndef __BLE_PROFILE_DEFS_H__
#define __BLE_PROFILE_DEFS_H__

#include "esp_gatt_defs.h"

#define BLE_DEVICE_NAME "BLE Device"

#define BLE_PROFILE_COUNT 1

#define BLE_LED_PROFILE_ID 0

#define BLE_LED_PROFILE_UUID 0x0001

#define BLE_LED_PROFILE_CHAR_UUID 0x0010

typedef enum e_ble_profile_actions
{
    BLE_PROFILE_ACTION_INITIALIZE = 0,
    BLE_PROFILE_ACTION_WRITE_VALUE,
    BLE_PROFILE_ACTION_READ_VALUE,
    BLE_PROFILE_ACTION_MAX,
} e_ble_profile_actions_t;

typedef int (*ble_profile_callback_func)(e_ble_profile_actions_t profile_action, void *profile_arg, void *params);

typedef struct s_ble_profile_characteristics
{
    uint16_t service_handle;
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t property;
    esp_attr_value_t attribute_value;
    esp_attr_control_t control;
}s_ble_profile_characteristics_t;

typedef struct s_ble_profile_info
{
    uint16_t app_id;
    uint16_t connection_id;
    uint16_t gatts_interface;
    uint16_t number_of_handle;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    ble_profile_callback_func ble_profile_cb;
    s_ble_profile_characteristics_t *characteristics;
} s_ble_profile_info_t;

#endif // __BLE_PROFILE_DEFS_H__

