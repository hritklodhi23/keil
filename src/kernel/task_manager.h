/**
 * @file    task_manager.h
 * @brief   Mini RTOS – Task Manager Public API
 *
 * Provides functions to create, delete, suspend, and resume tasks.
 * Each task gets a statically-allocated TCB slot from a fixed-size pool.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "rtos_kernel.h"

/* =========================================================================
 * Task Handle
 * ========================================================================= */

/**
 * @brief  Opaque handle returned by rtos_task_create().
 *
 * Internally this is the index of the TCB in the global TCB table.
 * User code should treat it as an opaque value.
 */
typedef uint32_t rtos_task_handle_t;

/** Sentinel value indicating an invalid / unassigned handle. */
#define RTOS_INVALID_HANDLE  ((rtos_task_handle_t)0xFFFFFFFFUL)

/* =========================================================================
 * Error Codes
 * ========================================================================= */

#define RTOS_OK              0    /**< Success. */
#define RTOS_ERR_NO_SLOTS   -1   /**< Task table is full. */
#define RTOS_ERR_PARAM      -2   /**< Invalid parameter. */
#define RTOS_ERR_STATE      -3   /**< Operation not valid in current state. */

/* =========================================================================
 * Public API
 * ========================================================================= */

/**
 * @brief  Initialise the task manager.
 *
 * Clears the TCB table.  Called once by rtos_kernel_init().
 */
void task_manager_init(void);

/**
 * @brief  Create a new task.
 *
 * Allocates a TCB slot, initialises the fake initial stack frame so the
 * context-switch code can start the task as if it just returned from an
 * exception, and adds the task to the scheduler's ready queue.
 *
 * @param  func        Task entry function (must not return; see rtos_task_func_t).
 * @param  arg         Argument forwarded to @p func as its first parameter.
 * @param  priority    Task priority (0 = highest, RTOS_PRIORITY_LEVELS-1 = lowest).
 * @param  stack       Pointer to the stack buffer (must be 4-byte aligned).
 * @param  stack_size  Size of @p stack in bytes (>= RTOS_MIN_STACK_SIZE).
 * @param  name        Human-readable name (max 15 characters).
 * @param  handle      [out] Receives the new task's handle.
 *
 * @return RTOS_OK on success, negative error code on failure.
 */
int rtos_task_create(rtos_task_func_t   func,
                     void              *arg,
                     uint8_t            priority,
                     uint32_t          *stack,
                     uint32_t           stack_size,
                     const char        *name,
                     rtos_task_handle_t *handle);

/**
 * @brief  Delete a task.
 *
 * Removes the task from the scheduler and marks its TCB as DELETED.
 * The task's stack memory (passed at creation) is NOT freed — the caller
 * is responsible for managing the buffer lifetime.
 *
 * Passing RTOS_INVALID_HANDLE or the handle of the currently-running task
 * causes the current task to self-delete and triggers an immediate context
 * switch.
 *
 * @param  handle  Handle returned by rtos_task_create(), or RTOS_INVALID_HANDLE
 *                 to delete the calling task.
 */
void rtos_task_delete(rtos_task_handle_t handle);

/**
 * @brief  Suspend a task.
 *
 * Moves the task to SUSPENDED state.  A suspended task is not scheduled
 * until explicitly resumed with rtos_task_resume().
 *
 * @param  handle  Task handle, or RTOS_INVALID_HANDLE for the calling task.
 */
void rtos_task_suspend(rtos_task_handle_t handle);

/**
 * @brief  Resume a suspended task.
 *
 * Moves the task from SUSPENDED → READY and adds it to the ready queue.
 *
 * @param  handle  Handle of the task to resume.
 */
void rtos_task_resume(rtos_task_handle_t handle);

/**
 * @brief  Get a pointer to a TCB by table index.
 *
 * Used internally by the kernel; user code should not normally call this.
 *
 * @param  index  Index in the TCB table [0, RTOS_MAX_TASKS).
 * @return Pointer to the TCB, or NULL if the slot is empty/deleted.
 */
rtos_tcb_t *task_manager_get_tcb(uint32_t index);

/**
 * @brief  Get the handle of the currently running task.
 * @return Handle of the calling task.
 */
rtos_task_handle_t rtos_task_get_current_handle(void);

/**
 * @brief  Get the state of a task.
 * @param  handle  Task handle.
 * @return Current rtos_task_state_t, or TASK_DELETED if handle is invalid.
 */
rtos_task_state_t rtos_task_get_state(rtos_task_handle_t handle);

#endif /* TASK_MANAGER_H */
