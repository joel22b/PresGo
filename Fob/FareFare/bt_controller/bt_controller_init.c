/*
 * bt_controller_init.c
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#include "bt_controller.h"

#include "app_log.h"
#include "app_assert.h"
#include "gatt_db.h"

#include "state_machine/state_machine.h"
#include "fare_fob/config.h"

void
btc_init_stack (sl_bt_evt_system_boot_t *evt_system_boot)
{
  // Print boot message.
  app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d\n",
               evt_system_boot->major, evt_system_boot->minor,
               evt_system_boot->patch, evt_system_boot->build);

  // Extract unique ID from BT Address.
  bd_addr address;
  uint8_t address_type;
  sl_status_t ret_val = sl_bt_system_get_identity_address (&address,
                                                           &address_type);
  app_assert_status (ret_val);

  // Pad and reverse unique ID to get System ID.
  uint8_t system_id[8];
  system_id[0] = address.addr[5];
  system_id[1] = address.addr[4];
  system_id[2] = address.addr[3];
  system_id[3] = 0xFF;
  system_id[4] = 0xFE;
  system_id[5] = address.addr[2];
  system_id[6] = address.addr[1];
  system_id[7] = address.addr[0];

  ret_val = sl_bt_gatt_server_write_attribute_value (gattdb_system_id, 0,
                                                     sizeof(system_id),
                                                     system_id);
  app_assert_status (ret_val);

  app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X\n",
               address_type ? "static random" : "public device",
               address.addr[5], address.addr[4], address.addr[3],
               address.addr[2], address.addr[1], address.addr[0]);

  btc_adv_init();

  // TODO: Edit SDK code to not have CTE advertisements start on boot
  //       Then we won't need to stop them immediately
  btc_adv_stop(btc_adv_services_cte);

  btc_connect_init();

#ifdef DEBUG_JUST_ADV
  app_log_info("DEBUG_JUST_ADV flag is declared, starting advertising and not stopping");
  sm_update_state (SM_STATE_RAPID_ADV, true);
#else
  sm_update_state (SM_STATE_CHECK_FOR_BUS, true);
#endif
}
