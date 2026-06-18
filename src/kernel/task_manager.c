/**
 * @file    task_manager.c
 * @brief   Mini RTOS – Task Manager Implementation
 *
 * Manages a statically-allocated pool of RTOS_MAX_TASKS TCB slots.
 * The most important job here is building the initial fake stack frame
 * so the context-switch code can start a brand-new task as if it just
 * returned from an exception handler.
 *
 * Initial Stack Frame Layout (ARM Cortex-M, grows downward)
 * ──────────────────────────────────────────────────────────
 *
 *  High address (top of stack buffer)
 *  ┌────────────────┐  <- stack_base + stack_size
 *  │   xPSR         │  = 0x01000000  (Thumb bit set, no exceptions)
 *  │   PC           │  = task entry function address
 *  │   LR           │  = task_exit_stub (called if task returns)
 *  │   R12          │  = 0
 *  │   R3           │  = 0
 *  │   R2           │  = 0
 *  │   R1           │  = 0
 *  │   R0           │  = arg  (first function argument)
 *  ├────────────────┤  <- hardware-saved frame (8 words = 32 bytes)
 *  │   R11          │  = 0
 *  │   R10          │  = 0
 *  │   R9           │  = 0
 *  │   R8           │  = 0
 *  │   R7           │  = 0
 *  │   R6           │  = 0
 *  │   R5           │  = 0
 *  │   R4           │  = 0
 *  ├────────────────┤  <- software-saved frame (8 words = 32 bytes)
 *  │   (free stack) │
 *  Low address (bottom of stack buffer)
 *
 *  TCB->sp points to the bottom of the software-saved frame (R4 slot).
 *  PendSV_Handler pops R4-R11 first, then the CPU's exception return
 *  mechanism pops R0-R3, R12, LR, PC, xPSR automatically.
 */

#include "task_manager.h"
#include "scheduler.h"
#include "rtos_kernel.h"
#include "config.h"
#include <string.h>

/* =========================================================================
 * TCB Pool
 * ========================================================================= */

/** Statically-allocated pool of TCBs. */
static rtos_tcb_t s_tcb_pool[RTOS_MAX_TASKS];

/* =========================================================================
 * Private: Task Exit Stub
 * ========================================================================= */

/**
 * @brief  Called if a task function ever returns (it shouldn't).
 *
 * Deletes the calling task and triggers a context switch.
 */
static void task_exit_stub(void)
{
    rtos_task_delete(RTOS_INVALID_HANDLE);
    /* Should not reach here; loop just in case. */
    for (;;);
}

/* =========================================================================
 * task_manager_init
 * ========================================================================= */

void task_manager_init(void)
{
    memset(s_tcb_pool, 0, sizeof(s_tcb_pool));
    for (uint32_t i = 0; i < RTOS_MAX_TASKS; i++) {
        s_tcb_pool[i].state = TASK_DELETED;
    }
}

/* =========================================================================
 * rtos_task_create
 * ========================================================================= */

int rtos_task_create(rtos_task_func_t    func,
                     void               *arg,
                     uint8_t             priority,
                     uint32_t           *stack,
                     uint32_t            stack_size,
                     const char         *name,
                     rtos_task_handle_t *handle)
{
    /* Validate parameters */
    if (func == NULL || stack == NULL || handle == NULL)       return RTOS_ERR_PARAM;
    if (stack_size < RTOS_MIN_STACK_SIZE)                      return RTOS_ERR_PARAM;
    if (priority >= RTOS_PRIORITY_LEVELS)                      return RTOS_ERR_PARAM;
    if (((uintptr_t)stack & 0x3U) != 0)                        return RTOS_ERR_PARAM;

    /* Find a free TCB slot */
    uint32_t primask = rtos_critical_enter();
    rtos_tcb_t *tcb = NULL;
    uint32_t    idx = 0;
    for (uint32_t i = 0; i < RTOS_MAX_TASKS; i++) {
        if (s_tcb_pool[i].state == TASK_DELETED) {
            tcb = &s_tcb_pool[i];
            idx = i;
            break;
        }
    }
    rtos_critical_exit(primask);

    if (tcb == NULL) return RTOS_ERR_NO_SLOTS;

    /* ── Fill the fake initial stack frame ────────────────────────────── */

    /* Fill entire stack with debug pattern to help detect overflow */
#if RTOS_STACK_CHECK_ENABLE
    uint8_t *p = (uint8_t *)stack;
    for (uint32_t i = 0; i < stack_size; i++) {
        p[i] = RTOS_STACK_FILL_BYTE;
    }
#endif

    /* Pointer arithmetic: top of stack (stack grows downward). */
    uint32_t *stack_top = stack + (stack_size / sizeof(uint32_t));

    /* ── Hardware-saved exception frame (Cortex-M auto-pushes on exception entry) */
    *(--stack_top) = 0x01000000UL;           /* xPSR: Thumb bit set */
    *(--stack_top) = (uint32_t)func;         /* PC   — task entry point */
    *(--stack_top) = (uint32_t)task_exit_stub; /* LR — return address */
    *(--stack_top) = 0x00000000UL;           /* R12 */
    *(--stack_top) = 0x00000000UL;           /* R3  */
    *(--stack_top) = 0x00000000UL;           /* R2  */
    *(--stack_top) = 0x00000000UL;           /* R1  */
    *(--stack_top) = (uint32_t)arg;          /* R0  — argument to task func */

    /* ── Software-saved callee registers (PendSV_Handler pops these) */
    *(--stack_top) = 0x00000000UL;           /* R11 */
    *(--stack_top) = 0x00000000UL;           /* R10 */
    *(--stack_top) = 0x00000000UL;           /* R9  */
    *(--stack_top) = 0x00000000UL;           /* R8  */
    *(--stack_top) = 0x00000000UL;           /* R7  */
    *(--stack_top) = 0x00000000UL;           /* R6  */
    *(--stack_top) = 0x00000000UL;           /* R5  */
    *(--stack_top) = 0x00000000UL;           /* R4  */

    /* ── Populate the TCB ──────────────────────────────────────────────── */
    tcb->sp          = stack_top;
    tcb->stack_base  = stack;
    tcb->stack_size  = stack_size;
    tcb->func        = func;
    tcb->arg         = arg;
    tcb->priority    = priority;
    tcb->state       = TASK_READY;
    tcb->sleep_ticks = 0;
    tcb->slice_ticks = RTOS_TIME_SLICE_TICKS;
    tcb->next        = NULL;

    /* Copy name (ensure null-terminated) */
    if (name != NULL) {
        strncpy(tcb->name, name, sizeof(tcb->name) - 1);
        tcb->name[sizeof(tcb->name) - 1] = '\0';
    } else {
        tcb->name[0] = '\0';
    }

    /* Add to the scheduler's ready queue */
    primask = rtos_critical_enter();
    scheduler_add_task(tcb);
    rtos_critical_exit(primask);

    *handle = (rtos_task_handle_t)idx;
    return RTOS_OK;
}

/* =========================================================================
 * rtos_task_delete
 * ========================================================================= */

void rtos_task_delete(rtos_task_handle_t handle)
{
    uint32_t primask = rtos_critical_enter();

    rtos_tcb_t *tcb;

    if (handle == RTOS_INVALID_HANDLE) {
        tcb = g_current_task;
    } else if (handle < RTOS_MAX_TASKS) {
        tcb = &s_tcb_pool[handle];
    } else {
        rtos_critical_exit(primask);
        return;
    }

    if (tcb == NULL || tcb->state == TASK_DELETED) {
        rtos_critical_exit(primask);
        return;
    }

    /* Remove from the ready queue if it's there. */
    scheduler_remove_task(tcb);

    tcb->state = TASK_DELETED;

    if (tcb == g_current_task) {
        /* Self-delete: need an immediate context switch. */
        g_next_task = scheduler_get_next();
        RTOS_ASSERT(g_next_task != NULL);

        /* Trigger PendSV */
        *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
    }

    rtos_critical_exit(primask);
}

/* =========================================================================
 * rtos_task_suspend / rtos_task_resume
 * ========================================================================= */

void rtos_task_suspend(rtos_task_handle_t handle)
{
    uint32_t primask = rtos_critical_enter();

    rtos_tcb_t *tcb;
    if (handle == RTOS_INVALID_HANDLE) {
        tcb = g_current_task;
    } else if (handle < RTOS_MAX_TASKS) {
        tcb = &s_tcb_pool[handle];
    } else {
        rtos_critical_exit(primask);
        return;
    }

    if (tcb == NULL || tcb->state == TASK_DELETED) {
        rtos_critical_exit(primask);
        return;
    }

    scheduler_remove_task(tcb);
    tcb->state = TASK_SUSPENDED;

    if (tcb == g_current_task) {
        /* Suspending self — switch away immediately. */
        g_next_task = scheduler_get_next();
        *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
    }

    rtos_critical_exit(primask);
}

void rtos_task_resume(rtos_task_handle_t handle)
{
    if (handle >= RTOS_MAX_TASKS) return;

    uint32_t primask = rtos_critical_enter();
    rtos_tcb_t *tcb = &s_tcb_pool[handle];

    if (tcb->state == TASK_SUSPENDED) {
        tcb->state = TASK_READY;
        scheduler_add_task(tcb);

        /* If the resumed task has higher priority than current, preempt now. */
        if (g_rtos_running && tcb->priority < g_current_task->priority) {
            g_next_task = tcb;
            *((volatile uint32_t *)0xE000ED04) = (1UL << 28);
        }
    }

    rtos_critical_exit(primask);
}

/* =========================================================================
 * Accessors
 * ========================================================================= */

rtos_tcb_t *task_manager_get_tcb(uint32_t index)
{
    if (index >= RTOS_MAX_TASKS) return NULL;
    if (s_tcb_pool[index].state == TASK_DELETED) return NULL;
    return &s_tcb_pool[index];
}

rtos_task_handle_t rtos_task_get_current_handle(void)
{
    if (g_current_task == NULL) return RTOS_INVALID_HANDLE;
    /* Compute index from pointer arithmetic */
    return (rtos_task_handle_t)(g_current_task - s_tcb_pool);
}

rtos_task_state_t rtos_task_get_state(rtos_task_handle_t handle)
{
    if (handle >= RTOS_MAX_TASKS) return TASK_DELETED;
    return s_tcb_pool[handle].state;
}
