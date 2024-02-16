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
	uint8_t reqId;
	uint16_t connection;
	uint16_t service_start;
	uint16_t service_end;
	uint16_t tx;
	uint16_t rx;
	VTIMER_HandleType timer;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_connection btc_connection_t;

/*struct btc_scan_adv {
	btc_connect_state_t state;
	uint8_t reqId;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_scan_adv btc_scan_adv_t;*/

#endif /* SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_ */
