/**
 * @file    rtos_kernel.c
 * @brief   Mini RTOS – Core Kernel Implementation
 *
 * Implements kernel initialisation, the idle task, SysTick tick handler
 * integration, delay/yield, and critical-section helpers.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 *
 * HOW IT WORKS (summary)
 * ─────────────────────────────────────────────────────────────────────────
 * 1. rtos_kernel_init() clears the TCB table, sets PendSV to lowest
 *    priority, and creates the idle task.
 * 2. rtos_start() loads the first task's stack pointer into PSP, switches
 *    the CPU to use PSP, then enables SysTick and triggers the first switch.
 * 3. Every SysTick ISR calls rtos_tick() to decrement sleep counters,
 *    advance the time-slice counter, then calls rtos_schedule() which
 *    sets g_next_task and pends PendSV if a switch is needed.
 * 4. PendSV_Handler (in context_switch.c) saves R4-R11 on the current PSP,
 *    stores the new PSP back into g_current_task->sp, loads g_next_task->sp,
 *    restores R4-R11, and returns to Thread mode on the new stack.
 */

#include "rtos_kernel.h"
#include "task_manager.h"
#include "scheduler.h"
#include "config.h"
#include <string.h>

/* =========================================================================
 * Cortex-M System Control Register addresses
 * ========================================================================= */

/* SysTick registers */
#define SYSTICK_CTRL   (*((volatile uint32_t *)0xE000E010))
#define SYSTICK_LOAD   (*((volatile uint32_t *)0xE000E014))
#define SYSTICK_VAL    (*((volatile uint32_t *)0xE000E018))

/* NVIC System Handler Priority Register 3 (contains PendSV & SysTick prio) */
#define SHPR3          (*((volatile uint32_t *)0xE000ED20))

/* Interrupt Control and State Register (to pend PendSV) */
#define ICSR           (*((volatile uint32_t *)0xE000ED04))
#define ICSR_PENDSVSET (1UL << 28)

/* SysTick CTRL bits */
#define SYSTICK_CLKSOURCE (1UL << 2)   /* Use processor clock */
#define SYSTICK_TICKINT   (1UL << 1)   /* Enable SysTick interrupt */
#define SYSTICK_ENABLE    (1UL << 0)   /* Enable counter */

/* =========================================================================
 * Global Kernel Variables (declared extern in rtos_kernel.h)
 * ========================================================================= */

volatile uint8_t   g_rtos_running    = 0;
volatile uint32_t  g_rtos_tick_count = 0;
rtos_tcb_t        *g_current_task    = NULL;
rtos_tcb_t        *g_next_task       = NULL;

/* =========================================================================
 * Private: Idle Task
 * ========================================================================= */

/** Stack for the idle task (statically allocated). */
static uint32_t s_idle_stack[RTOS_MIN_STACK_SIZE / sizeof(uint32_t)];

/**
 * @brief  Idle task body.
 *
 * Runs when no other task is READY.  Uses WFI to sleep the CPU until the
 * next interrupt, which saves power on real hardware.
 */
static void idle_task(void *arg)
{
    (void)arg;
    for (;;) {
        /* Wait For Interrupt — halts the CPU clock until an IRQ arrives.
         * Remove this if your debugger has trouble stopping at WFI. */
        __asm volatile ("wfi");
    }
}

/* =========================================================================
 * Kernel Init
 * ========================================================================= */

int rtos_kernel_init(void)
{
    /* 1. Initialise the task manager (clears the TCB table). */
    task_manager_init();

    /* 2. Initialise the scheduler's ready queues. */
    scheduler_init();

    /* 3. Set PendSV to the lowest interrupt priority so it only fires after
     *    all real ISRs have completed.  SysTick gets one step higher. */
    SHPR3 = (SHPR3 & 0x00FFFFFFUL)
          | ((uint32_t)RTOS_PENDSV_PRIORITY  << 24)   /* PendSV — bits [31:24] */
          | ((uint32_t)RTOS_SYSTICK_PRIORITY << 16);   /* SysTick — bits [23:16] */

    /* 4. Create the idle task at the lowest priority.
     *    It will only run when nothing else is READY. */
    rtos_task_handle_t idle_handle;
    int rc = rtos_task_create(idle_task,
                              NULL,
                              RTOS_IDLE_PRIORITY,
                              s_idle_stack,
                              sizeof(s_idle_stack),
                              "idle",
                              &idle_handle);
    RTOS_ASSERT(rc == 0);

    return rc;
}

/* =========================================================================
 * Kernel Start
 * ========================================================================= */

void rtos_start(void)
{
    RTOS_ASSERT(!g_rtos_running);

    /* Pick the first task to run. */
    g_current_task = scheduler_get_next();
    RTOS_ASSERT(g_current_task != NULL);
    g_current_task->state = TASK_RUNNING;

    /* Configure SysTick for RTOS_TICK_MS period. */
    uint32_t reload = (RTOS_SYSCLK_HZ / 1000UL) * RTOS_TICK_MS - 1UL;
    SYSTICK_VAL  = 0;
    SYSTICK_LOAD = reload;
    SYSTICK_CTRL = SYSTICK_CLKSOURCE | SYSTICK_TICKINT | SYSTICK_ENABLE;

    g_rtos_running = 1;

    /* --- Launch the first task ---
     *
     * We need to:
     *   a) Load the first task's initial SP into PSP.
     *   b) Switch the CPU to use PSP in Thread mode (CONTROL.SPSEL = 1).
     *   c) Pop the fake exception frame we placed on the stack during
     *      task creation so the CPU starts executing the task function.
     *
     * The simplest portable approach is to trigger PendSV here, but that
     * requires g_current_task to already point at the new task.  Instead
     * we inline a minimal restore sequence:
     */
    __asm volatile (
        /* Load current task's SP into PSP */
        "LDR     R0, =g_current_task    \n"
        "LDR     R1, [R0]               \n"   /* R1 = g_current_task (TCB ptr) */
        "LDR     R2, [R1]               \n"   /* R2 = TCB->sp */
        "MSR     PSP, R2                \n"   /* Set PSP = saved SP */

        /* Switch Thread mode to use PSP (CONTROL[1] = 1) */
        "MOV     R0, #0x02              \n"
        "MSR     CONTROL, R0            \n"
        "ISB                            \n"   /* Instruction Synchronisation Barrier */

        /* Restore callee-saved registers from the fake frame we created.
         * In task_manager.c we placed R4-R11 on the stack below the
         * hardware-saved frame so we pop them here. */
        "POP     {R4-R11}               \n"

        /* Pop hardware-saved registers (R0-R3, R12, LR, PC, xPSR).
         * When the CPU pops PC it jumps to the task function. */
        "POP     {R0-R3, R12, LR}       \n"
        "POP     {PC}                   \n"   /* Branch to task entry */
        /* xPSR is already correct (Thumb bit set) from the fake frame. */
        ::: "memory"
    );

    /* Should never reach here */
    for (;;);
}

/* =========================================================================
 * Tick Handler (called from SysTick_Handler in systick_handler.c)
 * ========================================================================= */

/**
 * @brief  Advance the RTOS tick counter and handle sleeping tasks.
 *
 * Called from SysTick_Handler.  This function is NOT an ISR itself — the
 * ISR wrapper is in systick_handler.c so the naming matches CMSIS conventions.
 */
void rtos_tick(void)
{
    g_rtos_tick_count++;

    /* Decrement sleep counters; wake tasks whose time has expired. */
    for (uint32_t i = 0; i < RTOS_MAX_TASKS; i++) {
        rtos_tcb_t *tcb = task_manager_get_tcb(i);
        if (tcb == NULL) continue;
        if (tcb->state == TASK_BLOCKED && tcb->sleep_ticks > 0) {
            tcb->sleep_ticks--;
            if (tcb->sleep_ticks == 0) {
                /* Wake the task — move it back to the ready queue. */
                tcb->state = TASK_READY;
                scheduler_add_task(tcb);
            }
        }
    }

    /* Ask the scheduler whether it wants to preempt the current task. */
    scheduler_tick();
}

/* =========================================================================
 * Yield
 * ========================================================================= */

void rtos_yield(void)
{
    uint32_t primask = rtos_critical_enter();

    if (g_current_task != NULL) {
        /* Reset this task's time slice and put it back at the end of its
         * priority queue so the next task at the same level gets to run. */
        g_current_task->slice_ticks = 0;
        g_current_task->state       = TASK_READY;
        scheduler_add_task(g_current_task);
    }

    /* Select the next task. */
    g_next_task = scheduler_get_next();
    RTOS_ASSERT(g_next_task != NULL);

    /* Pend a context switch via PendSV. */
    ICSR = ICSR_PENDSVSET;

    rtos_critical_exit(primask);
}

/* =========================================================================
 * Delay
 * ========================================================================= */

void rtos_delay(uint32_t ticks)
{
    if (ticks == 0) {
        rtos_yield();
        return;
    }

    uint32_t primask = rtos_critical_enter();

    /* Block the current task for `ticks` scheduler ticks. */
    g_current_task->state       = TASK_BLOCKED;
    g_current_task->sleep_ticks = ticks;

    /* Remove from the ready queue (it's currently marked RUNNING, so the
     * scheduler won't find it there, but be explicit about the removal). */
    scheduler_remove_task(g_current_task);

    /* Pick the next task and trigger PendSV. */
    g_next_task = scheduler_get_next();
    RTOS_ASSERT(g_next_task != NULL);
    ICSR = ICSR_PENDSVSET;

    rtos_critical_exit(primask);
}

/* =========================================================================
 * Tick Count Accessor
 * ========================================================================= */

uint32_t rtos_get_tick_count(void)
{
    return g_rtos_tick_count;
}

/* =========================================================================
 * Critical Section Helpers
 * ========================================================================= */

uint32_t rtos_critical_enter(void)
{
    uint32_t primask;
    __asm volatile ("MRS %0, PRIMASK" : "=r"(primask));
    __asm volatile ("CPSID I" ::: "memory");
    return primask;
}

void rtos_critical_exit(uint32_t saved_primask)
{
    __asm volatile ("MSR PRIMASK, %0" :: "r"(saved_primask) : "memory");
}
