# Phase 0: Circular Buffer in Keil — Step-by-Step Setup Guide

This guide assumes **zero Keil experience**. Follow each step exactly.

---

## **PART 1: Understanding What We're Doing**

### What is Keil?
- **Keil MDK µVision** is an IDE (like Visual Studio or XCode) for embedded systems
- It has a **compiler** (turns C code into machine code)
- It has a **debugger** (lets you step through code, watch variables, etc.)
- It has a **simulator** (pretends to be real hardware so you can test without buying a microcontroller)

### What is the Circular Buffer?
- A **FIFO queue** (First-In, First-Out)
- Data comes in at the "head", goes out at the "tail"
- Useful for: UART buffers, data pipelines, real-time tasks
- Learning goal: **master pointers, memory, and C fundamentals**

---

## **PART 2: Create a New Keil Project from Scratch**

### Step 1: Launch Keil µVision
1. Open **Keil µVision 5** (icon usually on desktop or Start menu)
2. You'll see a splash screen → click anywhere or wait for it to load
3. The main window appears with a blank project area

### Step 2: Create a New Project
1. Go to **File → New Project**
2. Choose a location: **`/home/runner/work/keil/keil/project/Phase0_CircularBuffer/`**
   - (Create this folder if it doesn't exist)
3. Name it: **`Phase0_CircularBuffer.uvprojx`**
4. Click **Save**

### Step 3: Select Target Device
A dialog appears asking "Select Device for Target 'Target 1'"

1. In the search box, type: **`STM32F103C8`**
2. Find and click: **STM32F103C8 (ARM Cortex-M3)**
3. Click **OK**

This sets up:
- **Compiler**: ARM Compiler 6 (LLVM-based)
- **Device**: STM32F103 ("Blue Pill") — same as the Mini RTOS project
- **Memory**: 64 KB Flash, 20 KB RAM

### Step 4: Setup Management Wizard (Optional but Helpful)
After clicking OK, you might see "Manage Project Items" dialog:
- Click **Finish** (we'll add files manually)

---

## **PART 3: Create Project Folder Structure**

In Keil's **Project** panel (left side):

1. Right-click **Target 1** → **Manage Project Items**
2. Or simply manually add files (see next steps)

For now, let's create folders in the actual file system:

```
Phase0_CircularBuffer/
├── Listings/        (compiler output)
├── Objects/         (intermediate files)
├── src/
│   ├── circular_buffer.h
│   ├── circular_buffer.c
│   ├── test_circular_buffer.c  (main program with tests)
│   └── startup_stm32f10x.s     (will add this)
├── Phase0_CircularBuffer.uvprojx
└── Phase0_CircularBuffer.uvoptx
```

---

## **PART 4: Add Source Files to Keil Project**

### Step 4a: Copy Circular Buffer Files
The files are already created in the repo at:
- `/home/runner/work/keil/keil/src/phase0_memory/circular_buffer.h`
- `/home/runner/work/keil/keil/src/phase0_memory/circular_buffer.c`

**In Keil:**

1. In the **Project** panel (left), right-click **Target 1**
2. Select **Add Group** → name it **"Source"** → **OK**
3. Right-click **Source** → **Add Existing File to Group 'Source'**
4. Navigate to `/home/runner/work/keil/keil/src/phase0_memory/`
5. Select **`circular_buffer.c`** → **Open**
6. Repeat for **`circular_buffer.h`** (or just add .c files — headers are included automatically)

---

## **PART 5: Create a Test Program (main.c)**

We need a **main.c** that tests the circular buffer.

**In Keil:**

1. Right-click **Source** group → **Add New Item to Group 'Source'**
2. Select **C File (.c)**
3. Name it: **`test_circular_buffer.c`**
4. Click **OK** → Opens in editor

**Copy this code into test_circular_buffer.c:**

```c
/**
 * @file test_circular_buffer.c
 * @brief Circular buffer test program
 * 
 * This is the main() program.
 * It tests circular buffer operations and is debuggable in Keil simulator.
 */

#include <stdio.h>
#include <stdint.h>
#include "circular_buffer.h"

/* Buffer storage — declared globally so it's in RAM, not stack */
uint8_t cbuf_memory[256];

/**
 * Test 1: Basic write and read
 */
void test_write_read(void) {
    printf("\n=== Test 1: Basic Write/Read ===\n");
    
    circular_buffer_t cbuf;
    cbuf_init(&cbuf, cbuf_memory, 256);
    
    /* Write some bytes */
    printf("Writing: A, B, C, D\n");
    cbuf_write(&cbuf, 'A');
    cbuf_write(&cbuf, 'B');
    cbuf_write(&cbuf, 'C');
    cbuf_write(&cbuf, 'D');
    
    printf("Buffer count: %d (expected 4)\n", cbuf_count(&cbuf));
    printf("Buffer available: %d (expected 252)\n", cbuf_available(&cbuf));
    
    /* Read them back */
    uint8_t byte;
    printf("Reading: ");
    while (!cbuf_is_empty(&cbuf)) {
        cbuf_read(&cbuf, &byte);
        printf("%c ", byte);
    }
    printf("\n");
    
    printf("Buffer count: %d (expected 0)\n", cbuf_count(&cbuf));
    printf("✓ Test 1 passed\n");
}

/**
 * Test 2: Overflow behavior
 */
void test_overflow(void) {
    printf("\n=== Test 2: Overflow ===\n");
    
    circular_buffer_t cbuf;
    cbuf_init(&cbuf, cbuf_memory, 4);  /* Small buffer */
    
    printf("Buffer size: 4 bytes\n");
    
    /* Try to write 6 bytes */
    printf("Writing 6 bytes: ");
    for (uint8_t i = 0; i < 6; i++) {
        uint8_t byte = 'A' + i;
        bool success = cbuf_write(&cbuf, byte);
        printf("%c(%s) ", byte, success ? "OK" : "FULL");
    }
    printf("\n");
    
    printf("Buffer count: %d (expected 4)\n", cbuf_count(&cbuf));
    printf("Buffer available: %d (expected 0)\n", cbuf_available(&cbuf));
    printf("✓ Test 2 passed\n");
}

/**
 * Test 3: Wrap-around (circular behavior)
 */
void test_wraparound(void) {
    printf("\n=== Test 3: Wrap-Around ===\n");
    
    circular_buffer_t cbuf;
    cbuf_init(&cbuf, cbuf_memory, 8);  /* 8-byte buffer */
    
    /* Write 5, read 3, write 5 more — tests wrap-around */
    printf("Writing: 1, 2, 3, 4, 5\n");
    cbuf_write(&cbuf, 1);
    cbuf_write(&cbuf, 2);
    cbuf_write(&cbuf, 3);
    cbuf_write(&cbuf, 4);
    cbuf_write(&cbuf, 5);
    
    uint8_t byte;
    printf("Reading: ");
    cbuf_read(&cbuf, &byte); printf("%d ", byte);
    cbuf_read(&cbuf, &byte); printf("%d ", byte);
    cbuf_read(&cbuf, &byte); printf("%d ", byte);
    printf("(read 3)\n");
    
    printf("Count after read: %d\n", cbuf_count(&cbuf));
    
    printf("Writing: 6, 7, 8, 9, 10\n");
    cbuf_write(&cbuf, 6);
    cbuf_write(&cbuf, 7);
    cbuf_write(&cbuf, 8);
    cbuf_write(&cbuf, 9);
    cbuf_write(&cbuf, 10);
    
    printf("Count: %d (expected 7)\n", cbuf_count(&cbuf));
    
    printf("Reading all: ");
    while (!cbuf_is_empty(&cbuf)) {
        cbuf_read(&cbuf, &byte);
        printf("%d ", byte);
    }
    printf("\n");
    printf("✓ Test 3 passed\n");
}

/**
 * Test 4: Peek without removing
 */
void test_peek(void) {
    printf("\n=== Test 4: Peek ===\n");
    
    circular_buffer_t cbuf;
    cbuf_init(&cbuf, cbuf_memory, 10);
    
    printf("Writing: X, Y, Z\n");
    cbuf_write(&cbuf, 'X');
    cbuf_write(&cbuf, 'Y');
    cbuf_write(&cbuf, 'Z');
    
    uint8_t byte;
    printf("Peeking at offset 0: ");
    cbuf_peek(&cbuf, 0, &byte);
    printf("%c\n", byte);
    
    printf("Peeking at offset 1: ");
    cbuf_peek(&cbuf, 1, &byte);
    printf("%c\n", byte);
    
    printf("Peeking at offset 2: ");
    cbuf_peek(&cbuf, 2, &byte);
    printf("%c\n", byte);
    
    printf("Count (should still be 3): %d\n", cbuf_count(&cbuf));
    printf("✓ Test 4 passed\n");
}

/**
 * Main entry point
 */
int main(void) {
    printf("=================================\n");
    printf("Phase 0: Circular Buffer Tests\n");
    printf("=================================\n");
    
    test_write_read();
    test_overflow();
    test_wraparound();
    test_peek();
    
    printf("\n=================================\n");
    printf("All tests completed!\n");
    printf("=================================\n");
    
    /* Infinite loop (in real embedded systems, main never returns) */
    while (1) {
        /* Do nothing */
    }
    
    return 0;
}
```

---

## **PART 6: Configure Build Settings**

### Step 1: Set Output Folder
1. In Keil, go to **Project → Options for Target 'Target 1'**
2. Go to **Output** tab
3. Check **Create EXE** (if not already checked)
4. In **Name of Executable**, set it to: **Phase0_CircularBuffer.axf**
5. Click **OK**

### Step 2: Verify C/C++ Settings
1. Go to **Project → Options for Target 'Target 1'** again
2. **C/C++** tab
3. Make sure **Optimization Level** is set to **-O0** (no optimization, easier to debug)
4. **Include Paths**: Should include the folder where `circular_buffer.h` is
   - Add path: `/home/runner/work/keil/keil/src/phase0_memory/`
5. Click **OK**

---

## **PART 7: Build the Project**

1. In Keil, press **F7** (or go to **Project → Build Target**)
2. Watch the **Build Output** panel at the bottom
3. You should see:
   ```
   Compiling test_circular_buffer.c...
   Compiling circular_buffer.c...
   Linking...
   Building: Phase0_CircularBuffer
   
   Build Complete
   ```

If you see **errors**, check:
- Files are in the correct path
- Include paths are set correctly
- File names match exactly

---

## **PART 8: Debug in Simulator**

### Step 1: Select Simulator
1. Go to **Project → Options for Target 'Target 1'**
2. Go to **Debug** tab
3. Under **Use**, select **Simulator** (not ST-Link)
4. Click **OK**

### Step 2: Start Debugging
1. Press **Ctrl+F5** (or **Debug → Start/Stop Debug Session**)
2. The debugger starts and stops at the first line of `main()`
3. You'll see:
   - **Code** panel showing your C code
   - **Registers** panel (bottom right) showing CPU registers
   - **Watch** panel (bottom left) where you can add variables to monitor
   - **Output/Debug** panel showing printf() output

### Step 3: Step Through Code
- **F10**: Step over (execute current line, don't go into functions)
- **F11**: Step into (go inside function calls)
- **Shift+F11**: Step out (execute until current function returns)
- **F5** or **Debug → Run**: Run until breakpoint

### Step 4: Add a Breakpoint
1. Click on a line in the code editor
2. Press **F9** to set a breakpoint (red dot appears)
3. Click **Debug → Run** — program stops at breakpoint
4. Examine variables in the **Watch** window

### Step 5: Watch Circular Buffer Variables
1. In the **Watch** window, click **Insert Watch**
2. Type variable name, e.g.: **`cbuf.head`**
3. You'll see it update as you step
4. Add: `cbuf.tail`, `cbuf.count`, `cbuf_memory[0]`, etc.

### Step 6: View Output
1. Go to **View → Serial Windows → UART #1** (or printf window)
2. As you step, printf() output appears here
3. This shows test results!

---

## **PART 9: Understanding the Debugger**

### Common Things to Do:

**To see variable value:**
- Hover over variable in code → tooltip shows value
- Or add to **Watch** window

**To see memory contents:**
- Go to **View → Memory** → type address (e.g., `&cbuf_memory[0]`)
- See exact bytes stored in RAM

**To see call stack:**
- Go to **View → Call Stack**
- Shows which functions called which functions

**To see disassembly:**
- Go to **View → Disassembly**
- Shows machine code (ARM Cortex-M3 instructions)
- Advanced but useful for understanding how C becomes assembly!

---

## **PART 10: Run Full Tests**

1. Press **Ctrl+F5** to start debugging
2. Open the **Output/Debug** window
3. Click **Debug → Run** (or press F5)
4. Watch test output in Debug window
5. Program runs until it hits the `while(1)` loop
6. Press **Debug → Stop** (or Shift+F5) to stop

You should see:
```
=================================
Phase 0: Circular Buffer Tests
=================================

=== Test 1: Basic Write/Read ===
Writing: A, B, C, D
Buffer count: 4 (expected 4)
Buffer available: 252 (expected 252)
Reading: A B C D
Buffer count: 0 (expected 0)
✓ Test 1 passed

=== Test 2: Overflow ===
...
```

---

## **TROUBLESHOOTING**

### Problem: "Undefined symbol" error
- **Solution**: Make sure circular_buffer.h is included in test file
- Check include path in Project → Options → C/C++

### Problem: "Cannot find file"
- **Solution**: Check file path in "Add Existing File"
- Use full path: `/home/runner/work/keil/keil/src/phase0_memory/circular_buffer.c`

### Problem: Debugger shows "No source file"
- **Solution**: The path to source doesn't match
- Go to **Debug → Source Files** → add correct path

### Problem: Output window shows nothing
- **Solution**: Make sure you're in the correct tab
- Try **View → Serial Windows → UART #1**
- Or check that printf() is working (add breakpoint right after first printf)

---

## **Next Steps After This Works**

Once you see all tests pass:

1. **Modify tests** — add your own test cases
2. **Add edge cases** — what if buffer size is 1? What if we write/read exact capacity?
3. **Try to break it** — write code that causes bugs (then debug them!)
4. **Add watchpoints** — break when a variable changes value (not just when code reaches a line)

Then move to **Linked List** (Phase 0.2) and **Bit Manipulation** (Phase 0.3).

---

## **Key Learning Points**

By the end of this Phase 0 project, you should understand:

✅ How to use Keil IDE
✅ How to build C code for embedded systems
✅ How to use the simulator and debugger
✅ How circular buffers work internally
✅ How to step through code and inspect memory
✅ How to find and fix bugs

**This is fundamental before buying hardware.**
