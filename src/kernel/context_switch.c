/**
 * @file    context_switch.c
 * @brief   Mini RTOS – ARM Cortex-M PendSV Context Switch
 *
 * This file contains the PendSV_Handler written entirely in inline assembly.
 * It is the heart of the context-switch mechanism.
 *
 * ── Why PendSV? ───────────────────────────────────────────────────────────
 * PendSV is set to the lowest interrupt priority so it only fires after all
 * other ISRs (including SysTick) have finished.  This prevents a context
 * switch from interrupting an ISR.
 *
 * ── Cortex-M Exception Entry/Exit (brief recap) ────────────────────────
 * When any exception fires, the CPU hardware automatically saves
 *   {R0, R1, R2, R3, R12, LR, PC, xPSR}  onto the current stack (PSP or MSP).
 * We call this the "hardware frame".
 *
 * PendSV_Handler additionally saves the callee-saved registers
 *   {R4, R5, R6, R7, R8, R9, R10, R11}   manually onto PSP.
 * We call this the "software frame".
 *
 * On restore, we pop the software frame manually, then use EXC_RETURN
 * (0xFFFFFFFD) to tell the CPU to pop the hardware frame from PSP and
 * resume Thread mode on the new task's stack.
 *
 * ── Stack layout after handler entry ────────────────────────────────────
 *
 *  PSP (before our push) →  xPSR    (top of hardware frame)
 *                            PC
 *                            LR
 *                            R12
 *                            R3
 *                            R2
 *                            R1
 *                            R0     ← PSP+0 when exception entry completed
 *
 *  After "STMDB R0!, {R4-R11}" :
 *
 *                            R4
 *                            R5
 *                            R6
 *                            R7
 *                            R8
 *                            R9
 *                            R10
 *                            R11    ← new PSP (= R0 after STMDB)
 *
 * Target: ARM Cortex-M3/M4/M33 (Thumb-2 instruction set)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "context_switch.h"
#include "rtos_kernel.h"

/* =========================================================================
 * PendSV_Handler
 * ========================================================================= */

/**
 * @brief  Naked PendSV handler — the RTOS context switch engine.
 *
 * __attribute__((naked)) suppresses compiler prologue/epilogue generation.
 * The function body is pure assembly.
 */
__attribute__((naked)) void PendSV_Handler(void)
{
    __asm volatile (
        /* ── Disable interrupts during the switch ──────────────────────── */
        "CPSID   I                          \n"

        /* ── Save callee-saved registers of the outgoing task ─────────── */
        /* R0 = PSP (current task's stack pointer after hardware save)     */
        "MRS     R0, PSP                    \n"
        "ISB                                \n"

        /* Push R4-R11 onto PSP (R0 updated to new SP after push).
         * STMDB R0!, {R4-R11}  is PUSH equivalent for a full descending stack */
        "STMDB   R0!, {R4-R11}              \n"

        /* ── Save new PSP into g_current_task->sp (offset 0) ─────────── */
        "LDR     R1, =g_current_task        \n"
        "LDR     R1, [R1]                   \n"   /* R1 = g_current_task (TCB ptr) */
        "STR     R0, [R1]                   \n"   /* TCB->sp = R0 (updated PSP)    */

        /* ── Switch g_current_task = g_next_task ─────────────────────── */
        "LDR     R2, =g_next_task           \n"
        "LDR     R3, [R2]                   \n"   /* R3 = g_next_task */
        "LDR     R1, =g_current_task        \n"
        "STR     R3, [R1]                   \n"   /* g_current_task = g_next_task  */

        /* ── Load incoming task's SP ─────────────────────────────────── */
        "LDR     R0, [R3]                   \n"   /* R0 = new TCB->sp              */

        /* ── Restore callee-saved registers of the incoming task ─────── */
        "LDMIA   R0!, {R4-R11}              \n"   /* Pop R4-R11; R0 updated        */

        /* ── Update PSP and re-enable interrupts ─────────────────────── */
        "MSR     PSP, R0                    \n"   /* Set PSP = top of hardware frame */
        "ISB                                \n"
        "CPSIE   I                          \n"

        /* ── Exception return ────────────────────────────────────────── */
        /* EXC_RETURN = 0xFFFFFFFD:
         *   bits [3:0] = 0b1101 → return to Thread mode, use PSP, no FP */
        "LDR     R0, =0xFFFFFFFD            \n"
        "BX      R0                         \n"
        ::: "memory"
    );
}
