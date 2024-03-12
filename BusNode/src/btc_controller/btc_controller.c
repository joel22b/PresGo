/*
 * btc_controller.c
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"

#include "ble_status.h"
#include "bluenrg_lp_stack.h"

#include "protocol_serial.h"
#include "config.h"

//static btc_scan_adv_t btc_scan_advs[BTC_CONNECTIONS_NUM];

void btc_init() {
	//printf("Initialization: BLE started\n\r");

	aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN, BTC_ADDRESS);

	/* Set the TX power to 0 dBm */
	aci_hal_set_tx_power_level(0, 24);

	btc_connect_init();
	btc_adv_init();
	btc_event_init();

	//printf("Initialization: BLE complete\n\r");
}

void btc_tick() {
	btc_connect_tick();
}

uint8_t btc_address_match(uint8_t* addr1, uint8_t* addr2) {
	for (uint8_t i = 0; i < BTC_ADDRESS_LEN; i++) {
		if (addr1[i] != addr2[i]) {
			return 0;
		}
	}
	return 1;
}
