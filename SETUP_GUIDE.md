# Mini RTOS (Priority-Based Scheduler) – Complete Setup Guide

> **Skill level:** Beginner–Intermediate embedded systems developer  
> **Time required:** 2–4 hours  
> **Target hardware:** STM32F103C8T6 ("Blue Pill") — ARM Cortex-M3, 72 MHz  
> **IDE:** Keil MDK µVision 5 (Windows)

---

## Table of Contents

1. [Phase 1 – Environment Setup](#phase-1--environment-setup)
   - [1.1 Remove an Existing Keil Installation](#11-remove-an-existing-keil-installation)
   - [1.2 Download Keil MDK](#12-download-keil-mdk)
   - [1.3 Install Keil MDK](#13-install-keil-mdk)
   - [1.4 Install the STM32F1 Device Pack](#14-install-the-stm32f1-device-pack)
   - [1.5 Obtain a Free Licence (MDK-Community)](#15-obtain-a-free-licence-mdk-community)
2. [Phase 2 – Project Structure](#phase-2--project-structure)
   - [2.1 Repository Layout](#21-repository-layout)
   - [2.2 Create the Project in Keil](#22-create-the-project-in-keil)
   - [2.3 Add Source Files to the Project](#23-add-source-files-to-the-project)
   - [2.4 Configure Include Paths](#24-configure-include-paths)
3. [Phase 3 – RTOS Architecture Deep Dive](#phase-3--rtos-architecture-deep-dive)
   - [3.1 Task Control Block (TCB)](#31-task-control-block-tcb)
   - [3.2 Task States and Transitions](#32-task-states-and-transitions)
   - [3.3 Priority Queue](#33-priority-queue)
   - [3.4 Context Switching (PendSV)](#34-context-switching-pendsv)
   - [3.5 Scheduler Logic](#35-scheduler-logic)
   - [3.6 Task Creation and Deletion](#36-task-creation-and-deletion)
   - [3.7 Sleep / Delay](#37-sleep--delay)
   - [3.8 Mutex and Semaphore](#38-mutex-and-semaphore)
   - [3.9 Interrupt Integration (SysTick)](#39-interrupt-integration-systick)
4. [Phase 4 – Example Applications](#phase-4--example-applications)
   - [4.1 Main Demo (main.c)](#41-main-demo-mainc)
   - [4.2 Multi-LED Blink (led_blink_app.c)](#42-multi-led-blink-led_blink_appc)
   - [4.3 Synchronisation Examples (sync_example.c)](#43-synchronisation-examples-sync_examplec)
5. [Phase 5 – Compilation and Debugging](#phase-5--compilation-and-debugging)
   - [5.1 Compiler Settings](#51-compiler-settings)
   - [5.2 Linker Settings](#52-linker-settings)
   - [5.3 Building the Project](#53-building-the-project)
   - [5.4 Flashing to Hardware](#54-flashing-to-hardware)
   - [5.5 Debugging with µVision](#55-debugging-with-µvision)
6. [Troubleshooting](#troubleshooting)
7. [Further Reading](#further-reading)

---

## Phase 1 – Environment Setup

### 1.1 Remove an Existing Keil Installation

If you have an older version of Keil MDK installed and want a completely fresh start:

1. **Close Keil µVision** if it is running.
2. Open **Control Panel → Programs → Programs and Features**.
3. Find **Keil MDK-ARM** (or "MDK-ARM Vx.xx") in the list.
4. Click **Uninstall** and follow the wizard.
5. After uninstallation, manually delete leftover folders (optional but recommended for a clean slate):
   - `C:\Keil_v5\`  (default install path)
   - `C:\Users\<YourName>\AppData\Roaming\ARM\`
6. **Reboot** your PC.

> 💡 **Tip:** If you only want to update Keil (not do a fresh install), you can skip to §1.2 and install the new version on top — the installer will upgrade in place.

---

### 1.2 Download Keil MDK

1. Open your browser and go to:  
   **https://www.keil.arm.com/mdk-community/**

2. Click **"Download MDK"**.

3. Fill in the short registration form (name, email, country).

4. You will be emailed a download link **and** a Community licence activation code.  
   The installer file is named something like `MDK536.EXE` (the number is the version).

> **Which edition to choose?**  
> | Edition | Code size limit | Cost | Use case |
> |---|---|---|---|
> | MDK-Community | Unlimited (non-commercial) | Free | Learning / hobby |
> | MDK-Essential | 256 KB | Free with registration | Small commercial projects |
> | MDK-Plus/Professional | Unlimited | Paid | Commercial products |
>
> For this guide, **MDK-Community** is sufficient.

---

### 1.3 Install Keil MDK

1. Run `MDK536.EXE` as **Administrator** (right-click → "Run as administrator").
2. Accept the licence agreement.
3. Choose the install directory. The default `C:\Keil_v5\` is fine.
4. Click **Next** through the wizard. The installer will:
   - Install the ARM Compiler 6 (clang-based)
   - Install the ARM Compiler 5 (legacy, optional)
   - Install µVision IDE
   - Install the Pack Installer
5. When installation completes, **do not launch** µVision yet — install the device pack first (§1.4).

---

### 1.4 Install the STM32F1 Device Pack

Device packs provide the startup files, CMSIS headers, flash algorithms, and SVD debugger register views for a specific MCU family.

**Method A — Pack Installer (recommended)**

1. Launch Keil µVision 5.
2. Go to **Project → Manage → Pack Installer** (or click the Pack Installer icon in the toolbar).
3. In the left pane, expand **STMicroelectronics → STM32F1 Series**.
4. In the right pane, click **Install** next to **Keil::STM32F1xx_DFP** (Device Family Pack).
5. Wait for the download to complete (≈ 50 MB).

**Method B — Offline (if you have no internet in the lab)**

1. Download the pack file from another machine:  
   `https://www.keil.arm.com/packs/stm32f1xx_dfp-keil/`
2. Copy the `.pack` file to the target PC.
3. In µVision, open **Pack Installer → File → Import**.
4. Select the downloaded `.pack` file.

---

### 1.5 Obtain a Free Licence (MDK-Community)

1. In µVision, go to **File → Licence Management**.
2. Click **"Get LIC via Internet"** (requires internet access).
3. Enter the activation code from the email you received in §1.2.
4. Click **Add LIC**. The licence should appear in the list with status **Active**.

> **No internet?** Use the Single-User Licence (PSN) method described at  
> https://developer.arm.com/documentation/ka004088/latest

---

## Phase 2 – Project Structure

### 2.1 Repository Layout

After cloning (or downloading) this repository you will see:

```
keil/                          ← repository root
├── SETUP_GUIDE.md             ← this file
├── README.md
├── project/
│   ├── MiniRTOS.uvprojx       ← Keil project file (open this!)
│   └── MiniRTOS_notes.txt
└── src/
    ├── include/
    │   └── config.h           ← RTOS configuration (edit this to tune)
    ├── kernel/                ← RTOS kernel source files
    │   ├── rtos_kernel.h
    │   ├── rtos_kernel.c
    │   ├── task_manager.h
    │   ├── task_manager.c
    │   ├── scheduler.h
    │   ├── scheduler.c
    │   ├── context_switch.h
    │   ├── context_switch.c
    │   ├── synchronization.h
    │   ├── synchronization.c
    │   ├── systick_handler.h
    │   └── systick_handler.c
    └── app/                   ← Example application files
        ├── main.c
        ├── led_blink_app.c
        └── sync_example.c
```

---

### 2.2 Create the Project in Keil

> **Shortcut:** If you want to use the pre-configured project file, simply open  
> `project/MiniRTOS.uvprojx` in µVision and skip to §2.4.

To create the project manually from scratch:

1. In µVision: **Project → New µVision Project…**
2. Navigate to the `project/` folder inside your repository clone.
3. Name the project `MiniRTOS` and click **Save**.
4. The **Select Device** dialog opens. Search for `STM32F103C8`:
   - Expand: **STMicroelectronics → STM32F1 Series → STM32F103 → STM32F103C8**
   - Click **OK**.
5. The **Manage Run-Time Environment** dialog opens.  
   For this bare-metal project, **uncheck everything** and click **OK**.  
   *(We are not using the CMSIS-RTOS or HAL layers — everything is implemented from scratch.)*

---

### 2.3 Add Source Files to the Project

1. In the **Project** pane (left side), right-click **Target 1** → **Manage Project Items…**

2. Create the following **Groups** and add files:

   | Group name | Files to add |
   |---|---|
   | `Kernel` | `../src/kernel/rtos_kernel.c` |
   | `Kernel` | `../src/kernel/task_manager.c` |
   | `Kernel` | `../src/kernel/scheduler.c` |
   | `Kernel` | `../src/kernel/context_switch.c` |
   | `Kernel` | `../src/kernel/synchronization.c` |
   | `Kernel` | `../src/kernel/systick_handler.c` |
   | `Application` | `../src/app/main.c` |
   | `Application` | `../src/app/led_blink_app.c` |
   | `Application` | `../src/app/sync_example.c` |

   > **Note on led_blink_app.c and sync_example.c:** These files define their own
   > entry-point functions (`led_blink_app_init()` and `sync_example_init()`), NOT
   > a second `main()`.  They are included in the build but are only called if
   > you explicitly invoke them from `main.c`.  You can safely add all three files
   > to the project — there is only one `main()`.

3. Click **OK** to close the dialog.

---

### 2.4 Configure Include Paths

The compiler needs to know where to find the header files.

1. Right-click the **Target** → **Options for Target…** (shortcut: **Alt+F7**).
2. Select the **C/C++ (AC6)** tab.
3. In the **Include Paths** field, click the `…` button and add:
   - `../src/include`
   - `../src/kernel`
4. In the **Define** field, add: `STM32F103xB`
5. Click **OK**.

---

## Phase 3 – RTOS Architecture Deep Dive

This section explains the design of every kernel component so you understand *why* the code is written the way it is.

---

### 3.1 Task Control Block (TCB)

Every task in the Mini RTOS is represented by a `rtos_tcb_t` structure defined in `rtos_kernel.h`:

```c
typedef struct rtos_tcb {
    uint32_t         *sp;           // Saved stack pointer  ← MUST be at offset 0
    uint32_t         *stack_base;   // Bottom of stack (for overflow checking)
    uint32_t          stack_size;   // Stack size in bytes
    rtos_task_func_t  func;         // Task entry function
    void             *arg;          // Argument to task function
    uint8_t           priority;     // 0 = highest priority
    rtos_task_state_t state;        // READY / RUNNING / BLOCKED / SUSPENDED / DELETED
    uint32_t          sleep_ticks;  // Remaining ticks until wake-up
    uint32_t          slice_ticks;  // Remaining time-slice ticks (round-robin)
    char              name[16];     // Debug name
    struct rtos_tcb  *next;         // Next TCB in a linked list (scheduler queue)
} rtos_tcb_t;
```

**Why must `sp` be at offset 0?**  
The PendSV assembly handler accesses `TCB->sp` with a single `LDR/STR` at `[TCB_ptr, #0]`.  
Moving `sp` to any other position would break the context switch without any compiler warning.

---

### 3.2 Task States and Transitions

```
         rtos_task_create()
                │
                ▼
             READY  ◄─────── BLOCKED (sleep expired / mutex released)
                │                  ▲
                │ scheduler picks  │
                ▼                  │
            RUNNING ───────────────┘ (calls rtos_delay / rtos_mutex_lock)
                │
                │ rtos_task_suspend()
                ▼
           SUSPENDED
                │
                │ rtos_task_resume()
                ▼
             READY
```

| State | Meaning |
|---|---|
| `TASK_READY` | In the ready queue; eligible to be scheduled |
| `TASK_RUNNING` | Currently executing on the CPU |
| `TASK_BLOCKED` | Waiting for a timer, mutex, or semaphore |
| `TASK_SUSPENDED` | Explicitly paused; not eligible to run |
| `TASK_DELETED` | TCB slot is free for reuse |

---

### 3.3 Priority Queue

The scheduler maintains `RTOS_PRIORITY_LEVELS` (default: 8) singly-linked FIFO queues — one per priority level — declared in `scheduler.c`:

```
Priority 0 (highest):  [TCB_A] → [TCB_B] → NULL
Priority 1:            [TCB_C] → NULL
Priority 2:            NULL
...
Priority 7 (idle):     [idle_TCB] → NULL
```

**Enqueue** (`scheduler_add_task`) appends to the tail in O(1).  
**Dequeue** (`scheduler_get_next`) removes the head of the lowest-numbered non-empty queue in O(N_priorities).

With only 8 priority levels the O(N) scan is negligible — no bitmask tricks are needed for this learning implementation.

---

### 3.4 Context Switching (PendSV)

The Cortex-M CPU has a special exception called **PendSV** (Pendable Service Call) designed specifically for context switching.

**Why PendSV (not SysTick directly)?**  
If we did the context switch inside SysTick, we could interrupt another ISR mid-execution. PendSV is set to the *lowest* interrupt priority so it only fires after every higher-priority ISR has finished.

**PendSV sequence (from `context_switch.c`):**

```
1. CPU enters PendSV_Handler automatically.
   Hardware has already saved {R0-R3, R12, LR, PC, xPSR} onto PSP.

2. Handler saves callee-saved registers {R4-R11} onto PSP:
       STMDB R0!, {R4-R11}   ; R0 = current PSP, updated after push

3. Store updated PSP into g_current_task->sp (at offset 0):
       STR   R0, [R1]        ; R1 = g_current_task

4. Switch: g_current_task = g_next_task

5. Load new task's SP:
       LDR   R0, [R3]        ; R3 = g_next_task

6. Restore {R4-R11} from new stack:
       LDMIA R0!, {R4-R11}

7. Set PSP = updated R0 (top of hardware frame).

8. EXC_RETURN (0xFFFFFFFD):
   CPU automatically restores {R0-R3, R12, LR, PC, xPSR} from PSP
   and resumes execution in Thread mode.
```

**Initial stack frame** (created by `task_manager.c` during `rtos_task_create()`):

```
High address (top of buffer)
  ┌──────────────┐
  │ xPSR         │  = 0x01000000  (Thumb bit SET)
  │ PC           │  = task function address
  │ LR           │  = task_exit_stub
  │ R12          │  = 0
  │ R3           │  = 0
  │ R2           │  = 0
  │ R1           │  = 0
  │ R0           │  = arg
  ├──────────────┤  ← hardware frame (8 words)
  │ R11–R4       │  = 0 (8 words)
  ├──────────────┤  ← TCB->sp points here
  │ (free stack) │
Low address (bottom of buffer)
```

---

### 3.5 Scheduler Logic

The scheduler (`scheduler.c`) decides which task runs next. It is called:

- **Every SysTick** via `scheduler_tick()` to handle time-slice rotation and priority preemption.
- **On demand** via `scheduler_get_next()` when a task blocks or yields.

**Decision flowchart (inside `scheduler_tick()`):**

```
Any task in queue with priority < current task's priority?
  YES → Preempt: put current task back in queue, pick new task, pend PendSV
  NO  → (Round-robin enabled?) Decrement slice counter
            slice_ticks == 0?
              YES → Rotate: put current at queue tail, pick queue head, pend PendSV
              NO  → Continue running current task
```

---

### 3.6 Task Creation and Deletion

**`rtos_task_create()`** does four things:
1. Validates parameters (stack size, priority range, alignment).
2. Finds a free TCB slot in the statically-allocated pool.
3. Fills the initial stack frame (fake exception frame + R4–R11).
4. Adds the TCB to the scheduler's ready queue.

**`rtos_task_delete()`** does:
1. Removes the task from the scheduler.
2. Marks the TCB as `TASK_DELETED` (freeing the slot).
3. If deleting self: triggers an immediate context switch via PendSV.

Stack memory is managed by the *caller* — the RTOS does not dynamically allocate or free stacks. This is intentional: it avoids heap fragmentation and makes memory usage predictable.

---

### 3.7 Sleep / Delay

```c
void rtos_delay(uint32_t ticks);
```

1. Sets `current_task->sleep_ticks = ticks`.
2. Changes state to `TASK_BLOCKED`.
3. Triggers a context switch.
4. Every SysTick decrements `sleep_ticks` for all BLOCKED tasks.
5. When `sleep_ticks` reaches 0, the task is moved to READY and re-queued.

This is a **non-blocking delay** — the CPU is free to run other tasks while a task is sleeping.

---

### 3.8 Mutex and Semaphore

**Semaphore** (`rtos_semaphore_t`):
- Counting (configurable max) or binary (max = 1).
- `rtos_sem_wait()`: decrement count; block if count would go negative.
- `rtos_sem_post()`: increment count; wake the highest-priority waiter.

**Mutex** (`rtos_mutex_t`):
- Binary only (locked / unlocked).
- Tracks the *owner* — only the owner can unlock it.
- **Priority inheritance:** when a high-priority task blocks on a mutex held by a low-priority task, the low-priority task's priority is temporarily elevated to match.  This prevents *priority inversion* (where a medium-priority task could indefinitely delay the high-priority task).

---

### 3.9 Interrupt Integration (SysTick)

SysTick is the heartbeat of the RTOS:

```
Every RTOS_TICK_MS ms:
  SysTick interrupt fires
    → SysTick_Handler() in systick_handler.c
        → rtos_tick() in rtos_kernel.c
            → Decrement sleep counters, wake expired tasks
            → scheduler_tick() in scheduler.c
                → Check for preemption or time-slice expiry
                → Pend PendSV if switch needed
  SysTick handler returns
  PendSV fires (lowest priority — after all other ISRs)
    → PendSV_Handler() in context_switch.c
        → Save R4-R11 of outgoing task
        → Switch g_current_task = g_next_task
        → Restore R4-R11 of incoming task
        → EXC_RETURN → CPU restores R0-R3, R12, LR, PC, xPSR
  New task resumes execution
```

---

## Phase 4 – Example Applications

### 4.1 Main Demo (`main.c`)

**What it does:**
- Creates three tasks at priorities 1, 3, and 5.
- Each task blinks the on-board LED at a different rate (200 ms, 500 ms, 1000 ms).
- A shared mutex ensures only one task drives the LED at a time.

**Expected behaviour on Blue Pill:**
- LED blinks in complex overlapping pattern driven by the three tasks.
- High-priority task (200 ms) preempts the others immediately when its delay expires.

**Steps to run:**
1. Build the project (F7).
2. Connect ST-Link programmer to Blue Pill.
3. Flash (F8).
4. Observe LED blinking.

---

### 4.2 Multi-LED Blink (`led_blink_app.c`)

**What it does:**
- Three LED blink tasks at 100 ms, 500 ms, and 1000 ms.
- A controller task (priority 1) suspends the slow LED task after 5 s, then resumes it after another 5 s.

**Key concepts demonstrated:**
- `rtos_task_suspend()` / `rtos_task_resume()` from a different task.
- The suspended task's LED stops blinking while suspended.
- The controller task preempts immediately when its delay expires (priority 1 vs 4 and 6).

**To enable this example:**
Replace the body of `main()` in `main.c` with a call to `led_blink_app_init()`.

---

### 4.3 Synchronisation Examples (`sync_example.c`)

Three sub-examples in one file:

| Example | Primitives used | What to observe |
|---|---|---|
| Protected shared counter | Mutex | Counter reaches exactly 200 after both tasks finish |
| Producer / consumer | Semaphore | Consumer never runs ahead of producer |
| Priority inheritance | Mutex | High-priority task is not blocked indefinitely by medium-priority task |

**To enable this example:**
Replace the body of `main()` with a call to `sync_example_init()`.

---

## Phase 5 – Compilation and Debugging

### 5.1 Compiler Settings

Open **Options for Target** (Alt+F7) → **C/C++ (AC6)** tab:

| Setting | Recommended value | Why |
|---|---|---|
| Optimisation | `-O1` (debug) / `-O2` (release) | `-O0` may break `volatile` / inline assembly assumptions |
| C Standard | C99 | Required for `//` comments and `<stdint.h>` |
| Warnings | `-W2` (or `-Wall`) | Catch common mistakes early |
| Define | `STM32F103xB` | Selects correct peripheral map in CMSIS headers |
| Include paths | `../src/include;../src/kernel` | Find `config.h` and all kernel headers |

> ⚠️ **Do NOT use `-O3`** for the initial bring-up. Aggressive optimisation can reorder
> operations around `volatile` accesses in ways that break bare-metal code.

---

### 5.2 Linker Settings

Open **Options for Target** → **Linker** tab:

1. Leave **Use Memory Layout from Target Dialog** checked (default).
2. The target dialog (Device tab) already sets:
   - Flash: `0x08000000`, size `0x10000` (64 KB)
   - RAM:   `0x20000000`, size `0x5000`  (20 KB)
3. No custom scatter file is needed for this project.

> If you want more control, you can provide a custom `.sct` file — see ARM documentation
> for the scatter-loading description language.

---

### 5.3 Building the Project

1. Press **F7** (or **Project → Build Target**).
2. Check the **Build Output** pane at the bottom for errors and warnings.
3. A successful build shows:
   ```
   Program Size: Code=xxxx RO-data=xx RW-data=xx ZI-data=xxxx
   "MiniRTOS.axf" - 0 Error(s), 0 Warning(s).
   ```

**Common build errors:**

| Error | Cause | Fix |
|---|---|---|
| `undefined symbol rtos_tick` | systick_handler.c not in project | Add the file to the Kernel group |
| `Cannot open source file "config.h"` | Include path not set | Add `../src/include` to include paths |
| `error: use of undeclared identifier 'g_current_task'` | rtos_kernel.h not included | Add `#include "rtos_kernel.h"` at the top of the file |
| `'uint32_t' undeclared` | Missing stdint.h | Add `#include <stdint.h>` or include rtos_kernel.h which includes it |

---

### 5.4 Flashing to Hardware

**Using ST-Link V2:**
1. Connect the ST-Link to the Blue Pill's SWD header (4 pins: 3.3V, GND, SWDIO, SWCLK).
2. In µVision: **Flash → Download** (F8).
3. The build output shows `Programming Done` when complete.
4. Press the Blue Pill's **RESET** button to start the firmware.

**Using a USB-UART adapter (BOOT0 method):**
1. Set BOOT0 jumper to `1` on the Blue Pill.
2. Connect UART adapter: TX→A10(RX), RX→A9(TX), 3.3V, GND.
3. Use **STM32CubeProgrammer** (free, from STMicroelectronics) to flash the `.hex` file from `project/Objects/`.
4. Set BOOT0 back to `0` and press RESET.

---

### 5.5 Debugging with µVision

1. Press **Ctrl+F5** (or **Debug → Start/Stop Debug Session**).
2. µVision enters debug mode and halts the CPU at the start of `main()`.

**Useful debug views:**

| View | Menu path | What it shows |
|---|---|---|
| Watch | View → Watch Windows | Variable values (add `g_rtos_tick_count`, `g_current_task`) |
| Memory | View → Memory Windows | Raw memory at any address |
| Registers | View → Registers | CPU register values (R0–R15, PSR, PRIMASK…) |
| Call Stack | View → Call Stack | Current call chain |
| Peripheral | Peripherals → (device-specific) | Live register values of SysTick, GPIO, etc. |
| OS Support | View → Analysis Windows → OS Support | When using CMSIS-RTOS2 (not used here, but informational) |

**Breakpoint tips:**
- Set a breakpoint inside `scheduler_tick()` to observe every scheduling decision.
- Set a breakpoint in `PendSV_Handler()` to step through a context switch (use **Step Into** / F11 carefully — the assembly handler is delicate to step through).
- Use **Run to Cursor** (Ctrl+F10) to avoid stopping at every tick.

---

## Troubleshooting

| Symptom | Likely cause | Solution |
|---|---|---|
| CPU immediately hard-faults on startup | Stack overflow or null pointer in TCB | Check `RTOS_DEFAULT_STACK_SIZE` in `config.h`; increase to 1024 initially |
| LED never blinks | Wrong GPIO pin / clock not enabled | Verify `RCC_APB2ENR` bit for GPIOC; check pin number in `main.c` |
| Tasks seem to run sequentially (no preemption) | PendSV priority not set lowest | Verify SHPR3 write in `rtos_kernel_init()`; check with Memory view at `0xE000ED20` |
| HardFault in PendSV handler | PSP misalignment or wrong EXC_RETURN value | Ensure stacks are 8-byte aligned; verify initial stack frame in `task_manager.c` |
| Debugger can't stop at breakpoints | Optimisation reordering code | Reduce optimisation to `-O0` for debugging |
| `rtos_delay()` sleeps for the wrong duration | `RTOS_SYSCLK_HZ` mismatch | Confirm your MCU's actual clock speed and update `config.h` |
| Mutex causes deadlock | Task locks mutex twice (no recursive mutex support) | Each task should lock the mutex once, do work, then unlock |
| `g_shared_counter` not exactly 200 | Mutex not actually protecting the increment | Verify mutex init was called; check that `rtos_mutex_lock` is called before the read |

---

## Further Reading

| Topic | Resource |
|---|---|
| ARM Cortex-M exception model | [ARM Architecture Reference Manual (ARMv7-M)](https://developer.arm.com/documentation/ddi0403) |
| PendSV context switch tutorial | [Joseph Yiu – "The Definitive Guide to ARM Cortex-M3 and Cortex-M4" Chapter 9](https://www.amazon.com/dp/0124080820) |
| CMSIS API reference | https://arm-software.github.io/CMSIS_5/ |
| FreeRTOS source (production RTOS for comparison) | https://github.com/FreeRTOS/FreeRTOS-Kernel |
| STM32F103 Reference Manual | https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf |
| Keil MDK Documentation | https://developer.arm.com/documentation/101407 |

---

*Guide written for the Mini RTOS learning project.  
All code is original and intended for educational purposes.*
