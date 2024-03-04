/*
 * state_machine_check_for_bus.c
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#include "state_machine.h"

#include "app_log.h"

#include "bt_controller/bt_controller.h"
#include "fare_fob/config.h"

void sm_state_check_for_bus_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sm_update_state(SM_STATE_SLEEP, false);

  (void)(handle);
  (void)(data);
}

void sm_state_check_for_bus_enter() {
  btc_adv_scanner_start();

  sl_status_t ret_val = sm_timer_start(SM_DURATION_CHECK_FOR_BUS,
                                       sm_state_check_for_bus_callback,
                                       0);

  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start timer for SM_CHECK_FOR_BUS: return value = %lu\n\r", ret_val);
  }
}

void sm_state_check_for_bus_exit() {
  btc_adv_scanner_stop();

  sl_status_t ret_val = sm_timer_stop();
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to stop timer for SM_CHECK_FOR_BUS: return value = %lu\n\r", ret_val);
  }
}
