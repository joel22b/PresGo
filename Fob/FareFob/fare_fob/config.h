/*
 * config.h
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdint.h"

/* ====================================
 * Debug Flags
 *
 * Uncomment when desired
 *
 * DEBUG_JUST_ADV:
 *  Sets the tag to immediately go into
 *  advertising and remain advertising
 *  **BAD BATTERY PERFORMANCE**
==================================== */
#define DEBUG_JUST_ADV

/* ====================================
 * State Machine Timer Durations (in ms)
 *
 * Defaults:
 * SM_DURATION_CHECK_FOR_BUS  100ms
 * SM_DURATION_SLEEP          5000ms
 * SM_DURATION_RAPID_ADV      1000ms
==================================== */
#define SM_DURATION_CHECK_FOR_BUS 100   //ms
#define SM_DURATION_SLEEP         5000  //ms
#define SM_DURATION_RAPID_ADV     1000  //ms

/* ====================================
 * Bus Advertisement Identifier Data
 *
 * IDENTIFER_BT_DATA_START
 *  The character that proceeds the
 *  name in an extended advertisement
 *  packet
 * IDENTIFER_BT_DATA_BUS
 *  The name that is searched for in
 *  every extended advertisement
 * IDENTIFER_BT_DATA_BUS_LEN
 *  The length of IDENTIFER_BT_DATA_BUS
==================================== */
#define IDENTIFER_BT_DATA_START 0x09
#define IDENTIFER_BT_DATA_BUS "BUSCONNECTION1"
#define IDENTIFER_BT_DATA_BUS_LEN 14

/* ====================================
 * Fare Data
 *
 * FARE_DEFAULT_UUID
 *  The UUID of this FareFob
 *  TODO: Make this not hardcoded
 *    and stored permanently
==================================== */
static const uint8_t FARE_DEFAULT_UUID[] = {0xAA, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0xFF};

/* ====================================
 * Generic Configs
==================================== */
#define UUID_LEN 16

#endif /* CONFIG_H_ */
