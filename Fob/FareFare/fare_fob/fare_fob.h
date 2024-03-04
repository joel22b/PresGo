/*
 * far_fob.h
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#ifndef FARE_FOB_H_
#define FARE_FOB_H_

#include "stdint.h"
#include <stdbool.h>

#include "sl_bt_api.h"

#include "protocol.h"

void ff_init();
bool ff_is_bus_adv(sl_bt_evt_scanner_extended_advertisement_report_t *report);
void ff_rx_data(uint8_t connection, uint8array* data);
sl_status_t ff_tx_data(uint8_t connection, pt_msg_t* msg);

void ff_msg_req(uint8_t connection, pt_msg_t* msg);

#endif /* FARE_FOB_H_ */
