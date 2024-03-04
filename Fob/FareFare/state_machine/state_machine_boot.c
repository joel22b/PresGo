/*
 * state_machine_boot.c
 *
 *  Created on: Feb. 2, 2024
 *      Author: Joel
 */

#include "state_machine.h"

//#include "bt_controller/bt_controller.h"

void sm_state_boot_exit() {
  app_log_info("SM: Boot exited");
  //btc_adv_stop(btc_adv_services_standard);
}
