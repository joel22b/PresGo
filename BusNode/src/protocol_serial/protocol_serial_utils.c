/*
 * protocol_serial_utils.c
 *
 *  Created on: Feb. 7, 2024
 *      Author: Joel
 */

#include "protocol_serial.h"

#include "stdio.h"

uint8_t ps_cmp(uint8_t* a, uint8_t* b, uint8_t len) {
	//printf("Compare\n\r");
	for (uint8_t i = 0; i < len; i++) {
		//printf("i [%d] a[%c] b[%c]\n\r", i, a[i], b[i]);
		if (a[i] != b[i]) {
			return 0;
		}
	}
	//printf("Success\n\r");
	return 1;
}

uint8_t ps_to_uint8(uint8_t* in, uint8_t len) {
	uint8_t total = 0;
	for (uint8_t i = 0; i < len; i++) {
		switch(in[i]) {
			case '0':
				total *= 10;
				break;
			case '1':
				total *= 10;
				total += 1;
				break;
			case '2':
				total *= 10;
				total += 2;
				break;
			case '3':
				total *= 10;
				total += 3;
				break;
			case '4':
				total *= 10;
				total += 4;
				break;
			case '5':
				total *= 10;
				total += 5;
				break;
			case '6':
				total *= 10;
				total += 6;
				break;
			case '7':
				total *= 10;
				total += 7;
				break;
			case '8':
				total *= 10;
				total += 8;
				break;
			case '9':
				total *= 10;
				total += 9;
				break;
			default:
				return 255;
		}
	}
	return total;
}

uint8_t ps_bool_to_str(uint8_t bool) {
	if (bool) {
		return 'T';
	}
	return 'F';
}

uint8_t ps_str_to_hex(uint8_t* byte) {
	uint8_t upper = ps_char_to_hex(byte[0]);
	upper = upper << 4;
	uint8_t lower = ps_char_to_hex(byte[1]);
	return upper + lower;
}

uint8_t ps_char_to_hex(uint8_t chr) {
	if (chr >= 0x30 && chr < 0x40) {
		// 0-9
		return chr - 0x30;
	}
	if (chr >= 0x41 && chr < 0x47) {
		// A-F
		return chr + 10 - 0x41;
	}
	if (chr >= 0x61 && chr < 0x67) {
		// a-f
		return chr + 10 - 0x61;
	}
	// Not valid
	return 0;
}
