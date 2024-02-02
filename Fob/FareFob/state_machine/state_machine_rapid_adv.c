/*
 * state_machine_rapid_adv.c
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#include "state_machine.h"

#include "app_log.h"

#include "bt_controller/bt_controller.h"
#include "fare_fob/config.h"

void sm_state_rapid_adv_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sm_update_state(SM_STATE_CHECK_FOR_BUS, false);

  (void)(handle);
  (void)(data);
}

void sm_state_rapid_adv_enter() {
  btc_adv_start(btc_adv_services_standard);

#ifndef DEBUG_JUST_ADV
  sl_status_t ret_val = sm_timer_start(SM_DURATION_RAPID_ADV,
                                       sm_state_rapid_adv_callback,
                                       0);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start timer for SM_RAPID_ADV: return value = %d\n\r", ret_val);
  }
#endif
}

void sm_state_rapid_adv_exit() {
  btc_adv_stop(btc_adv_services_standard);

  sl_status_t ret_val = sm_timer_stop();
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to stop timer for SM_RAPID_ADV: return value = %d\n\r", ret_val);
  }
}
