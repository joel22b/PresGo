/*
 * recent_bus.h
 *
 *  Created on: Feb. 14, 2024
 *      Author: Joel
 */

#ifndef RECENT_BUS_H_
#define RECENT_BUS_H_

#include "stdint.h"

#include "sl_sleeptimer.h"

void rb_init();

void rb_add_bus_to_list(uint8_t* address);
uint8_t rb_in_list(uint8_t* address);

void rb_timer_start();
void rb_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data);

#endif /* RECENT_BUS_H_ */
