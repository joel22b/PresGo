/*
 * protocol_serial.c
 *
 *  Created on: Feb. 6, 2024
 *      Author: Joel
 */

#include "protocol_serial.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "protocol_serial_types.h"
#include "btc_controller.h"

uint8_t ps_is_init = 0;

#define PS_MSG_BUFFER_LEN 128
#define PS_MSG_NUM 8
uint8_t ps_msg_buffer[PS_MSG_BUFFER_LEN];
uint8_t ps_msg_buffer_index = 0;
uint8_t ps_msg_buffer_record = 0;
uint8_t ps_msg[PS_MSG_NUM][PS_MSG_BUFFER_LEN];
uint8_t ps_msg_len[PS_MSG_NUM];

void ps_recv_callback(uint8_t* data, uint16_t len) {
	//printf("Len=%d\n\r", len);
	if (len > 0) {
		for (uint16_t i = 0; i < len; i++) {
			ps_recv_byte(data[i]);
		}
	}
}

void ps_recv_byte(uint8_t byte) {
	//printf("byte [%c] index [%d] record [%d]\n\r", byte, ps_msg_buffer_index, ps_msg_buffer_record);
	if (byte == PS_START_CHAR) {
		ps_msg_buffer_index = 0;
		ps_msg_buffer[ps_msg_buffer_index++] = byte;
		ps_msg_buffer_record = 1;
	}
	else if (ps_msg_buffer_record) {
		ps_msg_buffer[ps_msg_buffer_index++] = byte;
		if (byte == PS_END_CHAR) {
			ps_recv_add_msg();
			ps_msg_buffer_record = 0;
		}
	}
	//ps_msg_buffer[ps_msg_buffer_index++] = byte;
}

void ps_recv_add_msg() {
	if (ps_is_init) {
		for (uint8_t i = 0; i < PS_MSG_NUM; i++) {
			if (ps_msg_len[i] == 0) {
				ps_msg_len[i] = ps_msg_buffer_index;
				for (uint8_t j = 0; j < ps_msg_len[i]; j++) {
					ps_msg[i][j] = ps_msg_buffer[j];
				}
				return;
			}
		}
		// Error no spot found
		printf("Error no spot found in ps_msg");
	}
	else {
		printf("Error not initialized\n\r");
	}
}

void ps_init() {
	for (uint8_t i = 0; i < PS_MSG_NUM; i++) {
		ps_msg_len[i] = 0;
	}
	ps_is_init = 1;
	printf("Protocol Serial Initialized\n\r");
}

void ps_process() {
	if (ps_is_init) {
		for (uint8_t i = 0; i < PS_MSG_NUM; i++) {
			if (ps_msg_len[i] != 0) {
				ps_process_msg(i);
				ps_msg_len[i] = 0;
			}
		}
	}
}

void ps_process_msg(uint8_t index) {
	//printf("%.*s\r", ps_msg_len[index], ps_msg[index]);
	if (ps_msg_len[index] < PS_MIN_MSG) {
		// Invalid message, ignore
		return;
	}

	// Parse start char
	uint8_t parse_index = 0;
	if (ps_msg[index][parse_index] != PS_START_CHAR) {
		// Missing start character, ignore
		return;
	}
	parse_index++;

	// Parse identifier
	if (ps_cmp(&ps_msg[index][parse_index], PS_IDENTIFIER_REQUEST, PS_IDENTIFIER_LEN)) {
		parse_index += PS_IDENTIFIER_LEN;
		if (ps_msg[index][parse_index] != PS_SEPARATOR_CHAR) {
			// Missing separator, ignore
			return;
		}
		parse_index++;

		// Parse request id
		uint8_t reqId = ps_to_uint8(&ps_msg[index][parse_index], PS_REQUEST_ID_LEN);
		if (reqId == 255) {
			// Failed to parse, ignore
			return;
		}
		parse_index += PS_REQUEST_ID_LEN;

		if (ps_msg[index][parse_index] != PS_SEPARATOR_CHAR) {
			// Missing separator, ignore
			return;
		}
		parse_index++;

		// Parse type
		if (ps_cmp(&ps_msg[index][parse_index], PS_TYPE_FARE, PS_TYPE_LEN)) {
			parse_index += PS_TYPE_LEN + 1;
			// Request for Fare ID
			uint8_t addr[BTC_ADDRESS_LEN];
			for (uint8_t i = 0; i < BTC_ADDRESS_LEN; i++) {
				addr[BTC_ADDRESS_LEN - i - 1] = ps_str_to_hex(&ps_msg[index][parse_index + (i*2)]);
			}
			btc_connect_request(reqId, addr);
			//uint8_t uuid[] = {0xAA, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0xFF};
			//ps_send_rsp_fare(reqId, uuid);
		}
		else if (ps_cmp(&ps_msg[index][parse_index], PS_TYPE_DOOR, PS_TYPE_LEN)) {
			// Request for Door
			ps_send_rsp_door(reqId, 1);
		}
		else if (ps_cmp(&ps_msg[index][parse_index], PS_TYPE_DIST, PS_TYPE_LEN)) {
			// Request for Distance
			ps_send_rsp_dist(reqId, 10);
		}
	}
}
