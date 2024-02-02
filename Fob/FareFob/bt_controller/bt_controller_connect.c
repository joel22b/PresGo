/*
 * bt_controller_connection.c
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#include "bt_controller.h"

#include "app_log.h"
#include "app_assert.h"

const uint8_t service_uuid[16] = {0xba, 0x5e, 0xd6, 0x4a, 0xf0, 0x8f, 0x16, 0xa9, 0xe3, 0x41, 0xba, 0xca, 0x09, 0x98, 0xe8, 0xc3};

static uint8_t connection_count;
static btc_connect_handle_t connections[SL_BT_CONFIG_MAX_CONNECTIONS];

btc_connect_handle_t* btc_get_connect_handle(uint8_t connection) {
  if (connection > SL_BT_CONFIG_MAX_CONNECTIONS || connection <= 0) {
      return NULL;
  }
  return &connections[connection-1];
}

void btc_connect_init() {
  connection_count = 0;

  // Initialize connections array
  for (uint8_t i = 0; i < SL_BT_CONFIG_MAX_CONNECTIONS; i++) {
      connections[i].service = 0x00000000;
      connections[i].tx = 0x0000;
      connections[i].rx = 0x0000;
  }
}

void
btc_connect_openned (sl_bt_evt_connection_opened_t *evt_connect_openned)
{
  app_log_info("Connection opened: %d\n\r", evt_connect_openned->connection);
  app_log_info("Client address: %02X:%02X:%02X:%02X:%02X:%02X\n\r",
               evt_connect_openned->address.addr[5],
               evt_connect_openned->address.addr[4],
               evt_connect_openned->address.addr[3],
               evt_connect_openned->address.addr[2],
               evt_connect_openned->address.addr[1],
               evt_connect_openned->address.addr[0]);
  connection_count++;
  // Continue advertising if the stack allows further connections.
  if (connection_count < SL_BT_CONFIG_MAX_CONNECTIONS)
    {
      // Generate data for advertising
      /*sl_status_t ret_val = sl_bt_legacy_advertiser_generate_data (
          advertising_set_handle, sl_bt_advertiser_general_discoverable);
      app_assert_status (ret_val);*/

      btc_adv_start(btc_adv_services_legacy);
      app_log_info("Continue advertising\n\r");
    }
}

void
btc_connect_closed (sl_bt_evt_connection_closed_t *evt_connect_closed)
{
  app_log_info("Connection closed: %d\n\r",
               evt_connect_closed->connection);

  // Clear connection handle
  btc_connect_handle_t* handle = btc_get_connect_handle(evt_connect_closed->connection);
  handle->service = 0x00000000;
  handle->tx = 0x0000;
  handle->rx = 0x0000;

  // Generate data for advertising
  /*sc = sl_bt_legacy_advertiser_generate_data (
      advertising_set_handle, sl_bt_advertiser_general_discoverable);
  app_assert_status (sc);*/

  if (connection_count >= SL_BT_CONFIG_MAX_CONNECTIONS)
    {
      // Restart advertising after client has disconnected.
      btc_adv_start(btc_adv_services_legacy);
      app_log_info("Restart advertising\n");
    }
  connection_count--;
}

void btc_connect_parameters(sl_bt_evt_connection_parameters_t* evt_connect_parameters) {
  app_log_info("Connection Parameters: Connection handle %02X\n\r", evt_connect_parameters->connection);
  if (btc_get_connect_handle(evt_connect_parameters->connection)->service == 0x00000000) {
      // No service connected, discover some
      app_log_info("Trying to discover services\n\r");
      sl_status_t ret_val = sl_bt_gatt_discover_primary_services(evt_connect_parameters->connection);
      if (ret_val != SL_STATUS_OK) {
          app_log_info("Failed to discover remote gatt services: %02X\n\r", ret_val);
      }
  }
}

void btc_connect_gatt_service(sl_bt_evt_gatt_service_t* evt_gatt_service) {
  // Check that this is the correct service
  bool match = true;
  for (uint8_t i = 0; i < evt_gatt_service->uuid.len; i++) {
      if (service_uuid[i] != evt_gatt_service->uuid.data[i]) {
          match = false;
          break;
      }
  }

  if (match) {
      // Discover the characteristics of the service
      sl_status_t ret_val = sl_bt_gatt_discover_characteristics(
          evt_gatt_service->connection,
          evt_gatt_service->service);
      if (ret_val != SL_STATUS_OK && ret_val != SL_STATUS_IN_PROGRESS) {
          app_log_info("Failed to discover remote gatt services: %02X\n\r", ret_val);
      }

      // Update connection handle with service id
      btc_connect_handle_t* handle = btc_get_connect_handle(
          evt_gatt_service->connection);
      handle->service = evt_gatt_service->service;
  }
}

void btc_connect_gatt_characteristic(sl_bt_evt_gatt_server_characteristic_status_t* evt_gatt_characteristic) {
  // This is the characteristic used to transmit data
  btc_connect_handle_t* handle = btc_get_connect_handle(evt_gatt_characteristic->connection);
  handle->tx = evt_gatt_characteristic->characteristic;
}

void btc_connect_rx_data(sl_bt_evt_gatt_server_user_write_request_t* evt_write_request) {
  btc_connect_handle_t* handle = btc_get_connect_handle(evt_write_request->connection);
  if (handle->rx == 0x0000) {
      handle->rx = evt_write_request->characteristic;
  }

  uint16_t val = 0xBEEF;
  sl_status_t ret_val = sl_bt_gatt_server_send_notification(
      evt_write_request->connection, handle->tx, 2, (uint8_t*)&val);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to send GATT notification: %02X\n\r", ret_val);
  }
}
