/*
 * config.h
 *
 *  Created on: Feb. 13, 2024
 *      Author: Joel
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#define RSP_TIMEOUT 6000

/* ====================================
 * BusNode Address
 *
 * BTC_ADDRESS
 * 	The address of the BusNode that
 * 	is used in the recent bus list on
 * 	the FareFob
==================================== */
#define BTC_ADDRESS	{ 0x42, 0x55, 0x53, 0x30, 0x30, 0x31 }

/* ====================================
 * Bus Advertisement Identifier Data
 *
 * BTC_NAME_START	TODO
 *  The character that proceeds the
 *  name in an extended advertisement
 *  packet
 * BTC_NAME			TODO
 *  The name that is searched for in
 *  every extended advertisement
 * BTC_NAME_LEN		TODO
 *  The length of IDENTIFER_BT_DATA_BUS
==================================== */
#define BTC_NAME_START 	0x09
#define BTC_NAME 		"BUSCONNECTION1"
#define BTC_NAME_LEN 	14

#endif /* SRC_CONFIG_H_ */
