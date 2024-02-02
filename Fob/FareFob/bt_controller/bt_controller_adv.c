/*
 * bt_controller_adv.c
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#include "bt_controller.h"

#include "app_log.h"
#include "app_assert.h"
#include "sli_gatt_service_cte_adv.h"

#include "state_machine/state_machine.h"

static uint8_t advertising_set_handle = 0xff;

#define IDENTIFER_BT_DATA_START 0x09
#define IDENTIFER_BT_DATA_BUS "BUSCONNECTION1"
#define IDENTIFER_BT_DATA_BUS_LEN 14

bool isBusAdv(sl_bt_evt_scanner_extended_advertisement_report_t *report)
{
  // Ensure complete advertisement
  if (!report->data_completeness == sl_bt_scanner_data_status_complete) {
      return false;
  }

  // TODO: Add Adv SID check

  // Check data for Bus identifier
  bool dataStarted = false;
  uint8_t dataIndex = 0;
  for (uint8_t i = 0; i < report->data.len; i++) {

      if (dataStarted) {
          if (report->data.data[i] == IDENTIFER_BT_DATA_BUS[dataIndex++]) {
              if (dataIndex >= IDENTIFER_BT_DATA_BUS_LEN) {
                  return true;
              }
          }
          else {
              return false;
          }
      }
      else if (report->data.data[i] == IDENTIFER_BT_DATA_START) {
          dataStarted = true;
      }
  }

  return false;
}

void
btc_adv_init ()
{
  // Create an advertising set.
  sl_status_t ret_val = sl_bt_advertiser_create_set (&advertising_set_handle);
  app_assert_status(ret_val);

  // Generate data for advertising
  ret_val = sl_bt_legacy_advertiser_generate_data (
      advertising_set_handle, sl_bt_advertiser_general_discoverable);
  app_assert_status(ret_val);

  // Set advertising interval to 100ms.
  ret_val = sl_bt_advertiser_set_timing (advertising_set_handle, 160, // min. adv. interval (milliseconds * 1.6)
                                         160, // max. adv. interval (milliseconds * 1.6)
                                         0,   // adv. duration
                                         0);  // max. num. adv. events
  app_assert_status(ret_val);
}

void
btc_adv_start (btc_adv_services_t adv_services)
{
  sl_status_t ret_val;

  if (adv_services & btc_adv_services_cte)
  {
      ret_val = adv_cte_start ();
      if (ret_val != SL_STATUS_OK)
      {
          app_log_info("Started legacy adv: ret=%02X\n\r", ret_val);
      }
  }

  if (adv_services & btc_adv_services_legacy)
  {
      ret_val = sl_bt_legacy_advertiser_start (
          advertising_set_handle, sl_bt_advertiser_connectable_scannable);
      if (ret_val != SL_STATUS_OK)
      {
          app_log_info("Started legacy adv: ret=%02X\n\r", ret_val);
      }
  }
}

void
btc_adv_stop (btc_adv_services_t adv_services)
{
  sl_status_t ret_val;

  if (adv_services & btc_adv_services_cte) {
    ret_val = sl_bt_advertiser_stop (advertising_set_handle);
    if (ret_val != SL_STATUS_OK) {
      app_log_info("Stopped legacy adv: ret=%02X\n\r", ret_val);
    }
  }

  if (adv_services & btc_adv_services_legacy) {
      ret_val = adv_cte_stop ();
      if (ret_val != SL_STATUS_OK) {
          app_log_info("Stopped cte adv: ret=%02X\n\r", ret_val);
      }
  }
}

void btc_adv_scanner_start() {
  sl_status_t ret_val = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m, 0x2);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start bluetooth scanner: return value = %d\n\r",
                   ret_val);
  }
}

void btc_adv_scanner_stop() {
  sl_status_t ret_val = sl_bt_scanner_stop();
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to stop bluetooth scanner: return value = %d\n\r",
                   ret_val);
  }
}

void
btc_adv_extended (
    sl_bt_evt_scanner_extended_advertisement_report_t *evt_adv_extended)
{
  if (isBusAdv(evt_adv_extended)) {
      sm_update_state(SM_STATE_RAPID_ADV, true);
  }
}
