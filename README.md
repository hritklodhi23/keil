# Keil Learning Repository: Embedded Systems Fundamentals

This is a **learning repository** to build embedded systems knowledge from scratch — from C fundamentals to RTOS architecture — using **Keil MDK µVision 5** and the **STM32F103 simulator**.

## 🎯 What's Here?

### 📚 **Phase 0: C Memory & Debugging** (START HERE!)
Foundational C skills you need before buying hardware.

**→ New to Keil?** Follow this: [`PHASE0_CIRCULAR_BUFFER_GUIDE.md`](PHASE0_CIRCULAR_BUFFER_GUIDE.md)

- ✅ **Circular Buffer** — master FIFO queues, pointers, modulo arithmetic
- ✅ **Linked List** — understand pointer-to-pointer, malloc/free
- ✅ **Bit Manipulation** — bitwise operators, masks, shifts
- ✅ **GPIO Simulation** — learn register programming without hardware

**Track progress:** [`PHASE0_CHECKLIST.md`](PHASE0_CHECKLIST.md)

---

### 🔧 **Phase 1: Microcontroller Depth** (Coming Soon)
Real firmware engineering skills using STM32F103 drivers.

- STM32F103 driver implementations (GPIO, Timer, UART, ADC)
- Command parser & shell
- Data logger with SD card simulation
- State machines & task management

---

### 🚀 **Advanced: Mini RTOS**
A fully-implemented, from-scratch **Real-Time Operating System (RTOS)** targeting ARM Cortex-M3.

**Ready to build?** See [`SETUP_GUIDE.md`](SETUP_GUIDE.md)

#### Features
- **Priority-based preemptive scheduler** — 8 priority levels
- **Round-robin within same priority** — configurable time-slice
- **Task management** — create, delete, suspend, resume
- **Context switching** — hand-coded ARM assembly
- **Sleep/delay** — non-blocking via SysTick
- **Mutex & Semaphore** — synchronization primitives
- **Idle task** — WFI-based power saving

---

## 📋 Project Structure

```
keil/
├── PHASE0_CIRCULAR_BUFFER_GUIDE.md  ← START HERE (detailed Keil walkthrough)
├── PHASE0_CHECKLIST.md              ← Track Phase 0 progress
├── SETUP_GUIDE.md                   ← Mini RTOS guide
├── README.md                        ← This file
│
├── src/
│   ├── phase0_memory/               ← Phase 0.1-0.3: C fundamentals
│   │   ├── circular_buffer.h/c      ✅ FIFO queue implementation
│   │   ├── linked_list.h/c          ✅ Linked list (malloc/free)
│   │   ├── bit_utils.h/c            ✅ Bitwise operations library
│   │   ├── test_circular_buffer.c   ✅ Test program
│   │   └── README.md                ✅ Phase 0 overview
│   │
│   ├── phase0_registers/            ← Phase 0.4: Register programming
│   │   ├── gpio_simulation.h/c      ✅ GPIO register simulator
│   │   └── README.md                (coming soon)
│   │
│   ├── phase1_drivers/              ← Phase 1: Real drivers
│   │   └── (coming soon)
│   │
│   ├── phase1_projects/             ← Phase 1: Practical projects
│   │   └── (coming soon)
│   │
│   ├── kernel/                      ← RTOS kernel (advanced)
│   │   ├── rtos_kernel.h/c
│   │   ├── task_manager.h/c
│   │   ├── scheduler.h/c
│   │   ├── context_switch.h/c
│   │   ├── synchronization.h/c
│   │   └── systick_handler.h/c
│   │
│   └── app/                         ← RTOS example applications
│       ├── main.c
│       ├── led_blink_app.c
│       └── sync_example.c
│
└── project/
    ├── MiniRTOS.uvprojx             ← RTOS project (ready to use)
    └── Phase0_CircularBuffer/       ← (create your own projects here)
```

---

## 🚀 Quick Start (Choose Your Path)

### **Path A: Complete Beginner** (Recommended)
1. Open [`PHASE0_CIRCULAR_BUFFER_GUIDE.md`](PHASE0_CIRCULAR_BUFFER_GUIDE.md)
2. Follow step-by-step to:
   - Launch Keil MDK
   - Create your first project
   - Build & debug circular buffer
   - Learn Keil debugger
3. Use [`PHASE0_CHECKLIST.md`](PHASE0_CHECKLIST.md) to track progress
4. Complete all Phase 0 projects (4-6 weeks)
5. Move to Phase 1

**Time estimate:** 1-2 hours/day for 4-6 weeks → **confident to buy STM32**

---

### **Path B: Experienced with Keil**
1. Go directly to `src/phase0_memory/` 
2. Create Keil projects and add files
3. Write test programs yourself
4. Move to Phase 1 when ready

---

### **Path C: Want RTOS Now** (Not Recommended)
1. See [`SETUP_GUIDE.md`](SETUP_GUIDE.md) to build Mini RTOS project
2. **But** — you'll struggle without Phase 0 foundation
3. Consider starting with Phase 0 anyway

---

## 📖 Learning Roadmap

```
Phase 0 (Weeks 1-6)          Phase 1 (Weeks 7-18)        Phase 2+ (Weeks 18-36)
┌─────────────────────┐      ┌──────────────────────┐    ┌────────────────────┐
│ C Fundamentals      │      │ Microcontroller      │    │ Specialization     │
│ • Pointers          │ ──→  │ Depth                │ → │ • Automotive       │
│ • Memory            │      │ • STM32 Drivers      │   │ • Industrial IoT   │
│ • Debugging         │      │ • UART/SPI/I2C       │   │ • Embedded Linux   │
│ (Software only)     │      │ • Peripherals        │   │ (+ real hardware)  │
│                     │      │ (Simulator mostly)   │   │                    │
└─────────────────────┘      └──────────────────────┘    └────────────────────┘
     4-6 weeks                    8-12 weeks                   18+ weeks
     No hardware needed          No hardware needed           Needs STM32F103
```

---

## 🎓 Target Hardware

| Parameter | Value |
|-----------|-------|
| MCU | STM32F103C8T6 ("Blue Pill") |
| Core | ARM Cortex-M3 |
| Clock | 72 MHz |
| Flash | 64 KB |
| RAM | 20 KB |
| IDE | Keil MDK µVision 5 |
| Compiler | ARM Compiler 6 (LLVM-based) |
| Debugger | Simulator (Phase 0-1), ST-Link V2 (Phase 2+) |

---

## 💡 What You'll Learn

| Phase | Topics | Projects |
|-------|--------|----------|
| **Phase 0** | C pointers, memory, debugging, registers | Circular buffer, linked list, bit utils, GPIO sim |
| **Phase 1** | STM32F103 peripherals, drivers | GPIO driver, UART/SPI/I2C, ADC, timer |
| **Phase 2** | RTOS, task scheduling, IPC | Priority scheduler, mutex, semaphore, task queues |
| **Phase 3** | Specialization | CAN bus, embedded Linux, security, OTA updates |

---

## 🔑 Key Files to Read First

1. **Beginner?** → [`PHASE0_CIRCULAR_BUFFER_GUIDE.md`](PHASE0_CIRCULAR_BUFFER_GUIDE.md) (detailed walkthrough)
2. **Know Keil?** → [`src/phase0_memory/README.md`](src/phase0_memory/README.md) (project overview)
3. **Have hardware?** → [`SETUP_GUIDE.md`](SETUP_GUIDE.md) (Mini RTOS on real device)
4. **Tracking progress?** → [`PHASE0_CHECKLIST.md`](PHASE0_CHECKLIST.md) (milestone tracker)

---

## ⚡ Keil Shortcuts

| Key | Action |
|-----|--------|
| **F7** | Build |
| **F8** | Flash to hardware |
| **Ctrl+F5** | Start debugging |
| **F5** | Run (in debugger) |
| **F10** | Step over |
| **F11** | Step into |
| **F9** | Toggle breakpoint |

---

## 📝 Confidence Milestones (Before Buying STM32)

✅ **Phase 0.1 (Circular Buffer):**
- Understand FIFO queues & modulo arithmetic
- Use Keil IDE & simulator
- Debug using breakpoints & watch variables

✅ **Phase 0.2 (Linked List):**
- Master pointers & dynamic memory
- Understand malloc/free & memory leaks
- Trace complex data structures

✅ **Phase 0.3 (Bit Utils):**
- Fluent with bitwise operations
- Ready for register-level programming

✅ **Phase 0.4 (GPIO Simulation):**
- Understand GPIO registers (CRL, CRH, ODR, IDR)
- Ready to program real hardware!

---

## 🆘 Troubleshooting

**Keil won't open?**
- Check installation, may need license
- Try restarting computer

**Build errors?**
- Check include paths in Project Options
- Verify file paths match exactly

**Debugger shows no output?**
- Open View → Serial Windows → UART #1
- Make sure printf() is actually called

**Variables not updating in watch?**
- Optimization too high; set to `-O0`
- Code might be optimized away

See specific project README for more help.

---

## 🎯 Next Steps

**Right now:** Open [`PHASE0_CIRCULAR_BUFFER_GUIDE.md`](PHASE0_CIRCULAR_BUFFER_GUIDE.md) and start Phase 0.

**Goal:** Complete all Phase 0 projects in 4-6 weeks, then **confidently buy an STM32F103 "Blue Pill"** (~$2-4).

**Timeline:** By week 18, you'll be ready for real embedded product roles.

**Remember:** There are shortcuts to interviews, but no shortcuts to competence. This path builds *real* skills.

---

## 📚 Additional Resources

- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f103xx-arm-cortex-m3-32-bit-microcontroller-reference-manual-stmicroelectronics.pdf)
- [Keil MDK Documentation](https://www.keil.com/support/man/docs/uv4/index.html)
- [ARM Cortex-M3 Devices Generic User Guide](https://developer.arm.com/documentation/dui0552/latest/)

---

## 📜 License

This is a learning repository. Use freely for educational purposes.

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
