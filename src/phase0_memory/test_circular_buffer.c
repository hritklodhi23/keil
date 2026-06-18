/**
 * @file test_circular_buffer.c
 * @brief Circular buffer test program
 * 
 * This is the main() program.
 * It tests circular buffer operations and is debuggable in Keil simulator.
 * 
 * How to use:
 * 1. Add this file to your Keil project
 * 2. Build (F7)
 * 3. Debug (Ctrl+F5)
 * 4. Run (F5)
 * 5. Watch output in Debug window
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
 * Test 5: Multi-byte operations
 */
void test_multi_operations(void) {
    printf("\n=== Test 5: Multi-Byte Operations ===\n");
    
    circular_buffer_t cbuf;
    cbuf_init(&cbuf, cbuf_memory, 32);
    
    uint8_t data_out[10] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    uint8_t data_in[10] = {0};
    
    printf("Writing 10 bytes: 10, 20, 30...\n");
    uint16_t written = cbuf_write_multi(&cbuf, data_out, 10);
    printf("Bytes written: %d\n", written);
    printf("Buffer count: %d\n", cbuf_count(&cbuf));
    
    printf("Reading 10 bytes: ");
    uint16_t read = cbuf_read_multi(&cbuf, data_in, 10);
    printf("(got %d) ", read);
    for (int i = 0; i < read; i++) {
        printf("%d ", data_in[i]);
    }
    printf("\n");
    printf("✓ Test 5 passed\n");
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
    test_multi_operations();
    
    printf("\n=================================\n");
    printf("All tests completed!\n");
    printf("=================================\n");
    
    /* Infinite loop (in real embedded systems, main never returns) */
    while (1) {
        /* Do nothing */
    }
    
    return 0;
}
