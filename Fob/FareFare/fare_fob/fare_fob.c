/*
 * fare_fob.c
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#include "fare_fob.h"

#include "app_log.h"

#include "config.h"
#ifndef DEBUG_NO_RECENT_BUS
#include "recent_bus.h"
#endif
#include "bt_controller/bt_controller.h"

/* =============================
 * Global Variables
 *
 *
============================= */
uint8_t fare_uuid[UUID_LEN];


void ff_init() {
  for (uint8_t i = 0; i < UUID_LEN; i++) {
      fare_uuid[i] = FARE_DEFAULT_UUID[i];
  }

#ifndef DEBUG_NO_RECENT_BUS
  rb_init();
#endif
}

bool ff_is_bus_adv(sl_bt_evt_scanner_extended_advertisement_report_t *report)
{
  // Ensure complete advertisement
  if (!report->data_completeness == sl_bt_scanner_data_status_complete) {
      return false;
  }

#ifndef DEBUG_NO_RECENT_BUS
  // Check if in Recent Bus list
  if (rb_in_list(report->address.addr)) {
      return false;
  }
#endif

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

void ff_rx_data(uint8_t connection, uint8array* data) {
  if (data->len < sizeof(uint32_t)) {
      // The message is too short
      app_log_info("Message received is too short: real size [%d] minimum size [%d]\n\r",
                   data->len, sizeof(uint32_t));
  }
  pt_msg_t* msg = (pt_msg_t*)data->data;

  switch (msg->type) {
    case pt_msg_req_type:
      ff_msg_req(connection, msg);
      break;

    case pt_msg_fare_id_type:
      // Unhandled in FareFob
      break;

    default:
      app_log_info("Unknown message type: 0x%08X\n\r", msg->type);
      break;
  }
}

sl_status_t ff_tx_data(uint8_t connection, pt_msg_t* msg) {
  uint8_t len = pt_sizeof(msg);
  uint8array* data = malloc(sizeof(uint8array) + (len*sizeof(uint8_t)));
  data->len = len;
  memcpy(data->data, msg, len);
  // TODO: Add some error handling and retries
  sl_status_t ret_val = btc_connect_tx_data(connection, data);
  free(data);
  return ret_val;
}

void ff_msg_req(uint8_t connection, pt_msg_t* msg) {
  switch (msg->data.request.req) {
    case pt_req_fare_id:
      pt_msg_t msg;
      msg.type = pt_msg_fare_id_type;
      //app_log("UUID: ");
      for (uint8_t i = 0; i < UUID_LEN; i++) {
          //app_log("0x%02X ", fare_uuid[i]);
          msg.data.fare_id.uuid[i] = fare_uuid[i];
      }
      //app_log("\n\rDone\n\r");
      sl_status_t ret_val = ff_tx_data(connection, &msg);
      if (ret_val != SL_STATUS_OK) {
          app_log_info("Failed to send fare id message to connection %d: %lu\n\r", connection, ret_val);
      }
      break;

    case pt_req_done:
      app_log_info("Received done request\n\r");
      btc_connect_disconnect(connection);

#ifndef DEBUG_NO_RECENT_BUS
      rb_add_bus_to_list(btc_connect_get_address(connection));
#endif
      break;

    default:
      app_log_info("Unhandled request id:");
      break;
  }
}
