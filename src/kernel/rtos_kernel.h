/**
 * @file    rtos_kernel.h
 * @brief   Mini RTOS – Core Kernel Public API
 *
 * Declares the Task Control Block (TCB), task-state enumeration, and the
 * top-level kernel init/start functions.  All other RTOS modules include
 * this header to access shared kernel data structures.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef RTOS_KERNEL_H
#define RTOS_KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include "config.h"

/* =========================================================================
 * Task States
 * ========================================================================= */

/**
 * @brief All possible states of a Mini RTOS task.
 *
 * State diagram:
 *
 *   rtos_task_create()
 *         |
 *         v
 *      READY <------- BLOCKED (timeout expired / event occurred)
 *         |               ^
 *         v               |
 *      RUNNING ---------> BLOCKED  (waiting for mutex/semaphore/sleep)
 *         |
 *         v
 *     SUSPENDED  (explicitly suspended by another task or itself)
 *         |
 *         v
 *      DELETED
 */
typedef enum {
    TASK_READY      = 0,   /**< In the run queue; eligible to be scheduled. */
    TASK_RUNNING    = 1,   /**< Currently executing on the CPU. */
    TASK_BLOCKED    = 2,   /**< Waiting for an event (sleep, mutex, semaphore). */
    TASK_SUSPENDED  = 3,   /**< Explicitly suspended; not eligible to run. */
    TASK_DELETED    = 4    /**< Resources freed; TCB slot available for reuse. */
} rtos_task_state_t;

/* =========================================================================
 * Task Function Prototype
 * ========================================================================= */

/**
 * @brief Prototype for a task entry function.
 * @param arg  Optional argument passed at task-creation time.
 *
 * A task function must never return.  It should contain an infinite loop.
 * If it does return, the kernel will automatically delete the task.
 */
typedef void (*rtos_task_func_t)(void *arg);

/* =========================================================================
 * Task Control Block (TCB)
 * ========================================================================= */

/**
 * @brief Task Control Block — one instance per task.
 *
 * The *first* field MUST be `sp` (saved stack pointer) because the
 * context-switch assembly code loads/stores it at offset 0 from the TCB
 * pointer without any additional offset calculations.
 */
typedef struct rtos_tcb {
    uint32_t            *sp;            /**< Saved stack pointer (offset 0 — DO NOT MOVE). */
    uint32_t            *stack_base;    /**< Lowest valid stack address (for overflow check). */
    uint32_t             stack_size;    /**< Stack size in bytes. */
    rtos_task_func_t     func;          /**< Task entry function. */
    void                *arg;           /**< Argument passed to task entry function. */
    uint8_t              priority;      /**< Static priority (0 = highest). */
    rtos_task_state_t    state;         /**< Current task state. */
    uint32_t             sleep_ticks;   /**< Remaining ticks before waking (BLOCKED state). */
    uint32_t             slice_ticks;   /**< Time-slice counter (round-robin). */
    char                 name[16];      /**< Human-readable task name (for debugging). */
    struct rtos_tcb     *next;          /**< Next TCB in the scheduler's linked list. */
} rtos_tcb_t;

/* =========================================================================
 * Kernel State
 * ========================================================================= */

/** Opaque kernel state flags used internally; exposed so ISR code can read. */
extern volatile uint8_t  g_rtos_running;          /**< Non-zero after rtos_start(). */
extern volatile uint32_t g_rtos_tick_count;        /**< Total tick count since boot. */
extern          rtos_tcb_t *g_current_task;        /**< Pointer to the executing TCB. */
extern          rtos_tcb_t *g_next_task;           /**< Pointer to the TCB to switch to. */

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * @brief  Initialise the RTOS kernel.
 *
 * Must be called once before any other RTOS function.  Clears all TCB slots,
 * configures SysTick and PendSV priorities, and creates the idle task.
 *
 * @return 0 on success, negative error code on failure.
 */
int rtos_kernel_init(void);

/**
 * @brief  Start the RTOS scheduler.
 *
 * Enables SysTick, selects the highest-priority READY task, and triggers
 * the first context switch.  This function never returns to the caller.
 */
void rtos_start(void);

/**
 * @brief  Yield execution voluntarily.
 *
 * The calling task is moved to the back of its priority queue.  The
 * scheduler picks the next eligible task immediately.
 */
void rtos_yield(void);

/**
 * @brief  Enter a critical section (disable interrupts).
 * @return Saved interrupt mask to pass to rtos_critical_exit().
 */
uint32_t rtos_critical_enter(void);

/**
 * @brief  Exit a critical section (restore interrupts).
 * @param  saved_primask  Value returned by rtos_critical_enter().
 */
void rtos_critical_exit(uint32_t saved_primask);

/**
 * @brief  Advance the RTOS tick (called from SysTick_Handler).
 *
 * Increments the tick counter, wakes expired sleeping tasks, and calls
 * scheduler_tick() to handle preemption / time-slice rotation.
 * Must only be called from an interrupt context (SysTick ISR).
 */
void rtos_tick(void);

/**
 * @brief  Return the current system tick count.
 * @return Number of SysTick interrupts since rtos_start().
 */
uint32_t rtos_get_tick_count(void);

/**
 * @brief  Delay the calling task for at least `ticks` scheduler ticks.
 * @param  ticks  Number of ticks to sleep (0 = yield once).
 */
void rtos_delay(uint32_t ticks);

#endif /* RTOS_KERNEL_H */
