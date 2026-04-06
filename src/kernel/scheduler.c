/**
 * @file    scheduler.c
 * @brief   Mini RTOS – Scheduler Implementation
 *
 * Priority-based preemptive scheduler with optional round-robin within
 * the same priority level.  See scheduler.h for the design overview.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "scheduler.h"
#include "rtos_kernel.h"
#include "config.h"
#include <stddef.h>

/* =========================================================================
 * Ready Queues
 * ========================================================================= */

/**
 * @brief  Per-priority singly-linked FIFO queue.
 *
 * Maintained as head/tail pair for O(1) enqueue and O(1) dequeue.
 */
typedef struct {
    rtos_tcb_t *head;   /**< Front of queue (next to dequeue). */
    rtos_tcb_t *tail;   /**< Back  of queue (last enqueued).   */
} priority_queue_t;

static priority_queue_t s_queues[RTOS_PRIORITY_LEVELS];

/* =========================================================================
 * scheduler_init
 * ========================================================================= */

void scheduler_init(void)
{
    for (uint32_t i = 0; i < RTOS_PRIORITY_LEVELS; i++) {
        s_queues[i].head = NULL;
        s_queues[i].tail = NULL;
    }
}

/* =========================================================================
 * scheduler_add_task  (enqueue at tail)
 * ========================================================================= */

void scheduler_add_task(rtos_tcb_t *tcb)
{
    if (tcb == NULL) return;

    uint8_t prio = tcb->priority;
    if (prio >= RTOS_PRIORITY_LEVELS) return;

    tcb->next = NULL;  /* Ensure the node doesn't link to old list entries */

    if (s_queues[prio].tail == NULL) {
        /* Queue was empty */
        s_queues[prio].head = tcb;
        s_queues[prio].tail = tcb;
    } else {
        /* Append to tail */
        s_queues[prio].tail->next = tcb;
        s_queues[prio].tail       = tcb;
    }
}

/* =========================================================================
 * scheduler_remove_task  (remove from anywhere in queue)
 * ========================================================================= */

void scheduler_remove_task(rtos_tcb_t *tcb)
{
    if (tcb == NULL) return;

    uint8_t prio = tcb->priority;
    if (prio >= RTOS_PRIORITY_LEVELS) return;

    rtos_tcb_t *prev = NULL;
    rtos_tcb_t *curr = s_queues[prio].head;

    while (curr != NULL) {
        if (curr == tcb) {
            /* Unlink */
            if (prev == NULL) {
                s_queues[prio].head = curr->next;
            } else {
                prev->next = curr->next;
            }
            if (curr == s_queues[prio].tail) {
                s_queues[prio].tail = prev;
            }
            curr->next = NULL;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
    /* TCB was not in the queue — that is fine. */
}

/* =========================================================================
 * scheduler_get_next  (dequeue from head of highest-priority queue)
 * ========================================================================= */

rtos_tcb_t *scheduler_get_next(void)
{
    for (uint32_t prio = 0; prio < RTOS_PRIORITY_LEVELS; prio++) {
        if (s_queues[prio].head != NULL) {
            rtos_tcb_t *tcb = s_queues[prio].head;

            /* Dequeue from head */
            s_queues[prio].head = tcb->next;
            if (s_queues[prio].head == NULL) {
                s_queues[prio].tail = NULL;
            }
            tcb->next = NULL;

            return tcb;
        }
    }

    /* Should never reach here because the idle task is always ready. */
    RTOS_ASSERT(0);
    return NULL;
}

/* =========================================================================
 * scheduler_tick  (called every SysTick from rtos_tick())
 * ========================================================================= */

void scheduler_tick(void)
{
    if (!g_rtos_running || g_current_task == NULL) return;

    /* ── Check for higher-priority task becoming ready ────────────────── */
    /* Look for the highest-priority non-empty queue. */
    uint8_t best_prio = RTOS_PRIORITY_LEVELS; /* sentinel: no candidate */
    for (uint32_t prio = 0; prio < RTOS_PRIORITY_LEVELS; prio++) {
        if (s_queues[prio].head != NULL) {
            best_prio = (uint8_t)prio;
            break;
        }
    }

    int need_switch = 0;

    if (best_prio < g_current_task->priority) {
        /* A higher-priority task is waiting — preempt immediately. */
        need_switch = 1;
    }
#if RTOS_ROUND_ROBIN_ENABLE
    else if (best_prio == g_current_task->priority) {
        /* Same priority: decrement time slice. */
        if (g_current_task->slice_ticks > 0) {
            g_current_task->slice_ticks--;
        }
        if (g_current_task->slice_ticks == 0) {
            /* Time slice expired — rotate to next task at the same priority. */
            g_current_task->slice_ticks = RTOS_TIME_SLICE_TICKS;
            need_switch = 1;
        }
    }
#endif /* RTOS_ROUND_ROBIN_ENABLE */

    if (need_switch) {
        /* Put the current task back at the tail of its queue (if still RUNNING). */
        if (g_current_task->state == TASK_RUNNING) {
            g_current_task->state = TASK_READY;
            scheduler_add_task(g_current_task);
        }

        g_next_task = scheduler_get_next();
        g_next_task->state = TASK_RUNNING;

        /* Pend a PendSV context switch */
        *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
    }
}
