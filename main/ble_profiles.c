
#include "ble_profiles.h"
#include "ble_profile_defs.h"
#include "ble_callbacks.h"
#include "esp_err.h"
#include "esp_log.h"
#include "string.h"

static int ble_led_profile_callback(e_ble_profile_actions_t action, void *profile_arg, void *param)
{
    int ret = 0;
    s_ble_profile_info_t *profile_info = (s_ble_profile_info_t *)profile_arg;
    if (profile_info)
    {
        switch (action)
        {
        case BLE_PROFILE_ACTION_INITIALIZE:
        {
            ESP_LOGI(__FILE__, "%d: BLE_PROFILE_ACTION_INITIALIZE. (app_id: %d)", __LINE__, profile_info->app_id);
            profile_info->service_id.is_primary = true;
            profile_info->service_id.id.inst_id = 0x00;
            profile_info->service_id.id.uuid.len = ESP_UUID_LEN_16;
            profile_info->service_id.id.uuid.uuid.uuid16 = BLE_LED_PROFILE_UUID;
            break;
        }
        case BLE_PROFILE_ACTION_MAX:
        default:
        {
            ESP_LOGI(__FILE__, "%d: Unknown action provided. (action: %d)", __LINE__, action);
            break;
        }
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

int ble_profiles_prepare_profiles()
{
    int ret = 0;

    ble_profile_prepare_profile_info(ble_led_profile_callback, BLE_LED_PROFILE_ID, 1, 1);

    return ret;
}
