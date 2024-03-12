/*
 * protocol_serial.h
 *
 *  Created on: Feb. 6, 2024
 *      Author: Joel
 */

#ifndef PROTOCOL_SERIAL_H_
#define PROTOCOL_SERIAL_H_

#include "stdint.h"

#define UUID_LEN 16

void ps_recv_callback(uint8_t* data, uint16_t len);
void ps_recv_byte(uint8_t byte);
void ps_recv_add_msg();

void ps_init();
void ps_process();
void ps_process_msg(uint8_t index);

uint8_t ps_cmp(uint8_t* a, uint8_t* b, uint8_t len);
uint8_t ps_to_uint8(uint8_t* in, uint8_t len);
uint8_t ps_bool_to_str(uint8_t bool);
uint8_t ps_str_to_hex(uint8_t* byte);
uint8_t ps_char_to_hex(uint8_t chr);

void ps_send_rsp_connect(uint8_t reqId, uint32_t state);
void ps_send_rsp_fare(uint8_t reqId, uint8_t* uuid);
void ps_send_rsp_dist(uint8_t reqId, uint32_t distance);
void ps_send_rsp_door(uint8_t reqId, uint8_t inDoorway);
void ps_send_ann_door(uint8_t inDoorway);
void ps_send_ann_init(uint8_t flags);
void ps_send_ann_disconnect(uint8_t* addr);

#endif /* PROTOCOL_SERIAL_H_ */
