/*
 * config.h
 *
 *  Created on: Feb. 13, 2024
 *      Author: Joel
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include "ble_const.h"

#define RSP_TIMEOUT_CONNECT 10000
#define RSP_TIMEOUT_FARE 5500

/* ====================================
 * BusNode Address
 *
 * BTC_ADDRESS
 * 	The address of the BusNode that
 * 	is used in the recent bus list on
 * 	the FareFob
==================================== */
#define UUID_LEN 16
#define BTC_ADDRESS_LEN 6
static const uint8_t BTC_ADDRESS[] = { 0x42, 0x55, 0x53, 0x30, 0x30, 0x31 };

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
#define BTC_SCAN_INTERVAL       ((uint16_t)(100/0.625))     // 100 ms
#define BTC_SCAN_WINDOW         ((uint16_t)(100/0.625))     // 100 ms
#define BTC_CONN_INTERVAL_MIN   ((uint16_t)(100/1.25))      // 100 ms
#define BTC_CONN_INTERVAL_MAX   ((uint16_t)(100/1.25))      // 100 ms
#define BTC_SUPERVISION_TIMEOUT ((uint16_t)(1000/10))       // 1000 ms
#define BTC_CE_LENGTH           ((uint16_t)(10/0.625))      // 10 ms
#define BTC_ADV_NAME_START 	0x09
#define BTC_ADV_NAME 		"BUSCONNECTION1"
#define BTC_ADV_NAME_LEN 	14
#define BTC_ADV_INTERVAL    32 /* 20 (Changed) ms */
static const uint8_t BTC_ADV_BASE[] = {
	/* Advertising data: Flags AD Type */
	0x02, 0x01, 0x06,
	/* Advertising data: manufacturer specific data */
	26, //len
	AD_TYPE_MANUFACTURER_SPECIFIC_DATA,  //manufacturer type
	0x00, 0x00, //Company identifier code
	0x02,       // ID
	0x15,       //Length of the remaining payload
	0xE2, 0x0A, 0x39, 0xF4, 0x73, 0xF5, 0x4B, 0xC4, //Location UUID
	0xA1, 0x2F, 0x17, 0xD1, 0xAD, 0x07, 0xA9, 0x61,
	0x00, 0x05, // Major number
	0x00, 0x07, // Minor number
	(uint8_t)-56,         // Tx power measured at 1 m of distance (in dBm)
};
#define BTC_ADV_BASE_LEN sizeof(BTC_ADV_BASE)

/* =====================================
 * GATT Services
 *
 * BTC_GATT_SVC_UUID
 * 	The UUID for the main GATT service
===================================== */
static const uint8_t BTC_GATT_SVC_UUID[] = {0xba, 0x5e, 0xd6, 0x4a, 0xf0, 0x8f, 0x16, 0xa9, 0xe3, 0x41, 0xba, 0xca, 0x09, 0x98, 0xe8, 0xc3};
static const uint8_t BTC_GATT_CHR_TX_UUID[] = {0xe5, 0x2d, 0x86, 0xc8, 0x4b, 0x50, 0x2c, 0xa1, 0x9d, 0x4a, 0x16, 0x8c, 0xb5, 0x1c, 0xb5, 0x5a};
static const uint8_t BTC_GATT_CHR_RX_UUID[] = {0xc1, 0x86, 0x2a, 0xa9, 0xf7, 0xf5, 0x30, 0xa8, 0x49, 0x47, 0x0c, 0xed, 0xec, 0x90, 0x71, 0x06};

#endif /* SRC_CONFIG_H_ */
