/*
 * protocol_serial_send.c
 *
 *  Created on: Feb. 7, 2024
 *      Author: Joel
 */

#include "protocol_serial.h"

#include "stdio.h"

#include "protocol_serial_types.h"

void ps_send_rsp_fare(uint8_t reqId, uint8_t* uuid) {
	printf("\t%.*s%c%02X%c%.*s%c",
			PS_IDENTIFIER_LEN, PS_IDENTIFIER_RESPONSE,
			PS_SEPARATOR_CHAR,
			reqId,
			PS_SEPARATOR_CHAR,
			PS_TYPE_LEN, PS_TYPE_FARE,
			PS_SEPARATOR_CHAR);
	for (uint8_t i = 0; i < UUID_LEN; i++) {
		printf("%02X", uuid[i]);
	}
	printf("\n\r");
}

void ps_send_rsp_dist(uint8_t reqId, uint32_t distance) {
	printf("\t%.*s%c%02X%c%.*s%c%d\n\r",
				PS_IDENTIFIER_LEN, PS_IDENTIFIER_RESPONSE,
				PS_SEPARATOR_CHAR,
				reqId,
				PS_SEPARATOR_CHAR,
				PS_TYPE_LEN, PS_TYPE_DIST,
				PS_SEPARATOR_CHAR,
				distance);
}

void ps_send_rsp_door(uint8_t reqId, uint8_t inDoorway) {
	printf("\t%.*s%c%02X%c%.*s%c%c\n\r",
				PS_IDENTIFIER_LEN, PS_IDENTIFIER_RESPONSE,
				PS_SEPARATOR_CHAR,
				reqId,
				PS_SEPARATOR_CHAR,
				PS_TYPE_LEN, PS_TYPE_DOOR,
				PS_SEPARATOR_CHAR,
				ps_bool_to_str(inDoorway));
}

void ps_send_ann_door(uint8_t inDoorway) {
	printf("\t%.*s%c%.*s%c%c\n\r",
				PS_IDENTIFIER_LEN, PS_IDENTIFIER_ANNOUNCEMENT,
				PS_SEPARATOR_CHAR,
				PS_TYPE_LEN, PS_TYPE_DOOR,
				PS_SEPARATOR_CHAR,
				ps_bool_to_str(inDoorway));
}
