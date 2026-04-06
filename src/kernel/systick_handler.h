/**
 * @file    systick_handler.h
 * @brief   Mini RTOS – SysTick and Interrupt Handler Integration
 *
 * Exposes the SysTick_Handler prototype (must match the CMSIS/startup
 * vector table name exactly) and helper functions for managing
 * interrupt-driven time keeping.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef SYSTICK_HANDLER_H
#define SYSTICK_HANDLER_H

#include <stdint.h>

/**
 * @brief  SysTick exception handler.
 *
 * Called automatically by the Cortex-M CPU every RTOS_TICK_MS milliseconds.
 * This function calls rtos_tick() which updates sleep counters and drives
 * the scheduler, then returns to whatever code was interrupted.
 *
 * The name "SysTick_Handler" is the CMSIS-standard weak-symbol name;
 * defining it here overrides the default (empty) handler in the startup file.
 */
void SysTick_Handler(void);

#endif /* SYSTICK_HANDLER_H */
