/**
 * @file    synchronization.h
 * @brief   Mini RTOS – Mutex and Semaphore Public API
 *
 * Provides:
 *   • Binary / counting semaphores
 *   • Mutexes (binary semaphore with ownership tracking and priority
 *     inheritance to prevent priority inversion)
 *
 * Both primitives block the calling task (TASK_BLOCKED) rather than
 * spinning, so they do not waste CPU cycles.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "rtos_kernel.h"
#include "task_manager.h"

/* =========================================================================
 * Semaphore
 * ========================================================================= */

/**
 * @brief  Semaphore control block.
 *
 * Do not access fields directly — use the API functions.
 */
typedef struct {
    volatile int32_t       count;                          /**< Current count (>=0). */
    int32_t                max_count;                      /**< Maximum count. */
    rtos_tcb_t            *wait_list[RTOS_MAX_WAITERS];    /**< Tasks blocked on this semaphore. */
    uint32_t               wait_count;                     /**< Number of waiters. */
    uint8_t                in_use;                         /**< 1 if this slot is allocated. */
} rtos_semaphore_t;

/**
 * @brief  Initialise a semaphore.
 *
 * @param  sem        Pointer to the semaphore structure to initialise.
 * @param  initial    Initial count value.
 * @param  max_count  Maximum count (use 1 for a binary semaphore).
 *
 * @return RTOS_OK on success, RTOS_ERR_PARAM if arguments are invalid.
 */
int rtos_sem_init(rtos_semaphore_t *sem, int32_t initial, int32_t max_count);

/**
 * @brief  Wait (P / take) on a semaphore.
 *
 * Decrements the semaphore count.  If the count would go below zero the
 * calling task blocks until another task calls rtos_sem_post().
 *
 * @param  sem      Pointer to an initialised semaphore.
 * @param  timeout  Maximum ticks to wait (0 = wait forever).
 *
 * @return RTOS_OK on success, RTOS_ERR_STATE if timeout expired.
 */
int rtos_sem_wait(rtos_semaphore_t *sem, uint32_t timeout);

/**
 * @brief  Signal (V / give) a semaphore.
 *
 * Increments the count.  If a task is blocked on the semaphore it is moved
 * to the READY state and added to the scheduler queue.
 *
 * @param  sem  Pointer to an initialised semaphore.
 *
 * @return RTOS_OK on success, RTOS_ERR_PARAM if sem is NULL.
 */
int rtos_sem_post(rtos_semaphore_t *sem);

/**
 * @brief  Return the current semaphore count (non-blocking query).
 * @param  sem  Pointer to an initialised semaphore.
 * @return Current count value.
 */
int32_t rtos_sem_get_count(const rtos_semaphore_t *sem);

/* =========================================================================
 * Mutex
 * ========================================================================= */

/**
 * @brief  Mutex control block.
 *
 * Mutexes add ownership and priority-inheritance on top of a binary
 * semaphore.  Only the task that locked the mutex may unlock it.
 */
typedef struct {
    volatile uint8_t       locked;                         /**< 1 = locked. */
    rtos_tcb_t            *owner;                          /**< TCB of the task holding the lock. */
    uint8_t                owner_original_priority;        /**< Owner's priority before any inheritance. */
    rtos_tcb_t            *wait_list[RTOS_MAX_WAITERS];    /**< Tasks blocked on this mutex. */
    uint32_t               wait_count;                     /**< Number of waiters. */
    uint8_t                in_use;                         /**< 1 if this slot is allocated. */
} rtos_mutex_t;

/**
 * @brief  Initialise a mutex (initially unlocked).
 * @param  mtx  Pointer to the mutex structure to initialise.
 * @return RTOS_OK on success, RTOS_ERR_PARAM if @p mtx is NULL.
 */
int rtos_mutex_init(rtos_mutex_t *mtx);

/**
 * @brief  Lock a mutex.
 *
 * If the mutex is already locked, the calling task blocks.  Basic priority
 * inheritance is applied: if the calling task has higher priority than the
 * current owner, the owner's priority is temporarily elevated to prevent
 * priority inversion.
 *
 * @param  mtx      Pointer to an initialised mutex.
 * @param  timeout  Maximum ticks to wait (0 = wait forever).
 *
 * @return RTOS_OK on success, RTOS_ERR_STATE if timeout expired.
 */
int rtos_mutex_lock(rtos_mutex_t *mtx, uint32_t timeout);

/**
 * @brief  Unlock a mutex.
 *
 * Must be called by the same task that called rtos_mutex_lock().  Restores
 * the owner's original priority and wakes the highest-priority waiter.
 *
 * @param  mtx  Pointer to an initialised mutex.
 *
 * @return RTOS_OK on success, RTOS_ERR_STATE if caller is not the owner.
 */
int rtos_mutex_unlock(rtos_mutex_t *mtx);

#endif /* SYNCHRONIZATION_H */
