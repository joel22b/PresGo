/*
 * state_machine_sleep.c
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#include "state_machine.h"

#include "fare_fob/config.h"

void sm_state_sleep_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sm_update_state(SM_STATE_CHECK_FOR_BUS, true);

  (void)(handle);
  (void)(data);
}

void sm_state_sleep_enter() {
  sl_status_t ret_val = sm_timer_start(SM_DURATION_SLEEP,
                                       sm_state_sleep_callback,
                                       0);

  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start timer for SM_SLEEP: return value = %lu\n\r", ret_val);
  }
}

void sm_state_sleep_exit() {
  sl_status_t ret_val = sm_timer_stop();
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to stop timer for SM_SLEEP: return value = %lu\n\r", ret_val);
  }
}
