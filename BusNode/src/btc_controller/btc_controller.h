/*
 * btc_controller.h
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#ifndef SRC_BTC_CONTROLLER_BTC_CONTROLLER_H_
#define SRC_BTC_CONTROLLER_BTC_CONTROLLER_H_

#include "btc_controller_types.h"
#include "protocol.h"

void btc_init();

uint8_t btc_scan_adv_match(uint8_t* addr);

void btc_connect_request(uint8_t reqId, uint8_t* addr);
void btc_connect_start(uint16_t connection, uint8_t* addr);
void btc_connect_set_rx(uint16_t connection, uint16_t rxHandle);
void btc_connect_finish(uint16_t connection);
void btc_connect_disconnect(uint16_t connection);

void btc_connect_tx_request(uint16_t connection, pt_req_t reqType);

void btc_connect_tx_data(uint16_t connection, uint8_t* data, uint16_t len);
void btc_connect_rx_data(uint16_t connection, uint8_t* data, uint16_t len);

#endif /* SRC_BTC_CONTROLLER_BTC_CONTROLLER_H_ */
