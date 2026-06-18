/**
 * @file    config.h
 * @brief   Mini RTOS Configuration Parameters
 *
 * Centralised compile-time knobs.  Change values here to tune the RTOS
 * for your target hardware without touching kernel source files.
 *
 * Target: ARM Cortex-M (STM32 or any Cortex-M3/M4/M33 derivative)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef CONFIG_H
#define CONFIG_H

/* =========================================================================
 * Clock / Timing
 * ========================================================================= */

/** System clock frequency in Hz (update to match your MCU configuration) */
#define RTOS_SYSCLK_HZ          72000000UL   /* 72 MHz (typical STM32F103) */

/** RTOS tick period in milliseconds.
 *  The SysTick interrupt fires every RTOS_TICK_MS milliseconds.
 *  Lower values give finer time resolution but increase overhead. */
#define RTOS_TICK_MS            1UL          /* 1 ms tick */

/* =========================================================================
 * Task Limits
 * ========================================================================= */

/** Maximum number of tasks that can exist simultaneously (including idle). */
#define RTOS_MAX_TASKS          16U

/** Default stack size (bytes) allocated for each task unless overridden. */
#define RTOS_DEFAULT_STACK_SIZE 512U

/** Minimum stack size (bytes) — must hold one full exception frame + locals. */
#define RTOS_MIN_STACK_SIZE     128U

/** Stack fill pattern used to detect stack overflow at debug time. */
#define RTOS_STACK_FILL_BYTE    0xCDU

/* =========================================================================
 * Priority
 * ========================================================================= */

/** Total number of discrete priority levels (0 = highest, N-1 = lowest). */
#define RTOS_PRIORITY_LEVELS    8U

/** Priority assigned to the idle task (lowest possible). */
#define RTOS_IDLE_PRIORITY      (RTOS_PRIORITY_LEVELS - 1U)

/** Priority assigned to the highest-urgency tasks. */
#define RTOS_HIGHEST_PRIORITY   0U

/* =========================================================================
 * Scheduler
 * ========================================================================= */

/** Enable (1) or disable (0) round-robin within the same priority level.
 *  When enabled, tasks at the same priority share the CPU in a FIFO manner,
 *  each getting one tick before the next task at the same level is run. */
#define RTOS_ROUND_ROBIN_ENABLE 1U

/** Maximum time-slice (ticks) granted to a task before forced preemption.
 *  Only meaningful when RTOS_ROUND_ROBIN_ENABLE == 1. */
#define RTOS_TIME_SLICE_TICKS   10U

/* =========================================================================
 * Synchronisation
 * ========================================================================= */

/** Maximum number of mutexes that can be created simultaneously. */
#define RTOS_MAX_MUTEXES        8U

/** Maximum number of semaphores that can be created simultaneously. */
#define RTOS_MAX_SEMAPHORES     8U

/** Maximum number of tasks that can block on a single mutex or semaphore. */
#define RTOS_MAX_WAITERS        8U

/* =========================================================================
 * Debug / Safety
 * ========================================================================= */

/** Enable (1) stack overflow checking at every context switch.
 *  Disable (0) in production to save CPU cycles. */
#define RTOS_STACK_CHECK_ENABLE 1U

/** Enable (1) assertion macros; they trap to an infinite loop on failure. */
#define RTOS_ASSERT_ENABLE      1U

#if RTOS_ASSERT_ENABLE
  #define RTOS_ASSERT(cond)  do { if (!(cond)) { for(;;); } } while(0)
#else
  #define RTOS_ASSERT(cond)  ((void)0)
#endif

/* =========================================================================
 * Cortex-M Specific
 * ========================================================================= */

/** PendSV interrupt priority (lowest possible — 0xFF for 8-bit NVIC field).
 *  Context switches are triggered via PendSV at the lowest priority so that
 *  all real ISRs finish before the switch happens. */
#define RTOS_PENDSV_PRIORITY    0xFFU

/** SysTick interrupt priority (one step above PendSV). */
#define RTOS_SYSTICK_PRIORITY   0xFEU

#endif /* CONFIG_H */
