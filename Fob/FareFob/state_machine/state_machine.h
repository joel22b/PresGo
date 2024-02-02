#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "stdint.h"
#include <stdbool.h>

// Return codes
#define SM_SUCCESS 0
#define SM_INVALID_SWITCH 1
#define SM_UNKNOWN_STATE_CURRENT 2
#define SM_UNKNOWN_STATE_NEXT 3

// Timer duration in ms
#define SM_DURATION_CHECK_FOR_BUS 100
#define SM_DURATION_RAPID_ADV 1000
#define SM_DURATION_SLEEP 5000

// State definitions
typedef enum {
  SM_STATE_BOOT = 0,
  SM_STATE_CHECK_FOR_BUS,
  SM_STATE_SLEEP,
  SM_STATE_RAPID_ADV,
} sm_state_t;

void sm_init();

void sm_set_adv_handle(uint8_t* advertising_set_handle_);

void sm_process();

void sm_update_state(sm_state_t newState, bool override);

// State specific functions
void sm_state_boot_exit();

void sm_state_check_for_bus_enter();
void sm_state_check_for_bus_exit();

void sm_state_sleep_enter();

void sm_state_rapid_adv_enter();
void sm_state_rapid_adv_exit();

#endif // STATE_MACHINE_H
