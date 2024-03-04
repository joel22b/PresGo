#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "stdint.h"
#include <stdbool.h>

#include "sl_sleeptimer.h"
#include "app_log.h"

#include "state_machine_types.h"

/* ===========================================
 * State Machine Generic Functions
 *
 * Defined in state_machine.c
=========================================== */
void sm_init();
void sm_process();
void sm_update_state(sm_state_t newState, bool override);
sl_status_t sm_timer_start(uint16_t duration_ms,
                           sl_sleeptimer_timer_callback_t callback_func,
                           void* callback_data);
sl_status_t sm_timer_stop();

/* ===========================================
 * Boot State Functions
 *
 * Defined in state_machine_boot.c
=========================================== */
void sm_state_boot_exit();

/* ===========================================
 * Check For Bus State Functions
 *
 * Defined in state_machine_check_for_bus.c
=========================================== */
void sm_state_check_for_bus_enter();
void sm_state_check_for_bus_exit();

/* ===========================================
 * Sleep State Functions
 *
 * Defined in state_machine_sleep.c
=========================================== */
void sm_state_sleep_enter();
void sm_state_sleep_exit();

/* ===========================================
 * Rapid Adv State Functions
 *
 * Defined in state_machine_rapid_adv.c
=========================================== */
void sm_state_rapid_adv_enter();
void sm_state_rapid_adv_exit();

#endif // STATE_MACHINE_H
