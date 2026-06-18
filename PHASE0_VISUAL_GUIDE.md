# 🚀 Getting Started with Phase 0 — Visual Guide

**If you're completely new to Keil**, follow this visual guide before reading the detailed guide.

---

## What You're About to Do

```
   You (zero Keil experience)
         ↓
   Open Keil MDK software
         ↓
   Create a new project
         ↓
   Add C source files (circular buffer code)
         ↓
   Press F7 to BUILD
         ↓
   Press Ctrl+F5 to DEBUG
         ↓
   Watch test output & debug with breakpoints
         ↓
   ✨ Understand how Keil & embedded systems work!
```

---

## 📱 Keil MDK Interface Overview

When you launch Keil, you'll see:

```
┌─────────────────────────────────────────────────────────────┐
│  File Edit View Project Debug Tools Window Help             │  ← Menu Bar
├──────────────┬──────────────────────────────────────────────┤
│              │                                              │
│  Project     │          Code Editor                         │
│  Panel       │          (your C code here)                 │
│              │                                              │
│  • Target1   │      int main() {                           │
│    ├─Source  │          printf("Hello\n");                │
│    └─Header  │          while(1) { }                       │
│              │      }                                       │
│              │                                              │
│              ├──────────────────────────────────────────────┤
│              │  Build Output / Debug Output  (choose tabs)  │
│              │  [Compile messages appear here]              │
│              │  [Program output appears here]               │
└──────────────┴──────────────────────────────────────────────┘
```

### Key Areas:

1. **Project Panel** (left) — shows files & folder structure
2. **Code Editor** (center) — where you write C code
3. **Output Panel** (bottom) — shows build messages, debug output
4. **Toolbar** (top) — has F7 (Build), F8 (Flash), Ctrl+F5 (Debug)

---

## 🎯 Step-by-Step: Your First Circular Buffer Project

### Step 1: Launch Keil
```
Windows:  Click Keil icon or search "Keil MDK"
Mac/Linux: Use Wine or Docker (Keil is Windows-only)
```

You'll see the splash screen for a few seconds, then the main window.

### Step 2: File → New Project
```
Menu: File
  ↓
New Project
  ↓
Choose location: /home/runner/work/keil/keil/project/Phase0_CircularBuffer/
Name: Phase0_CircularBuffer.uvprojx
  ↓
Click SAVE
```

### Step 3: Select Device
A dialog appears asking what microcontroller you're using.

```
Search box: Type "STM32F103C8"
  ↓
Find: STM32F103C8 (ARM Cortex-M3)
  ↓
Click OK
```

Now Keil knows:
- ✅ Compiler: ARM Compiler 6
- ✅ Chip: STM32F103 
- ✅ Memory: 64 KB Flash, 20 KB RAM

### Step 4: Add Files to Project

In the **Project Panel** (left), you'll see "Target 1". Right-click it:

```
Right-click: Target 1
  ↓
"Add Group" → name it "Source"
  ↓
Right-click "Source" → "Add Existing File to Group"
  ↓
Navigate to: /home/runner/work/keil/keil/src/phase0_memory/
  ↓
Select: circular_buffer.c
  ↓
Open
```

Repeat to add:
- `circular_buffer.c`
- `circular_buffer.h` (optional — headers auto-included)
- `test_circular_buffer.c`

### Step 5: Create Include Path

```
Menu: Project
  ↓
Options for Target 'Target 1'
  ↓
Tab: C/C++
  ↓
Include Paths: (find empty text field)
  ↓
Type: /home/runner/work/keil/keil/src/phase0_memory/
  ↓
Click OK
```

This tells the compiler where to find `circular_buffer.h`.

### Step 6: Disable Optimization (for debugging)

```
Menu: Project
  ↓
Options for Target 'Target 1'
  ↓
Tab: C/C++
  ↓
Optimization: Change from O1 or O2 to O0 (no optimization)
  ↓
Click OK
```

**Why?** Optimization can hide variables from the debugger. We want to see everything.

### Step 7: Select Simulator

```
Menu: Project
  ↓
Options for Target 'Target 1'
  ↓
Tab: Debug
  ↓
Under "Use:", select: Simulator
  ↓
Click OK
```

This tells Keil to simulate the microcontroller instead of flashing real hardware.

### Step 8: Build (F7)

```
Press: F7
```

Watch the Output panel at the bottom:

```
Compiling circular_buffer.c...
Compiling test_circular_buffer.c...
Linking...
Building: Phase0_CircularBuffer.axf

Build Complete!
```

If you see **errors**, check:
- File paths are correct
- Include paths include `/home/runner/work/keil/keil/src/phase0_memory/`
- Files names match exactly

### Step 9: Start Debugging (Ctrl+F5)

```
Press: Ctrl+F5
```

The debugger starts and you'll see:
- Code stops at first line of `main()`
- Red dot on that line = you're stopped here
- Watch window opens (shows variables)

### Step 10: Run the Program (F5)

```
Press: F5
```

The program runs. Open the output window to see test results:

```
Menu: View
  ↓
Serial Windows
  ↓
UART #1
```

You'll see:
```
=================================
Phase 0: Circular Buffer Tests
=================================

=== Test 1: Basic Write/Read ===
Writing: A, B, C, D
Buffer count: 4 (expected 4)
...
✓ Test 1 passed
```

### Step 11: Set a Breakpoint and Step

```
Click on a line in test_circular_buffer.c
  ↓
Press: F9 (sets red dot = breakpoint)
  ↓
Press: Ctrl+F5 to restart debugging
  ↓
Program stops at your breakpoint
  ↓
Press: F10 to step (execute one line)
  ↓
Press: F11 to step-into (go inside function)
```

While stopped, hover over variables to see their values!

### Step 12: Add to Watch Window

```
In Watch window (bottom-left), click "Insert Watch"
  ↓
Type: cbuf.head
  ↓
Press Enter
```

Now you'll see `cbuf.head` update as you step through code!

---

## 📊 Common Workflows

### Workflow 1: Run Full Test
```
Build (F7)
  ↓
Debug (Ctrl+F5)
  ↓
Run (F5)
  ↓
Watch output panel
  ↓
Stop (Shift+F5)
```

### Workflow 2: Debug One Test Function
```
Click line in test_write_read() function
  ↓
Press F9 (set breakpoint)
  ↓
Debug (Ctrl+F5)
  ↓
Run (F5) — stops at breakpoint
  ↓
Step through (F10, F11)
  ↓
Watch variables change
  ↓
Stop (Shift+F5)
```

### Workflow 3: Find a Bug
```
Code behaves wrong (test fails)
  ↓
Set breakpoint near the problem
  ↓
Debug → Run
  ↓
Step through line-by-line
  ↓
Watch variables in Watch window
  ↓
Spot the bug!
  ↓
Fix code, rebuild (F7), debug again
```

---

## 🎨 Visual: What's in Memory?

When debugging, you can see actual memory contents:

```
Menu: View → Memory
  ↓
Type address: &cbuf_memory[0]
  ↓
You see:
  
Address  Hex Values
0x...    41 42 43 44  (ASCII: A B C D)
0x...    00 00 00 00  (empty bytes)
```

This shows you **exactly** what data is stored!

---

## 🚨 Troubleshooting Quick Reference

| Problem | Solution |
|---------|----------|
| "Undefined symbol: cbuf_write" | Add include path (Project → Options → C/C++) |
| Build errors with "cannot find circular_buffer.h" | Check file path, use absolute path |
| Debugger won't start | Select Simulator (Project → Options → Debug tab) |
| printf output not showing | Open View → Serial Windows → UART #1 |
| Watch shows old value | Restart debug session (Ctrl+F5) |
| Can't see variables in debugger | Disable optimization (set to -O0) |

---

## 📝 Reference: Key Keyboard Shortcuts

```
F7              Build project
F8              Flash to hardware
Ctrl+F5         Start/Restart debugging
F5              Run (continue execution)
Shift+F5        Stop debugging
F10             Step over (don't enter functions)
F11             Step into (enter function call)
Shift+F11       Step out (finish current function)
F9              Toggle breakpoint
Ctrl+G          Go to line
Ctrl+F          Find in file
```

---

## ✅ Success Criteria

When you're done with Phase 0.1, you should be able to:

- [ ] Create a Keil project without help
- [ ] Build C code without errors
- [ ] Start the debugger
- [ ] Set and hit breakpoints
- [ ] Step through code
- [ ] Watch variable values change
- [ ] Read program output
- [ ] Understand what the code is doing by reading, not guessing

If all checkmarks are true, **move to Phase 0.2 (Linked List)**.

---

## 🎓 What Happens Next?

Once Phase 0.1 works:

1. **Phase 0.2**: Create a Linked List project (same process, more complex code)
2. **Phase 0.3**: Create a Bit Manipulation project
3. **Phase 0.4**: Create a GPIO Simulation project (register-level programming!)

Then you'll understand:
- ✅ How embedded systems work
- ✅ How to debug firmware
- ✅ How to program registers
- ✅ **Ready to buy STM32 hardware!**

---

## 🆘 Need Help?

- **Stuck in Keil?** → Read [PHASE0_CIRCULAR_BUFFER_GUIDE.md](PHASE0_CIRCULAR_BUFFER_GUIDE.md) (detailed)
- **Want to understand circles buffers?** → Comments in `circular_buffer.h`
- **Confused about pointers?** → Google "C pointers tutorial" + spend time on it
- **Debugger confusing?** → Try stepping through one simple function 5 times (you'll get it)

---

## 🎯 You've Got This!

Right now:
1. Open Keil
2. Create project (save as `Phase0_CircularBuffer`)
3. Add files from `/home/runner/work/keil/keil/src/phase0_memory/`
4. Follow the steps above
5. See it work! 🎉

Then **move to the detailed guide** for deeper understanding.

Let's go! 🚀
