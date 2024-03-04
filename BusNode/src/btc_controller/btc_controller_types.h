/*
 * btc_controller_types.h
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#ifndef SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_
#define SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_

#include "stdint.h"

#include "rf_driver_hal_vtimer.h"

#define BTC_ADDRESS_LEN 6
#define BTC_CONNECTIONS_NUM 4
#define BTC_EVENTS_NUM 8
#define BTC_MAX_DATA_LENGTH 64

static const uint8_t BTC_UUID_ERROR[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

typedef enum {
  btc_connect_state_empty	 				= 0x00,
  btc_connect_state_scanning 				= 0x01,
  btc_connect_state_connecting				= 0x02,
  btc_connect_state_discover_config			= 0x04,
  btc_connect_state_discover_service		= 0x05,
  btc_connect_state_discover_characteristic	= 0x06,
  btc_connect_state_enable_notifications	= 0x07,
  btc_connect_state_connected				= 0x08,
  btc_connect_state_disconnecting			= 0x10,
  btc_connect_state_failed					= 0x20,
} btc_connect_state_t;

struct btc_connection {
	btc_connect_state_t state;
	uint8_t reqId_connect;
	uint8_t reqId_fare;
	uint16_t connection;
	uint16_t service_start;
	uint16_t service_end;
	uint16_t tx;
	uint16_t rx;
	VTIMER_HandleType timer;
	uint8_t ps_rsp_connect;
	uint8_t ps_rsp_fare;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_connection btc_connection_t;

typedef enum {
	btc_event_unknown		= 0x00,
	btc_event_connect		= 0x01,
	btc_event_disconnect	= 0x02,
	btc_event_proc_complete	= 0x03,
	btc_event_rx_data		= 0x04,
} btc_event_id;

struct btc_event_connect {
	uint16_t connection;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_event_connect btc_event_connect_t;

struct btc_event_disconnect {
	uint16_t connection;
	uint8_t reason;
};
typedef struct btc_event_disconnect btc_event_disconnect_t;

struct btc_event_proc_complete {
	uint16_t connection;
	uint8_t error;
};
typedef struct btc_event_proc_complete btc_event_proc_complete_t;

struct btc_event_rx_data {
	uint16_t connection;
	uint16_t attribute;
	uint16_t dataLength;
	uint8_t data[BTC_MAX_DATA_LENGTH];
};
typedef struct btc_event_rx_data btc_event_rx_data_t;

struct btc_event {
	btc_event_id id;
	union {
		btc_event_connect_t 		connect;
		btc_event_disconnect_t 		disconnect;
		btc_event_proc_complete_t 	proc_complete;
		btc_event_rx_data_t 		rx_data;
	};
};
typedef struct btc_event btc_event_t;

/*struct btc_scan_adv {
	btc_connect_state_t state;
	uint8_t reqId;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_scan_adv btc_scan_adv_t;*/

#endif /* SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_ */
