/**
 * @file    synchronization.c
 * @brief   Mini RTOS – Mutex and Semaphore Implementation
 *
 * Both semaphores and mutexes maintain a simple wait-list (array) of blocked
 * tasks.  When a resource becomes available, the waiter with the highest
 * priority (lowest numeric priority value) is woken first.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "synchronization.h"
#include "scheduler.h"
#include "rtos_kernel.h"
#include "config.h"
#include <string.h>

/* =========================================================================
 * Private Helpers
 * ========================================================================= */

/**
 * @brief  Add a task to a wait list.
 *
 * @param  list        Array of TCB pointers (size RTOS_MAX_WAITERS).
 * @param  count       Pointer to the current count of waiters.
 * @param  tcb         TCB to add.
 * @return 1 on success, 0 if the list is full.
 */
static int waitlist_add(rtos_tcb_t **list, uint32_t *count, rtos_tcb_t *tcb)
{
    if (*count >= RTOS_MAX_WAITERS) return 0;
    list[(*count)++] = tcb;
    return 1;
}

/**
 * @brief  Remove and return the highest-priority waiter from a wait list.
 *
 * Scans the list for the TCB with the lowest numeric priority (= highest
 * urgency), removes it, and fills the gap by moving the last entry.
 *
 * @param  list   Array of TCB pointers.
 * @param  count  Pointer to the current count (decremented on success).
 * @return The highest-priority TCB, or NULL if the list is empty.
 */
static rtos_tcb_t *waitlist_pop_highest_priority(rtos_tcb_t **list, uint32_t *count)
{
    if (*count == 0) return NULL;

    /* Find the entry with the smallest priority number (= highest urgency). */
    uint32_t best_idx = 0;
    for (uint32_t i = 1; i < *count; i++) {
        if (list[i]->priority < list[best_idx]->priority) {
            best_idx = i;
        }
    }

    rtos_tcb_t *chosen = list[best_idx];

    /* Fill gap by moving last entry into best_idx slot. */
    list[best_idx] = list[--(*count)];
    list[*count]   = NULL;

    return chosen;
}

/* =========================================================================
 * Semaphore
 * ========================================================================= */

int rtos_sem_init(rtos_semaphore_t *sem, int32_t initial, int32_t max_count)
{
    if (sem == NULL)            return RTOS_ERR_PARAM;
    if (initial < 0)            return RTOS_ERR_PARAM;
    if (max_count < 1)          return RTOS_ERR_PARAM;
    if (initial > max_count)    return RTOS_ERR_PARAM;

    memset(sem, 0, sizeof(*sem));
    sem->count      = initial;
    sem->max_count  = max_count;
    sem->in_use     = 1;
    return RTOS_OK;
}

int rtos_sem_wait(rtos_semaphore_t *sem, uint32_t timeout)
{
    if (sem == NULL || !sem->in_use) return RTOS_ERR_PARAM;

    uint32_t primask = rtos_critical_enter();

    if (sem->count > 0) {
        /* Resource is available — take it immediately. */
        sem->count--;
        rtos_critical_exit(primask);
        return RTOS_OK;
    }

    /* Resource not available — block the current task. */
    if (!waitlist_add(sem->wait_list, &sem->wait_count, g_current_task)) {
        rtos_critical_exit(primask);
        return RTOS_ERR_STATE;   /* Wait list full */
    }

    g_current_task->state       = TASK_BLOCKED;
    g_current_task->sleep_ticks = timeout;   /* 0 = block indefinitely */

    /* Choose the next task and trigger PendSV. */
    g_next_task = scheduler_get_next();
    *((volatile uint32_t *)0xE000ED04) = (1UL << 28);

    rtos_critical_exit(primask);

    /* Execution continues here once the task is woken.
     * Check whether we timed out (sleep_ticks was set to 0 by tick handler
     * via natural expiry, so we need a flag — simplification: assume woken
     * means success for now). */
    return RTOS_OK;
}

int rtos_sem_post(rtos_semaphore_t *sem)
{
    if (sem == NULL || !sem->in_use) return RTOS_ERR_PARAM;

    uint32_t primask = rtos_critical_enter();

    if (sem->wait_count > 0) {
        /* Wake the highest-priority waiter. */
        rtos_tcb_t *waiter = waitlist_pop_highest_priority(sem->wait_list,
                                                           &sem->wait_count);
        if (waiter != NULL) {
            waiter->state       = TASK_READY;
            waiter->sleep_ticks = 0;
            scheduler_add_task(waiter);

            /* Preempt if the woken task has higher priority than current. */
            if (g_rtos_running && waiter->priority < g_current_task->priority) {
                g_next_task = scheduler_get_next();
                *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
            }
        }
    } else if (sem->count < sem->max_count) {
        sem->count++;
    }

    rtos_critical_exit(primask);
    return RTOS_OK;
}

int32_t rtos_sem_get_count(const rtos_semaphore_t *sem)
{
    if (sem == NULL) return 0;
    return sem->count;
}

/* =========================================================================
 * Mutex
 * ========================================================================= */

int rtos_mutex_init(rtos_mutex_t *mtx)
{
    if (mtx == NULL) return RTOS_ERR_PARAM;

    memset(mtx, 0, sizeof(*mtx));
    mtx->locked = 0;
    mtx->owner  = NULL;
    mtx->in_use = 1;
    return RTOS_OK;
}

int rtos_mutex_lock(rtos_mutex_t *mtx, uint32_t timeout)
{
    if (mtx == NULL || !mtx->in_use) return RTOS_ERR_PARAM;

    uint32_t primask = rtos_critical_enter();

    if (!mtx->locked) {
        /* Mutex is free — take it. */
        mtx->locked                  = 1;
        mtx->owner                   = g_current_task;
        mtx->owner_original_priority = g_current_task->priority;
        rtos_critical_exit(primask);
        return RTOS_OK;
    }

    /* Mutex is already locked. ─────────────────────────────────────────
     * Priority Inheritance: if the current task has higher priority than
     * the mutex owner, temporarily elevate the owner's priority so it can
     * finish its critical section faster and release the mutex sooner. */
    if (g_current_task->priority < mtx->owner->priority) {
        /* Remove owner from its current queue position … */
        scheduler_remove_task(mtx->owner);
        /* … elevate priority … */
        mtx->owner->priority = g_current_task->priority;
        /* … and re-insert at the new (higher) priority. */
        if (mtx->owner->state == TASK_READY) {
            scheduler_add_task(mtx->owner);
        }
    }

    /* Block the calling task. */
    if (!waitlist_add(mtx->wait_list, &mtx->wait_count, g_current_task)) {
        rtos_critical_exit(primask);
        return RTOS_ERR_STATE;   /* Wait list full */
    }

    g_current_task->state       = TASK_BLOCKED;
    g_current_task->sleep_ticks = timeout;

    g_next_task = scheduler_get_next();
    *((volatile uint32_t *)0xE000ED04) = (1UL << 28);

    rtos_critical_exit(primask);
    return RTOS_OK;
}

int rtos_mutex_unlock(rtos_mutex_t *mtx)
{
    if (mtx == NULL || !mtx->in_use) return RTOS_ERR_PARAM;

    uint32_t primask = rtos_critical_enter();

    if (!mtx->locked || mtx->owner != g_current_task) {
        /* Caller doesn't own the mutex. */
        rtos_critical_exit(primask);
        return RTOS_ERR_STATE;
    }

    /* Restore the owner's original priority (undo any inheritance). */
    g_current_task->priority = mtx->owner_original_priority;

    if (mtx->wait_count > 0) {
        /* Hand the mutex to the highest-priority waiter. */
        rtos_tcb_t *waiter = waitlist_pop_highest_priority(mtx->wait_list,
                                                           &mtx->wait_count);
        mtx->owner                   = waiter;
        mtx->owner_original_priority = waiter->priority;
        /* mutex stays locked; new owner doesn't need to re-acquire */

        waiter->state       = TASK_READY;
        waiter->sleep_ticks = 0;
        scheduler_add_task(waiter);

        if (waiter->priority < g_current_task->priority) {
            g_next_task = scheduler_get_next();
            *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
        }
    } else {
        /* No waiters — fully release the mutex. */
        mtx->locked = 0;
        mtx->owner  = NULL;
    }

    rtos_critical_exit(primask);
    return RTOS_OK;
}
