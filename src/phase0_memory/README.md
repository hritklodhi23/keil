# Phase 0: C Memory & Debugging Projects

This directory contains **Phase 0 learning projects** — foundational C skills you need before buying hardware.

## Projects in This Phase

### 1. **Circular Buffer** ✅ Ready
- **File**: `circular_buffer.h`, `circular_buffer.c`, `test_circular_buffer.c`
- **Goal**: Master FIFO queues, pointers, memory management
- **Setup**: See [PHASE0_CIRCULAR_BUFFER_GUIDE.md](../../PHASE0_CIRCULAR_BUFFER_GUIDE.md)
- **Difficulty**: Easy
- **Time**: 1-2 hours (including setup learning curve)

### 2. **Linked List** ⏳ Ready in phase0_memory/
- **File**: `linked_list.h`, `linked_list.c`
- **Goal**: Understand pointer-to-pointer, dynamic allocation, list traversal
- **Setup**: Similar to circular buffer; add to new Keil project
- **Difficulty**: Medium
- **Time**: 2-3 hours

### 3. **Bit Manipulation Library** ⏳ Ready in phase0_memory/
- **File**: `bit_utils.h`, `bit_utils.c`
- **Goal**: Master bitwise operations before register programming
- **Setup**: Add to Keil project with test program
- **Difficulty**: Easy-Medium
- **Time**: 1-2 hours

---

## How to Start

### ✅ Step 1: Follow the Circular Buffer Guide
→ Open [PHASE0_CIRCULAR_BUFFER_GUIDE.md](../../PHASE0_CIRCULAR_BUFFER_GUIDE.md)

This teaches you:
- How to open Keil MDK
- How to create a project from scratch
- How to build and run code
- How to use the simulator and debugger

**Estimated time**: 2-3 hours (mostly learning Keil)

### Step 2: Create Linked List Project
Once circular buffer works, create a new Keil project for linked list:

```
Keil Project: Phase0_LinkedList.uvprojx
├── linked_list.h
├── linked_list.c
└── test_linked_list.c (you write this)
```

### Step 3: Create Bit Manipulation Project
Similar process for bit utilities.

---

## What You'll Learn

| Concept | Project | Key File |
|---------|---------|----------|
| FIFO queues, circular indexing | Circular Buffer | `circular_buffer.c` |
| Pointers, pointer-to-pointer | Linked List | `linked_list.c` |
| Malloc/free, memory leaks | Linked List | `linked_list.c` |
| Bitwise operators, masks | Bit Utils | `bit_utils.c` |
| Volatile, atomic access | All | See comments |
| Debugger usage | All | See guide |

---

## Debugging Tips for Each Project

### Circular Buffer
- **Watch**: `cbuf.head`, `cbuf.tail`, `cbuf.count`
- **Breakpoint**: Set at `cbuf_write()` and `cbuf_read()` to trace execution
- **Memory**: View `cbuf_memory[0..15]` to see actual data

### Linked List
- **Watch**: `list->head`, `list->count`
- **Memory**: View pointer values to understand linked structure
- **Breakpoint**: At node allocation/deletion to catch memory bugs

### Bit Utils
- **Watch**: Result of `bit_extract()`, `bit_insert()`
- **Disassembly**: View ARM instructions (should be single bit operations)
- **Binary display**: In watch window, right-click → hex/binary format

---

## Common Mistakes & Fixes

| Problem | Cause | Fix |
|---------|-------|-----|
| Buffer not wrapping correctly | Wrong modulo calculation | Check `(index + 1) % size` |
| Linked list leaking memory | Forgot to free nodes | Call `free()` in remove/clear |
| Bitfield extraction wrong | Off-by-one in mask | Verify with manual binary math |
| Debugger shows old values | Optimization enabled | Set `-O0` in project options |
| Include path error | File not found | Use absolute paths in project |

---

## Confidence Milestones

✅ **After Circular Buffer:**
- Can use Keil IDE
- Understand circular indexing
- Can debug using breakpoints
- Know why modulo arithmetic matters

✅ **After Linked List:**
- Comfortable with pointers
- Understand malloc/free
- Can trace memory bugs
- Know pointer-to-pointer idiom

✅ **After Bit Utils:**
- Fluent with bitwise operators
- Ready for register programming
- Understand bit masks and shifts

---

## Next: Phase 0.5 — Register-Level Understanding

Once these three projects work, move to:
→ See `../phase0_registers/` for GPIO simulation projects

These introduce **register-level programming** (the heart of embedded systems) **without needing hardware**.

---

## Handy Keil Shortcuts

| Shortcut | Action |
|----------|--------|
| **F7** | Build |
| **F8** | Flash (needs hardware) |
| **Ctrl+F5** | Start debugging |
| **F5** | Run (in debugger) |
| **F10** | Step over |
| **F11** | Step into |
| **F9** | Toggle breakpoint |
| **Shift+F5** | Stop debugging |
| **Ctrl+I** | Insert breakpoint at mouse |

---

## Questions?

- **Keil won't open?** → Check installation, may need license
- **Build errors?** → Check include paths in Project Options
- **Debugger not showing output?** → Open View → Serial Windows → UART #1
- **Variables not updating?** → Optimization may be too high; set to `-O0`

Start with the **Circular Buffer guide** now! 🚀
