# Mini RTOS – Priority-Based Scheduler for ARM Cortex-M

A fully-implemented, from-scratch **Mini Real-Time Operating System (RTOS)** targeting ARM Cortex-M microcontrollers (STM32F103 "Blue Pill").  Built as a learning project in **Keil MDK µVision 5**.

## Features

- **Priority-based preemptive scheduler** — 8 priority levels; higher-priority tasks always preempt lower-priority ones
- **Round-robin within the same priority** — configurable time-slice length
- **Task management** — create, delete, suspend, resume
- **Context switching** — hand-coded ARM Cortex-M PendSV handler (saves/restores R4–R11)
- **Sleep/delay** — non-blocking `rtos_delay(ticks)` via SysTick countdown
- **Mutex** — binary mutex with priority inheritance (prevents priority inversion)
- **Semaphore** — counting semaphore for producer/consumer patterns
- **Idle task** — WFI-based to save power when nothing is ready

## Quick Start

1. **Read the setup guide:** [`SETUP_GUIDE.md`](SETUP_GUIDE.md)  
   Complete walkthrough from fresh Keil installation → building → flashing.

2. **Open the project:** `project/MiniRTOS.uvprojx` in Keil µVision 5.

3. **Build:** Press **F7**.

4. **Flash:** Connect an ST-Link V2 and press **F8**.

## Project Layout

```
keil/
├── SETUP_GUIDE.md          ← Complete step-by-step guide
├── project/
│   └── MiniRTOS.uvprojx    ← Keil project file
└── src/
    ├── include/
    │   └── config.h        ← RTOS tuning knobs (tick rate, task limits, etc.)
    ├── kernel/             ← RTOS kernel
    │   ├── rtos_kernel.h/c     Core: init, start, tick, delay, yield
    │   ├── task_manager.h/c    Task creation/deletion/suspend/resume
    │   ├── scheduler.h/c       Priority queues + round-robin
    │   ├── context_switch.h/c  PendSV handler (ARM assembly)
    │   ├── synchronization.h/c Mutex + semaphore
    │   └── systick_handler.h/c SysTick ISR
    └── app/                ← Example applications
        ├── main.c              Three tasks at different priorities + mutex
        ├── led_blink_app.c     Multi-LED blink with suspend/resume demo
        └── sync_example.c      Mutex counter, producer/consumer, priority inheritance
```

## Target Hardware

| Parameter | Value |
|---|---|
| MCU | STM32F103C8T6 ("Blue Pill") |
| Core | ARM Cortex-M3 |
| Clock | 72 MHz |
| Flash | 64 KB |
| RAM | 20 KB |
| IDE | Keil MDK µVision 5 |
| Compiler | ARM Compiler 6 (clang-based) |

Any Cortex-M3/M4/M33 device works with minor changes to `config.h` (update `RTOS_SYSCLK_HZ`) and the GPIO init code in `main.c`.

## Learning Topics Covered

| Concept | Where to find it |
|---|---|
| Task Control Block (TCB) | `src/kernel/rtos_kernel.h` |
| Initial stack frame layout | `src/kernel/task_manager.c` (top comment) |
| PendSV context switch | `src/kernel/context_switch.c` |
| Priority queue implementation | `src/kernel/scheduler.c` |
| Priority inheritance | `src/kernel/synchronization.c` (`rtos_mutex_lock`) |
| SysTick integration | `src/kernel/systick_handler.c` + `rtos_kernel.c` |
| End-to-end example | `src/app/main.c` |
