/*
 * bt_controller.c
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#include "bt_controller.h"

#include "app_log.h"

void btc_handle_event(sl_bt_msg_t *evt) {
    switch (SL_BT_MSG_ID(evt->header)) {
      // -------------------------------
      // This event indicates the device has started and the radio is ready.
      // Do not call any stack command before receiving this boot event!
      case sl_bt_evt_system_boot_id:
        btc_init_stack(&evt->data.evt_system_boot);
        break;

      case sl_bt_evt_connection_phy_status_id:
        // This event means that the PHYs have updated status
        // Typically happens after connection occurs
        // Nothing needs to be done
        break;

      case sl_bt_evt_connection_tx_power_id:
        // This event means that the TX power has changed
        // Typically happens after connection occurs
        // Nothing needs to be done
        break;

      case sl_bt_evt_gatt_mtu_exchanged_id:
        // This event means that the MTUs (Maximum Transfer Units) have been
        // exchanged between both devices
        // Happens after connection occurs
        // Nothing needs to be done
        break;

      // -------------------------------
      // This event indicates that a new connection was opened.
      case sl_bt_evt_connection_opened_id:
        btc_connect_openned(&evt->data.evt_connection_opened);
        break;

      case sl_bt_evt_connection_parameters_id:
        btc_connect_parameters(&evt->data.evt_connection_parameters);
        break;

      case sl_bt_evt_connection_remote_used_features_id:
        // This event gives the remote features used by remote
        // Happens after connection occurs
        // Nothing needs to be done
        break;

      case sl_bt_evt_gatt_service_id:
        btc_connect_gatt_service(&evt->data.evt_gatt_service);
        break;

      case sl_bt_evt_gatt_procedure_completed_id:
        if (evt->data.evt_gatt_procedure_completed.result == SL_STATUS_OK) {
            app_log_info("GATT Procedure completed successfully: Connection handle %02X\n\r",
                         evt->data.evt_gatt_procedure_completed.connection);
        }
        else {
            app_log_info("GATT Procedure completed with error %02X: Connection handle %02X\n\r",
                         evt->data.evt_gatt_procedure_completed.result,
                         evt->data.evt_gatt_procedure_completed.connection);
        }
        break;

      case sl_bt_evt_gatt_characteristic_id:
        app_log_info("GATT Characteristics:\n\r Connection handle %02X Characteristic handle %04X Properties %02X\n\r",
                     evt->data.evt_gatt_characteristic.connection,
                     evt->data.evt_gatt_characteristic.characteristic,
                     evt->data.evt_gatt_characteristic.properties);
        break;

      case sl_bt_evt_gatt_server_characteristic_status_id:
        btc_connect_gatt_characteristic(&evt->data.evt_gatt_server_characteristic_status);
        break;

      case sl_bt_evt_gatt_server_user_write_request_id:
        btc_connect_rx_data(&evt->data.evt_gatt_server_user_write_request);
        break;

      // -------------------------------
      // This event indicates that a connection was closed.
      case sl_bt_evt_connection_closed_id:
        btc_connect_closed(&evt->data.evt_connection_closed);
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
        btc_adv_extended(&evt->data.evt_scanner_extended_advertisement_report);
        break;

      // This event is called when an advertisement is found
      case sl_bt_evt_scanner_scan_report_id:
        // Depreciated, replaced with legacy and extended advertising
        break;

      // -------------------------------
      // Default event handler.
      default:
        app_log_info("Unhandled event code: %lu\n\r", SL_BT_MSG_ID(evt->header));
        break;
    }
}
