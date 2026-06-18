# Phase 0 Quick-Start Checklist

Use this checklist to track your progress through Phase 0 projects.

---

## 🎯 Phase 0.1: Circular Buffer

### Pre-Setup
- [ ] Keil MDK µVision 5 installed on your computer
- [ ] Read: [PHASE0_CIRCULAR_BUFFER_GUIDE.md](PHASE0_CIRCULAR_BUFFER_GUIDE.md) (all parts)

### Project Setup
- [ ] Create folder: `Phase0_CircularBuffer/`
- [ ] Create Keil project: `Phase0_CircularBuffer.uvprojx`
- [ ] Selected device: **STM32F103C8**
- [ ] Added files to project:
  - [ ] `circular_buffer.h`
  - [ ] `circular_buffer.c`
  - [ ] `test_circular_buffer.c`
- [ ] Set include path: `/home/runner/work/keil/keil/src/phase0_memory/`
- [ ] Set optimization: `-O0` (no optimization for debugging)
- [ ] Selected Debug tool: **Simulator** (not ST-Link)

### Build & Debug
- [ ] Built successfully (F7) — no errors
- [ ] Started debugging (Ctrl+F5) — no errors
- [ ] Ran program (F5)
- [ ] Saw test output in Debug window

### Tests Passed
- [ ] Test 1: Basic Write/Read ✓
- [ ] Test 2: Overflow ✓
- [ ] Test 3: Wrap-Around ✓
- [ ] Test 4: Peek ✓
- [ ] Test 5: Multi-Byte Operations ✓

### Debugging Skills Acquired
- [ ] Can set breakpoints (F9)
- [ ] Can step through code (F10, F11)
- [ ] Can add variables to Watch window
- [ ] Can view memory contents
- [ ] Can interpret test output
- [ ] Can read variable values while code runs

### Understanding Achieved
- [ ] What a circular buffer is
- [ ] How head/tail/count track data
- [ ] Why modulo (%) is used for wrap-around
- [ ] What "FIFO" means
- [ ] When overflow occurs
- [ ] Why peek doesn't remove data
- [ ] Keil IDE basics

---

## 🎯 Phase 0.2: Linked List

### Pre-Setup
- [ ] Phase 0.1 (Circular Buffer) complete
- [ ] Read: Linked List comments in `linked_list.h`

### Project Setup
- [ ] Create folder: `Phase0_LinkedList/`
- [ ] Create Keil project: `Phase0_LinkedList.uvprojx`
- [ ] Selected device: **STM32F103C8**
- [ ] Added files to project:
  - [ ] `linked_list.h`
  - [ ] `linked_list.c`
  - [ ] `test_linked_list.c` (write this yourself!)

### Your Test Program
- [ ] Create `test_linked_list.c` with these tests:
  - [ ] Test: Insert at head (3 items)
  - [ ] Test: Append to tail (3 items)
  - [ ] Test: Get at index (retrieve items)
  - [ ] Test: Remove at index (delete items)
  - [ ] Test: Find using predicate
  - [ ] Test: Clear list (free all memory)

### Build & Debug
- [ ] Built successfully (F7) — no errors
- [ ] Started debugging (Ctrl+F5) — no errors
- [ ] Ran all tests (F5)
- [ ] All tests printed "✓ passed"

### Debugging Skills
- [ ] Set breakpoint in `ll_insert_head()`
- [ ] Watched `list->head` pointer change
- [ ] Viewed memory to see linked structure
- [ ] Stepped through node traversal
- [ ] Found the difference between pointer and dereferenced value

### Understanding Achieved
- [ ] What a linked list is
- [ ] Pointer-to-pointer for list head manipulation
- [ ] Traversal (loop through nodes)
- [ ] Insertion and deletion patterns
- [ ] Why we need `malloc()` for new nodes
- [ ] Why we must `free()` removed nodes
- [ ] Memory leak detection (count freed == count allocated)
- [ ] O(1) vs O(n) operations

---

## 🎯 Phase 0.3: Bit Manipulation

### Pre-Setup
- [ ] Phase 0.1 complete
- [ ] Read: Bit Utils comments in `bit_utils.h`
- [ ] Know binary notation: `0b0101` = 5

### Project Setup
- [ ] Create folder: `Phase0_BitUtils/`
- [ ] Create Keil project: `Phase0_BitUtils.uvprojx`
- [ ] Selected device: **STM32F103C8**
- [ ] Added files to project:
  - [ ] `bit_utils.h`
  - [ ] `bit_utils.c`
  - [ ] `test_bit_utils.c` (write this!)

### Your Test Program
Test these operations:
- [ ] `BIT_SET()` — set single bit
- [ ] `BIT_CLEAR()` — clear single bit
- [ ] `BIT_TOGGLE()` — flip bit
- [ ] `BIT_IS_SET()` — check if set
- [ ] `BITMASK()` — create multi-bit mask
- [ ] `bit_extract()` — get bits 4-7 from value
- [ ] `bit_insert()` — write new bits to field
- [ ] `bit_popcount()` — count set bits
- [ ] `bit_find_first_set()` — find LSB position
- [ ] `bit_reverse()` — reverse bit order

### Test Ideas
```c
// Example test structure
void test_bit_set(void) {
    uint32_t value = 0x00;
    value = BIT_SET(value, 3);      // Set bit 3
    assert(value == 0x08);          // Binary: 0b1000
    printf("✓ BIT_SET test passed\n");
}
```

### Build & Debug
- [ ] Built successfully (F7)
- [ ] All tests run without crashing
- [ ] All tests print "✓ passed"

### Debugging Skills
- [ ] Set breakpoint in `bit_extract()`
- [ ] Watched mask calculation happen
- [ ] Viewed value in **binary** (right-click Watch)
- [ ] Stepped through bit rotation
- [ ] Understood shift operations in assembly

### Understanding Achieved
- [ ] Bitwise AND (&), OR (|), XOR (^), NOT (~)
- [ ] Left shift (<<) and right shift (>>)
- [ ] Single-bit vs multi-bit operations
- [ ] Bitmask creation and application
- [ ] Why bit operations matter for registers
- [ ] Population count algorithm
- [ ] Bit rotation and reversal

---

## 🎯 Phase 0.4: Register-Level Understanding

### Pre-Setup
- [ ] Phase 0.1, 0.2, 0.3 complete
- [ ] Read: GPIO Simulation comments in `gpio_simulation.h`

### Project Setup
- [ ] Create folder: `Phase0_GPIO_Simulation/`
- [ ] Create Keil project: `Phase0_GPIO_Simulation.uvprojx`
- [ ] Selected device: **STM32F103C8**
- [ ] Added files to project:
  - [ ] `gpio_simulation.h`
  - [ ] `gpio_simulation.c`
  - [ ] `test_gpio_simulation.c` (write this!)

### Your Test Program
- [ ] Test: Configure pin as output (50 MHz)
- [ ] Test: Set and read pin output state
- [ ] Test: Toggle pin multiple times
- [ ] Test: Configure pin as input
- [ ] Test: Simulate external input
- [ ] Test: Bit-bang a simple protocol (e.g., blink pattern)

### Build & Debug
- [ ] Built successfully
- [ ] All tests pass
- [ ] Can visualize GPIO state with `gpio_print_state()`

### Debugging Skills
- [ ] Watched `GPIOA.ODR` change
- [ ] Viewed `GPIOA.CRL` configuration register
- [ ] Set breakpoint in `gpio_configure_pin()`
- [ ] Understood how bit shifts apply pin config
- [ ] Viewed memory-mapped register contents

### Understanding Achieved
- [ ] How GPIO registers work (CRL, CRH, IDR, ODR, BSRR)
- [ ] Pin configuration (mode, speed, configuration)
- [ ] Volatile access for hardware
- [ ] Memory-mapped I/O concept
- [ ] Set/Reset register patterns
- [ ] Input vs output states
- [ ] Ready to program real GPIO!

---

## 🎓 Phase 0 Complete!

When all checkboxes are done:

✅ You understand C fundamentals at the hardware level
✅ You can debug code confidently
✅ You know how to use Keil MDK
✅ You're ready to understand Phase 1 (microcontroller depth)

---

## Next Steps

Once Phase 0 is complete:

1. **Review** what you learned — write notes
2. **Extend projects** — add more complex tests
3. **Break things intentionally** — introduce bugs, then debug them
4. **Move to Phase 1** — STM32F103 driver simulation and more complex projects

---

## Time Estimate

- **Circular Buffer**: 2-3 hours (first time with Keil)
- **Linked List**: 1.5-2 hours
- **Bit Utils**: 1-1.5 hours
- **GPIO Simulation**: 1.5-2 hours
- **Total**: 6-8.5 hours

**Realistic timeline**: 1-2 weeks at 1-2 hours per day

Then you'll be **confident to buy STM32** and move to Phase 1!

---

## Help!

If stuck:
1. Check the PHASE0_CIRCULAR_BUFFER_GUIDE for Keil help
2. Read the `.h` file comments for API usage
3. Look at test file for examples
4. Try stepping through with debugger
5. Print variable values to understand flow

You've got this! 🚀
