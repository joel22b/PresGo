/*
 * config.h
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#ifndef CONFIG_H_
#define CONFIG_H_

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

#endif /* CONFIG_H_ */
