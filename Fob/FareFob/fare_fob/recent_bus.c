/*
 * recent_bus.c
 *
 *  Created on: Feb. 14, 2024
 *      Author: Joel
 */

#include "recent_bus.h"

#include "app_log.h"

#include "config.h"

struct rb_list_entry {
  uint32_t duration;
  uint8_t address[ADDRESS_LEN];
};
typedef struct rb_list_entry rb_list_entry_t;

sl_sleeptimer_timer_handle_t rb_timer_period;

static rb_list_entry_t rb_list[RB_LIST_LEN];

void rb_init() {
  for (uint8_t i = 0; i < RB_LIST_LEN; i++) {
      rb_list[i].duration = 0;
  }

  // The timer will keep running forever
  rb_timer_start();
}

void rb_add_bus_to_list(uint8_t* address) {
  for (uint8_t i = 0; i < RB_LIST_LEN; i++) {
      if (rb_list[i].duration != 0) {
          rb_list[i].duration = RB_LIST_DURATION_PERIODS;
          for (uint8_t j = 0; j < ADDRESS_LEN; j++) {
              rb_list[i].address[j] = address[j];
          }
          return;
      }
  }

  // If here, no open spot was found in
  // Recent Bus list, replace the one
  // with the lowest duration
  uint8_t lowest_index = 0;
  uint32_t lowest_duration = RB_LIST_DURATION_PERIODS;
  for (uint8_t i = 0; i < RB_LIST_LEN; i++) {
      if (rb_list[i].duration < lowest_duration) {
          lowest_index = i;
          lowest_duration = rb_list[i].duration;
      }
  }

  // Add bus to lowest_index
  rb_list[lowest_index].duration = RB_LIST_DURATION_PERIODS;
  for (uint8_t j = 0; j < ADDRESS_LEN; j++) {
      rb_list[lowest_index].address[j] = address[j];
  }
}

uint8_t rb_in_list(uint8_t* address) {
  for (uint8_t i = 0; i < RB_LIST_LEN; i++) {
      if (rb_list[i].duration != 0) {
          uint8_t match = 1;
          for (uint8_t j = 0; j < ADDRESS_LEN; j++) {
              if (rb_list[i].address[j] != address[j]) {
                  match = 0;
                  break;
              }
          }
          if (match) {
              return 1;
          }
      }
  }
  // If here, no match found
  return 0;
}

void rb_timer_start() {
  uint32_t timer_timeout = sl_sleeptimer_ms_to_tick(RB_PERIOD_MS);
  sl_status_t ret_val = sl_sleeptimer_start_timer(&rb_timer_period,
                                                  timer_timeout,
                                                  rb_timer_callback,
                                                  NULL,
                                                  20,
                                                  0);
  if (ret_val != SL_STATUS_OK) {
      app_log_info("Failed to start Recent Bus list timer\n\r");
  }
}

void rb_timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data) {
  // Restart the timer
  rb_timer_start();

  // Decrement the durations
  for (uint8_t i = 0; i < RB_LIST_LEN; i++) {
      if (rb_list[i].duration != 0) {
          rb_list[i].duration--;
      }
  }

  (void)handle;
  (void)data;
}
