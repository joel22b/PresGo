/*
 * bt_controller_types.h
 *
 *  Created on: Feb. 1, 2024
 *      Author: Joel
 */

#ifndef BT_CONTROLLER_TYPES_H_
#define BT_CONTROLLER_TYPES_H_

#include "fare_fob/config.h"

struct btc_connect_handle {
  uint32_t service;   // Service handle
  uint16_t tx;        // TX Characteristic handle
  uint16_t rx;        // RX Characteristic handle
  uint8_t address[ADDRESS_LEN];
};
typedef struct btc_connect_handle btc_connect_handle_t;

typedef enum
{
  btc_adv_services_none     = 0x0, /**< (0x0) None */
  btc_adv_services_cte      = 0x1, /**< (0x1) CTE (Constant Tone Extension) */
  btc_adv_services_legacy   = 0x2, /**< (0x2) Legacy Advertisements */
  btc_adv_services_extended = 0x4, /**< (0x4) Extended Advertisements */

  // Combinations
  btc_adv_services_standard = 0x3, /**< (0x3) CTE and Legacy */
} btc_adv_services_t;

#endif /* BT_CONTROLLER_TYPES_H_ */
