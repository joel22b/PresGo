/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"
#include "sl_simple_led_instances.h"
#include "state_machine/state_machine.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

// Number of active connections.
static uint8_t connection_count = 0;

#define EM_EVENT_MASK_ALL      (  SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3)

void power_transition_log_callback(sl_power_manager_em_t from,
                 sl_power_manager_em_t to)
{
  app_log_info("Power transition occurred: from EM%d to EM%d\n\r", from, to);
}

sl_power_manager_em_transition_event_handle_t power_transition_log_event_handle;
sl_power_manager_em_transition_event_info_t power_transition_log_event_info = {
  .event_mask = EM_EVENT_MASK_ALL,
  .on_event = power_transition_log_callback,
};

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  sl_led_init(&sl_led_led0);
  sl_led_turn_off(&sl_led_led0);
  //sl_led_turn_on(&sl_led_led0);

  //sl_power_manager_subscribe_em_transition_event(&power_transition_log_event_handle, &power_transition_log_event_info);

  sm_init();
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////

  sm_process();
}

bool correct_address(bd_addr address)
{
  // Name: ExtendedBeacon
  return address.addr[5] == 0x02
      && address.addr[4] == 0x80
      && address.addr[3] == 0xE1
      && address.addr[2] == 0x88
      && address.addr[1] == 0x77
      && address.addr[0] == 0x66;
}

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

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n\r",
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n\r",
                   address_type ? "static random" : "public device",
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Set advertising interval to 100ms.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        160, // min. adv. interval (milliseconds * 1.6)
        160, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);
      // Start general advertising and enable connections.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      app_log_info("Started advertising\n\r");

      sm_set_adv_handle(&advertising_set_handle);
      sm_update_state(SM_STATE_CHECK_FOR_BUS, true);
      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      //sl_led_toggle(&sl_led_led0);
      app_log_info("Connection opened: %d\n\r", evt->data.evt_connection_opened.connection);
      app_log_info("Client address: %02X:%02X:%02X:%02X:%02X:%02X\n\r",
                   evt->data.evt_connection_opened.address.addr[5],
                   evt->data.evt_connection_opened.address.addr[4],
                   evt->data.evt_connection_opened.address.addr[3],
                   evt->data.evt_connection_opened.address.addr[2],
                   evt->data.evt_connection_opened.address.addr[1],
                   evt->data.evt_connection_opened.address.addr[0]);
      connection_count++;
      // Continue advertising if the stack allows further connections.
      if (connection_count < SL_BT_CONFIG_MAX_CONNECTIONS) {
        // Generate data for advertising
        sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                   sl_bt_advertiser_general_discoverable);
        app_assert_status(sc);

        sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                           sl_bt_advertiser_connectable_scannable);
        app_assert_status(sc);
        app_log_info("Continue advertising\n\r");
      }
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      app_log_info("Connection closed: %d\n\r",
                   evt->data.evt_connection_closed.connection);

      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      if (connection_count >= SL_BT_CONFIG_MAX_CONNECTIONS) {
        // Restart advertising after client has disconnected.
        sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                           sl_bt_advertiser_connectable_scannable);
        app_assert_status(sc);
        app_log_info("Restart advertising\n");
      }
      connection_count--;
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // This event is called when a legacy advertisement is found
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      // Only looking for extended advertisements, so ignored
      break;

    // This event is called when an extended advertisement is found
    case sl_bt_evt_scanner_extended_advertisement_report_id:
      //sl_led_toggle(&sl_led_led0);
      //app_log_info("Extended Adv found: \n\r");
      //if (correct_address(evt->data.evt_scanner_extended_advertisement_report.address) &&
      if (isBusAdv(&evt->data.evt_scanner_extended_advertisement_report))
      {
        //sl_led_toggle(&sl_led_led0);
        app_log_info("Correct extended found: \n\r");
        /*app_log_info("Event flags: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.event_flags);
        app_log_info("Address type: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.address_type);
        app_log_info("Bonding: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.bonding);
        app_log_info("Channel: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.channel);
        app_log_info("Adv SID: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.adv_sid);
        app_log_info("Primary PHYs: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.primary_phy);
        app_log_info("Secondary PHYs: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.secondary_phy);
        app_log_info("Data completeness: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.data_completeness);
        app_log_info("Counter: %02X\n\r", evt->data.evt_scanner_extended_advertisement_report.counter);
        app_log_info("Data length: %d\n\r", evt->data.evt_scanner_extended_advertisement_report.data.len);
        for (uint8_t i = 0; i < evt->data.evt_scanner_extended_advertisement_report.data.len; i++)
        {
          app_log_info("%02X", evt->data.evt_scanner_extended_advertisement_report.data.data[i]);
        }
        app_log_info("\n\r");*/
      }
      break;

    // This event is called when an advertisement is found
    case sl_bt_evt_scanner_scan_report_id:
      // Depreciated, replaced with legacy and extended advertising
      break;

    // -------------------------------
    // Default event handler.
    default:
      app_log_info("Unhandled event code: %d\n\r", SL_BT_MSG_ID(evt->header));
      break;
  }
}
