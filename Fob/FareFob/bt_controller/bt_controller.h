/*
 * bt.controller.h
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#ifndef BT_CONTROLLER_H_
#define BT_CONTROLLER_H_

#include "sl_bluetooth.h"

#include "bt_controller_types.h"

/* ===========================================
 * Bluetooth Event Callback Function
 *
 * Defined in bt_controller.c
=========================================== */
void btc_handle_event(sl_bt_msg_t *evt);

/* ===========================================
 * Stack Initialization Function
 *
 * Defined in bt_controller_init.c
=========================================== */
void btc_init_stack(sl_bt_evt_system_boot_t* evt_system_boot);

/* ===========================================
 * Connection Functions
 *
 * Defined in bt_controller_connect.c
=========================================== */
void btc_connect_init();
void btc_connect_openned(sl_bt_evt_connection_opened_t* evt_connect_openned);
void btc_connect_closed(sl_bt_evt_connection_closed_t* evt_connect_closed);
void btc_connect_parameters(sl_bt_evt_connection_parameters_t* evt_connect_parameters);
void btc_connect_gatt_service(sl_bt_evt_gatt_service_t* evt_gatt_service);
void btc_connect_gatt_characteristic(sl_bt_evt_gatt_server_characteristic_status_t* evt_gatt_characteristic);
sl_status_t btc_connect_tx_data(uint8_t connection, uint8array* data);
void btc_connect_rx_data(sl_bt_evt_gatt_server_user_write_request_t* evt_write_request);

/* ===========================================
 * Advertisement Functions
 *
 * Defined in bt_controller_adv.c
=========================================== */
void btc_adv_init();
void btc_adv_start(btc_adv_services_t adv_services);
void btc_adv_stop(btc_adv_services_t adv_services);
void btc_adv_scanner_start();
void btc_adv_scanner_stop();
void btc_adv_extended(sl_bt_evt_scanner_extended_advertisement_report_t* evt_adv_extended);

#endif /* BT_CONTROLLER_H_ */
