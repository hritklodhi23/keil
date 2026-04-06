/**
 * @file    scheduler.h
 * @brief   Mini RTOS – Scheduler Public API
 *
 * Implements a priority-based preemptive scheduler with optional round-robin
 * within the same priority level.
 *
 * Data structure: One singly-linked list (FIFO queue) per priority level.
 *
 *  Priority 0 (highest): [TCB_A] -> [TCB_B] -> NULL
 *  Priority 1:           [TCB_C] -> NULL
 *  ...
 *  Priority N-1 (lowest, idle): [idle_TCB] -> NULL
 *
 * The scheduler always picks the non-empty queue with the lowest index
 * (= highest priority) and dequeues its head.  When round-robin is enabled,
 * the running task is re-enqueued at the tail of its queue after each tick.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "rtos_kernel.h"

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * @brief  Initialise the scheduler.
 *
 * Clears all priority queues.  Called once by rtos_kernel_init().
 */
void scheduler_init(void);

/**
 * @brief  Add a task to the ready queue at its assigned priority.
 * @param  tcb  Pointer to the TCB to enqueue.
 */
void scheduler_add_task(rtos_tcb_t *tcb);

/**
 * @brief  Remove a task from whatever queue it is currently in.
 *
 * Searches all priority queues for @p tcb and removes it.
 * Safe to call if the task is not currently in any queue.
 *
 * @param  tcb  Pointer to the TCB to remove.
 */
void scheduler_remove_task(rtos_tcb_t *tcb);

/**
 * @brief  Select the next task to run.
 *
 * Dequeues and returns the head of the highest-priority non-empty ready
 * queue.  Does NOT change g_current_task or g_next_task — the caller
 * is responsible for that.
 *
 * @return Pointer to the TCB that should run next (never NULL; at minimum
 *         the idle task is always in the queue).
 */
rtos_tcb_t *scheduler_get_next(void);

/**
 * @brief  Per-tick scheduler hook.
 *
 * Called from rtos_tick() on every SysTick interrupt.  Handles time-slice
 * expiry for round-robin and determines whether a preemptive switch is
 * needed.  If so, sets g_next_task and pends PendSV.
 */
void scheduler_tick(void);

#endif /* SCHEDULER_H */
