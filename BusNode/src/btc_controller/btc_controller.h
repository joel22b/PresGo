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

#include "bluenrg_lp_stack.h"

void btc_init();
void btc_tick();
uint8_t btc_address_match(uint8_t* addr1, uint8_t* addr2);

void btc_connect_init();
void btc_connect_tick();
btc_connection_t* btc_connect_get(uint8_t index);
btc_connection_t* btc_connect_get_connection(uint16_t connection);
void btc_connect_request(uint8_t reqId, uint8_t* addr);
void btc_connect_fare_request(uint8_t reqId, uint8_t* addr);
void btc_connect_start(uint8_t addrType, uint8_t* addr);
void btc_connect_finish(uint16_t connection, uint8_t* addr);
void btc_connect_disconnect(btc_connection_t* conn);
void btc_connect_proc_complete(btc_connection_t* conn, uint8_t error);
void btc_connect_discover_services(btc_connection_t* conn);
void btc_connect_discover_characteristics(btc_connection_t* conn);
void btc_connect_enable_notifications(btc_connection_t* conn);
void btc_connect_tx_request(btc_connection_t* conn, pt_req_t reqType);
void btc_connect_tx_data(btc_connection_t* conn, uint8_t* data, uint16_t len);
void btc_connect_rx_data(btc_connection_t* conn, uint8_t* data, uint16_t len);
void btc_connect_cleanup(btc_connection_t* conn);
void btc_connect_timeout_connect(void* data);
void btc_connect_timeout_fare(void* data);
uint8_t btc_connect_should_scan();
void btc_connect_disconnect_scanning();

void btc_adv_init();
void btc_adv_start();
void btc_adv_stop();
void btc_adv_scan_start();
void btc_adv_scan_stop();
void btc_adv_callback(uint8_t Num_Reports, Advertising_Report_t Advertising_Report[]);
uint8_t btc_adv_match(uint8_t* addr);

void btc_event_init();
btc_event_t* btc_event_next(uint8_t* index);
void btc_event_free(uint8_t index);
void btc_event_add(btc_event_t* event);

/*void btc_connect_request(uint8_t reqId, uint8_t* addr);
void btc_connect_start(uint16_t connection, uint8_t* addr);
void btc_connect_set_rx(uint16_t connection, uint16_t rxHandle);
void btc_connect_finish(uint16_t connection);
void btc_connect_disconnect(uint16_t connection);
void btc_connect_failure(uint16_t connection);

void btc_connect_timeout(void* data);

void btc_connect_tx_request(uint16_t connection, pt_req_t reqType);

void btc_connect_tx_data(uint16_t connection, uint8_t* data, uint16_t len);
void btc_connect_rx_data(uint16_t connection, uint8_t* data, uint16_t len);*/

#endif /* SRC_BTC_CONTROLLER_BTC_CONTROLLER_H_ */
