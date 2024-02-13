/*
 * btc_controller_types.h
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#ifndef SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_
#define SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_

#include "stdint.h"

#define BTC_ADDRESS_LEN 6
#define BTC_CONNECTIONS_NUM 4

typedef enum {
  btc_connect_state_unknown 		= 0x00,
  btc_connect_state_scanning 		= 0x01,
  btc_connect_state_connecting		= 0x02,
  btc_connect_state_connected		= 0x03,
  btc_connect_state_disconnected	= 0x04,
  btc_connect_state_complete		= 0x08,
} btc_connect_state_t;

struct btc_connection {
	btc_connect_state_t state;
	uint8_t reqId;
	uint16_t connection;
	uint16_t rx;
};
typedef struct btc_connection btc_connection_t;

struct btc_scan_adv {
	btc_connect_state_t state;
	uint8_t reqId;
	uint8_t address[BTC_ADDRESS_LEN];
};
typedef struct btc_scan_adv btc_scan_adv_t;

#endif /* SRC_BTC_CONTROLLER_BTC_CONTROLLER_TYPES_H_ */
