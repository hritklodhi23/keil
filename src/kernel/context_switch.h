/**
 * @file    context_switch.h
 * @brief   Mini RTOS – Context Switch Public API
 *
 * The actual context-switch code lives in context_switch.c and is written
 * in inline assembly (ARM Cortex-M PendSV handler).  This header exposes
 * the handler prototype so the startup/CMSIS vector table can reference it.
 *
 * Target: ARM Cortex-M (Cortex-M3/M4/M33)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#ifndef CONTEXT_SWITCH_H
#define CONTEXT_SWITCH_H

/**
 * @brief  PendSV exception handler — performs the actual context switch.
 *
 * This function is called automatically by the Cortex-M CPU when the PendSV
 * exception is pending.  It is listed as __attribute__((naked)) so the
 * compiler does not generate a prologue/epilogue — the handler manages the
 * stack entirely in assembly.
 *
 * Sequence:
 *  1. Save callee-saved registers R4-R11 on the current PSP.
 *  2. Store updated PSP into g_current_task->sp.
 *  3. g_current_task = g_next_task.
 *  4. Load g_current_task->sp into PSP.
 *  5. Restore R4-R11 from the new stack.
 *  6. Exception return (EXC_RETURN = 0xFFFFFFFD → Thread mode, PSP).
 */
void PendSV_Handler(void);

#endif /* CONTEXT_SWITCH_H */
