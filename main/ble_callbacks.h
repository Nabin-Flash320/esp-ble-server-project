
#ifndef __BLE_CALLBACKS_H__
#define __BLE_CALLBACKS_H__

#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"
#include "ble_profile_defs.h"

void ble_gap_callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void ble_gatt_if_callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_interface, esp_ble_gatts_cb_param_t *param);
int ble_profile_prepare_profile_info(ble_profile_callback_func callback_func, uint16_t app_id, uint16_t number_of_handle, uint16_t service_handle);

#endif // __BLE_CALLBACKS_H__
