/**
 * @file    systick_handler.c
 * @brief   Mini RTOS – SysTick ISR Implementation
 *
 * The SysTick peripheral is configured by rtos_start() in rtos_kernel.c.
 * This file only provides the ISR body; no hardware initialisation is done
 * here so the same file works with any Cortex-M MCU.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "systick_handler.h"
#include "rtos_kernel.h"

/**
 * @brief  SysTick_Handler — CMSIS-compatible periodic tick ISR.
 *
 * Fires every RTOS_TICK_MS milliseconds (configured in config.h).
 * Calls rtos_tick() which:
 *   1. Increments g_rtos_tick_count.
 *   2. Decrements sleep counters and wakes expired tasks.
 *   3. Calls scheduler_tick() which handles time-slice preemption and
 *      priority-based preemption, pending PendSV if a switch is needed.
 */
void SysTick_Handler(void)
{
    rtos_tick();
}
