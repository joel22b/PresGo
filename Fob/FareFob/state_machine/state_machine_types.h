/*
 * state_machine_types.h
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#ifndef STATE_MACHINE_TYPES_H_
#define STATE_MACHINE_TYPES_H_

// Return codes
#define SM_SUCCESS 0
#define SM_INVALID_SWITCH 1
#define SM_UNKNOWN_STATE_CURRENT 2
#define SM_UNKNOWN_STATE_NEXT 3

// State definitions
typedef enum {
  SM_STATE_BOOT = 0,
  SM_STATE_CHECK_FOR_BUS,
  SM_STATE_SLEEP,
  SM_STATE_RAPID_ADV,
} sm_state_t;



#endif /* STATE_MACHINE_TYPES_H_ */
