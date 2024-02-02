/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app_log.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"
#include "sl_simple_led_instances.h"
#include "state_machine/state_machine.h"
#include "bt_controller/bt_controller.h"

#define EM_EVENT_MASK_ALL      (  SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM0 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM0  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM1 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM1  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM2 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM2  \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_ENTERING_EM3 \
                                | SL_POWER_MANAGER_EVENT_TRANSITION_LEAVING_EM3)

void power_transition_log_callback(sl_power_manager_em_t from,
                 sl_power_manager_em_t to)
{
  app_log_info("Power transition occurred: from EM%d to EM%d\n\r", from, to);
}

sl_power_manager_em_transition_event_handle_t power_transition_log_event_handle;
sl_power_manager_em_transition_event_info_t power_transition_log_event_info = {
  .event_mask = EM_EVENT_MASK_ALL,
  .on_event = power_transition_log_callback,
};

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////

  sl_led_init(&sl_led_led0);
  sl_led_turn_off(&sl_led_led0);
  //sl_led_turn_on(&sl_led_led0);

  //sl_power_manager_subscribe_em_transition_event(&power_transition_log_event_handle, &power_transition_log_event_info);

  sm_init();
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////

  sm_process();
  //sl_power_manager_debug_print_em_requirements();
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  btc_handle_event(evt);
}
