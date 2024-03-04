/***************************************************************************//**
 * @file
 * @brief Top level application functions
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app_log.h"
#include "sl_bluetooth.h"

#include "state_machine/state_machine.h"
#include "fare_fob/fare_fob.h"
#include "bt_controller/bt_controller.h"

/***************************************************************************//**
 * Initialize application.
 ******************************************************************************/
void app_init(void)
{
  app_log_info("Initialization started\n\r");
  sm_init();
  ff_init();

  sm_update_state(SM_STATE_CHECK_FOR_BUS, false);
}

/***************************************************************************//**
 * App ticking function.
 ******************************************************************************/
void app_process_action(void)
{
  sm_process();
}

void sl_bt_on_event(sl_bt_msg_t *evt)
{
  btc_handle_event(evt);
}
