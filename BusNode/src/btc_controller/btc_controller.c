/*
 * btc_controller.c
 *
 *  Created on: Feb. 8, 2024
 *      Author: Joel
 */

#include "btc_controller.h"

#include "stdio.h"
#include "stdlib.h"

#include "app_state.h"
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

/*void btc_connect_request(uint8_t reqId, uint8_t* addr) {
	APP_FLAG_SET(BUTTON1_PRESSED);
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_unknown) {
			btc_connections[i].state = btc_connect_state_scanning;
			btc_connections[i].reqId = reqId;
			HAL_VTIMER_StartTimerMs(&btc_connections[i].timer, RSP_TIMEOUT);
			printf("Address: ");
			for (uint8_t j = 0; j < BTC_ADDRESS_LEN; j++) {
				printf("0x%02X ", addr[j]);
				btc_connections[i].address[j] = addr[j];
			}
			printf("\n\r");
			break;
		}
	}
}

void btc_connect_start(uint16_t connection, uint8_t* addr) {
	uint8_t reqId = 0xFF;
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_connecting) {
			uint8_t match = 1;
			for (uint8_t j = 0; j < BTC_ADDRESS_LEN; j++) {
				if (btc_connections[i].address[j] != addr[j]) {
					match = 0;
					break;
				}
			}
			if (match) {
				btc_connections[i].connection = connection;
				break;
			}
		}
	}
}

void btc_connect_set_rx(uint16_t connection, uint16_t rxHandle) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_connecting &&
				btc_connections[i].connection == connection) {
			btc_connections[i].rx = rxHandle;
			break;
		}
	}
}

void btc_connect_finish(uint16_t connection) {
	printf("Connection Done\n\r");
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_connecting &&
				btc_connections[i].connection == connection) {
			printf("Found connection\n\r");
			btc_connections[i].state = btc_connect_state_connected;

			btc_connect_tx_request(connection, pt_req_fare_id);
			break;
		}
	}
}

void btc_connect_disconnect(uint16_t connection) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].connection == connection) {
			btc_connections[i].state = btc_connect_state_disconnected;
			btc_connections[i].connection = 0;
			printf("Disconnected 0x%04X\n\r", connection);
			break;
		}
	}
}

void btc_connect_failure(uint16_t connection) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].connection == connection) {
			btc_connections[i].state = btc_connect_state_disconnected;
			printf("Failed connection 0x%04X\n\r", connection);
			ps_send_rsp_fare(btc_connections[i].reqId, BTC_UUID_ERROR);
			break;
		}
	}
}

void btc_connect_timeout(void* data) {
	printf("Timeout ptr: 0x%08X\n\r", data);
	btc_connection_t* connection = (btc_connection_t*)(data-8);
	if (connection->state != btc_connect_state_disconnected) {
		printf("Timeout connection: state=0x%02X\n\r", connection->state);
		ps_send_rsp_fare(connection->reqId, BTC_UUID_ERROR);
	}
	connection->connection = 0;
	connection->state = btc_connect_state_unknown;
}

void btc_connect_tx_request(uint16_t connection, pt_req_t reqType) {
	pt_msg_t msg;
	msg.type = pt_msg_req_type;
	msg.data.request.req = reqType;

	uint16_t len = pt_sizeof(&msg);
	uint8_t* data = malloc(len);
	for (uint8_t i = 0; i < len; i++) {
		data[i] = ((uint8_t*)&msg)[i];
	}
	btc_connect_tx_data(connection, data, len);
}

void btc_connect_tx_data(uint16_t connection, uint8_t* data, uint16_t len) {
	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
		if (btc_connections[i].state == btc_connect_state_connected &&
				btc_connections[i].connection == connection) {
			tBleStatus ret = aci_gatt_clt_write_without_resp(connection, btc_connections[i].rx+1, len, data);
			if(ret != BLE_STATUS_SUCCESS) {
				printf("Error sending data to slave %d: 0x%02X (handle 0x%04X)\n\r", i, ret, connection);
				btc_connect_failure(connection);
			}
		}
	}
}

void btc_connect_rx_data(uint16_t connection, uint8_t* data, uint16_t len) {
	if (len < sizeof(uint32_t)) {
		// The message is too short
	    printf("Message received is too short: real size [%d] minimum size [%d]\n\r",
	    		len, sizeof(uint32_t));
	}
	pt_msg_t* msg = (pt_msg_t*)data;

	switch (msg->type) {
		case pt_msg_req_type:
			// Unhandled in Node
			break;

	    case pt_msg_fare_id_type:
	    	for (uint8_t i = 0; i < BTC_CONNECTIONS_NUM; i++) {
	    		if (btc_connections[i].state == btc_connect_state_connected &&
	    				btc_connections[i].connection == connection) {
	    			ps_send_rsp_fare(btc_connections[i].reqId, msg->data.fare_id.uuid);
	    			btc_connect_tx_request(btc_connections[i].connection, pt_req_done);
	    			break;
	    		}
	    	}
	    	break;

	    default:
	    	printf("Unknown message type: 0x%08X\n\r", msg->type);
	    	break;
	}
}*/
