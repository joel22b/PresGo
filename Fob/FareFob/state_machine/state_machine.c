/*
 * state_machine.c
 *
 *  Created on: Jan. 20, 2024
 *      Author: Joel
 */

#include "state_machine.h"

#include "sl_bluetooth.h"
#include "app_log.h"
#include "sl_simple_led_instances.h"

// Variables
sm_state_t curr_state;
sm_state_t next_state;
sl_sleeptimer_timer_handle_t timer;
bool next_state_locked;
uint8_t* advertising_set_handle;

// Function Definitions
void sm_init() {
  curr_state = SM_STATE_BOOT;
  next_state = SM_STATE_BOOT;
  next_state_locked = false;
  advertising_set_handle = NULL;
}

void sm_set_adv_handle(uint8_t* advertising_set_handle_) {
  advertising_set_handle = advertising_set_handle_;
}

void sm_process() {
  if (next_state != curr_state) {
      switch (curr_state) {
        case SM_STATE_BOOT:
          // Only valid transition is check for bus
          if (next_state == SM_STATE_CHECK_FOR_BUS) {
              curr_state = SM_STATE_CHECK_FOR_BUS;
              sm_state_check_for_bus_enter();
          }
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

// State specific functions
void sm_state_boot_exit() {
  sl_status_t ret_val;
  ret_val = sl_bt_advertiser_stop(*advertising_set_handle);
  if (ret_val != SL_STATUS_OK)
  {
      app_log_info("Failed to stop bluetooth advertisements: return value = %d\n\r",
                   ret_val);
  }
}

void sm_state_check_for_bus_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sm_update_state(SM_STATE_SLEEP, false);

  (void)(handle);
  (void)(data);
}

void sm_state_check_for_bus_enter() {
  sl_status_t ret_val;
  ret_val = sl_bt_scanner_start(0x5, 0x2);
  if (ret_val != SL_STATUS_OK)
  {
      app_log_info("Failed to start bluetooth scanner: return value = %d\n\r",
                   ret_val);
  }

  // Set timer for check bus state
  uint32_t timer_timeout = sl_sleeptimer_ms_to_tick(SM_DURATION_CHECK_FOR_BUS);

  ret_val = sl_sleeptimer_start_timer(&timer,
                                     timer_timeout,
                                     sm_state_check_for_bus_callback,
                                     (void *)NULL,
                                     10,
                                     0);
  if (ret_val != SL_STATUS_OK)
  {
      app_log_info("Failed to start timer for SM_STATE_CHECK_FOR_BUS: return value = %d\n\r",
                   ret_val);
  }
  sl_led_turn_on(&sl_led_led0);
}

void sm_state_check_for_bus_exit() {
  sl_status_t ret_val;
  ret_val = sl_bt_scanner_stop();
  if (ret_val != SL_STATUS_OK)
  {
      app_log_info("Failed to stop bluetooth scanner: return value = %d\n\r",
                       ret_val);
  }

  // Ensure timer is done or stop it
  bool timerRunning = true;
  ret_val = sl_sleeptimer_is_timer_running(&timer, &timerRunning);
  if (ret_val != SL_STATUS_OK || timerRunning) {
      ret_val = sl_sleeptimer_stop_timer(&timer);
      if (ret_val != SL_STATUS_OK) {
          app_log_info("Failed to stop timer for SM_STATE_CHECK_FOR_BUS: return value = %d\n\r",
                       ret_val);
      }
  }
  sl_led_turn_off(&sl_led_led0);
}

void sm_state_sleep_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  sm_update_state(SM_STATE_CHECK_FOR_BUS, true);

  (void)(handle);
  (void)(data);
}

void sm_state_sleep_enter() {
  sl_status_t ret_val;
  // Set timer for sleep state
  uint32_t timer_timeout = sl_sleeptimer_ms_to_tick(SM_DURATION_SLEEP);

  ret_val = sl_sleeptimer_start_timer(&timer,
                                      timer_timeout,
                                      sm_state_sleep_callback,
                                      (void *)NULL,
                                      20,
                                      0);
  if (ret_val != SL_STATUS_OK)
  {
      app_log_info("Failed to start timer for SM_STATE_SLEEP: return value = %d\n\r",
                   ret_val);
  }
}

void sm_state_rapid_adv_enter() {
  // TODO: Start advertisements
}

void sm_state_rapid_adv_exit() {
  // TODO: Stop advertisements
}
