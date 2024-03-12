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
 * DEBUG_NO_RECENT_BUS
 *  Removed the Recent Bus list
 *  feature. Good for testing
==================================== */
//#define DEBUG_JUST_ADV
//#define DEBUG_NO_RECENT_BUS

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
static const uint8_t FARE_DEFAULT_UUID[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04};

/* ====================================
 * Generic Configs
==================================== */
#define UUID_LEN 16
#define ADDRESS_LEN 6

/* =====================================
 * Recent Bus list
 *
 * RB_LIST_LEN
 *  How many entries fit in the Recent
 *  Bus list
 * RB_PERIOD_SEC
 *  The number of seconds for a period
 *  in the Recent Bus list
 * RB_LIST_DURATION_MIN
 *  The number of minutes a bus should
 *  remain on the Recent Bus list
 * RB_PERIOD_MS
 *  Converts RB_PERIOD_SEC to
 *  milliseconds **DO NOT CHANGE**
 * RB_LIST_DURATION_PERIODS
 *  Converts RB_LIST_DURATION_MIN to
 *  a number of periods **DO NOT CHANGE**
===================================== */
#define RB_LIST_LEN               4
#define RB_PERIOD_SEC             30  // 30 seconds
#define RB_LIST_DURATION_MIN      90  // 1.5 hours
#define RB_PERIOD_MS              RB_PERIOD_SEC*1000
#define RB_LIST_DURATION_PERIODS  (60/RB_PERIOD_SEC)*RB_LIST_DURATION_MIN

#endif /* CONFIG_H_ */
