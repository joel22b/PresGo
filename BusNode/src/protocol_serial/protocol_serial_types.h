/*
 * protocol_serial_types.h
 *
 *  Created on: Feb. 6, 2024
 *      Author: Joel
 */

#ifndef PROTOCOL_SERIAL_TYPES_H_
#define PROTOCOL_SERIAL_TYPES_H_

// \t + 3 char identifier + : + 4 char type
#define PS_MIN_MSG	9

#define PS_START_CHAR		'\t'
#define PS_SEPARATOR_CHAR	':'
#define PS_END_CHAR			'\n'

#define PS_IDENTIFIER_LEN			3
#define PS_IDENTIFIER_REQUEST		"Req"
#define PS_IDENTIFIER_RESPONSE		"Rsp"
#define PS_IDENTIFIER_ANNOUNCEMENT	"Ann"

#define PS_REQUEST_ID_LEN	2

#define PS_TYPE_LEN		4
#define PS_TYPE_CONNECT	"Conn"
#define PS_TYPE_FARE	"Fare"
#define PS_TYPE_DIST	"Dist"
#define PS_TYPE_DOOR	"Door"
#define PS_TYPE_INIT	"Init"

// INIT FLAGS
#define PS_INIT_EMPTY					0x00
#define PS_INIT_DISTANCE_SENSOR			0x01
#define PS_INIT_BLUETOOTH_CONTROLLER	0x02

#endif /* PROTOCOL_SERIAL_TYPES_H_ */
