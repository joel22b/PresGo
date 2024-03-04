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
#include "fare_fob/fare_fob.h"

static uint8_t advertising_set_handle = 0xff;

void
btc_adv_init ()
{
  // Create an advertising set.
  sl_status_t ret_val = sl_bt_advertiser_create_set (&advertising_set_handle);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to create advertising set: 0x%04X\n\r", ret_val);
  }

  // Generate data for advertising
  ret_val = sl_bt_legacy_advertiser_generate_data (
      advertising_set_handle, sl_bt_advertiser_general_discoverable);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to generate advertising data: 0x%04X\n\r", ret_val);
  }

  // Set advertising interval to 100ms.
  ret_val = sl_bt_advertiser_set_timing (advertising_set_handle, 160, // min. adv. interval (milliseconds * 1.6)
                                         160, // max. adv. interval (milliseconds * 1.6)
                                         0,   // adv. duration
                                         0);  // max. num. adv. events
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to set advertising timing: 0x%04X\n\r", ret_val);
  }
}

void
btc_adv_start (btc_adv_services_t adv_services)
{
  //app_log_info("Started adv\n\r");
  sl_status_t ret_val;

  if (adv_services & btc_adv_services_cte)
  {
      //app_log_info("Started adv CTE\n\r");
      ret_val = adv_cte_start ();
      if (ret_val != SL_STATUS_OK)
      {
          app_log_info("Started legacy adv: ret=%04X\n\r", ret_val);
      }
  }

  if (adv_services & btc_adv_services_legacy)
  {
      //app_log_info("Started adv Legacy\n\r");
      ret_val = sl_bt_legacy_advertiser_start (
          advertising_set_handle, sl_bt_advertiser_connectable_scannable);
      if (ret_val != SL_STATUS_OK)
      {
          app_log_info("Started legacy adv: ret=%04X\n\r", ret_val);
      }
  }
}

void
btc_adv_stop (btc_adv_services_t adv_services)
{
  //app_log_info("Stopped adv\n\r");
  sl_status_t ret_val;

  if (adv_services & btc_adv_services_cte) {
      //app_log_info("Stopped adv CTE\n\r");
    ret_val = sl_bt_advertiser_stop (advertising_set_handle);
    if (ret_val != SL_STATUS_OK) {
      app_log_info("Stopped legacy adv: ret=%04X\n\r", ret_val);
    }
  }

  if (adv_services & btc_adv_services_legacy) {
      //app_log_info("Stopped adv Legacy\n\r");
      ret_val = adv_cte_stop ();
      if (ret_val != SL_STATUS_OK) {
          app_log_info("Stopped cte adv: ret=%04X\n\r", ret_val);
      }
  }
}

void btc_adv_scanner_start() {
  sl_status_t ret_val = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m, 0x2);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start bluetooth scanner: return value = %lu\n\r",
                   ret_val);
  }
}

void btc_adv_scanner_stop() {
  sl_status_t ret_val = sl_bt_scanner_stop();
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to stop bluetooth scanner: return value = %lu\n\r",
                   ret_val);
  }
}

void
btc_adv_extended (
    sl_bt_evt_scanner_extended_advertisement_report_t *evt_adv_extended)
{
  if (ff_is_bus_adv(evt_adv_extended)) {
      sm_update_state(SM_STATE_RAPID_ADV, true);
  }
}
