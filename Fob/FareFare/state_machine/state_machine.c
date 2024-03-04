/*
 * state_machine.c
 *
 *  Created on: Jan. 20, 2024
 *      Author: Joel
 */

#include "state_machine.h"

#include "sl_status.h"

//#include "fare_fob/config.h"

// Variables
sm_state_t curr_state;
sm_state_t next_state;
sl_sleeptimer_timer_handle_t timer;
bool next_state_locked;

// Function Definitions
void sm_init() {
  curr_state = SM_STATE_BOOT;
  next_state = SM_STATE_BOOT;
  next_state_locked = false;
}

void sm_process() {
  if (next_state != curr_state) {
      switch (curr_state) {
        case SM_STATE_BOOT:
          // Only valid transition is check for bus
          if (next_state == SM_STATE_CHECK_FOR_BUS) {
              sm_state_boot_exit();
              curr_state = SM_STATE_CHECK_FOR_BUS;
              sm_state_check_for_bus_enter();
          }
#ifdef DEBUG_JUST_ADV
          else if (next_state == SM_STATE_RAPID_ADV) {
              sm_state_boot_exit();
              curr_state = SM_STATE_RAPID_ADV;
              sm_state_rapid_adv_enter();
          }
#endif
          break;
        case SM_STATE_CHECK_FOR_BUS:
          if (next_state == SM_STATE_SLEEP) {
              sm_state_check_for_bus_exit();
              curr_state = SM_STATE_SLEEP;
              sm_state_sleep_enter();
          }
          else if (next_state == SM_STATE_RAPID_ADV) {
              sm_state_check_for_bus_exit();
              curr_state = SM_STATE_RAPID_ADV;
              sm_state_rapid_adv_enter();
          }
          break;
        case SM_STATE_SLEEP:
          // Only valid transition is check for bus
          if (next_state == SM_STATE_CHECK_FOR_BUS) {
              sm_state_sleep_exit();
              curr_state = SM_STATE_CHECK_FOR_BUS;
              sm_state_check_for_bus_enter();
          }
          break;
        case SM_STATE_RAPID_ADV:
          // Only valid transition is check for bus
          if (next_state == SM_STATE_CHECK_FOR_BUS) {
              sm_state_rapid_adv_exit();
              curr_state = SM_STATE_CHECK_FOR_BUS;
              sm_state_check_for_bus_enter();
          }
          break;
        default:
          break;
      }
      next_state = curr_state;
      next_state_locked = false;
  }
}

void sm_update_state(sm_state_t newState, bool override) {
  if (override || !next_state_locked) {
      next_state_locked = true;
      next_state = newState;
  }
}

sl_status_t sm_timer_start(uint16_t duration_ms,
                           sl_sleeptimer_timer_callback_t callback_func,
                           void* callback_data) {
  uint32_t timer_timeout = sl_sleeptimer_ms_to_tick(duration_ms);

  sl_status_t ret_val = sl_sleeptimer_start_timer(&timer,
                                                  timer_timeout,
                                                  callback_func,
                                                  callback_data,
                                                  20,
                                                  0);
    return ret_val;
}

sl_status_t sm_timer_stop() {
  // Ensure timer is done or stop it
  bool timerRunning = true;
  sl_status_t ret_val = sl_sleeptimer_is_timer_running(&timer, &timerRunning);
  if (ret_val != SL_STATUS_OK || timerRunning) {
      ret_val = sl_sleeptimer_stop_timer(&timer);
  }
  return ret_val;
}
