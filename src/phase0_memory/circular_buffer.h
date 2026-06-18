/**
 * @file circular_buffer.h
 * @brief Circular buffer implementation — Phase 0 learning project
 * 
 * Demonstrates:
 * - Memory management (stack vs heap)
 * - Pointer arithmetic
 * - Modulo for circular indexing
 * - volatile for safe concurrent access
 * 
 * Learning goal: Master C pointers and memory patterns before hardware
 */

#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Circular buffer structure
 * - Uses fixed-size buffer (static allocation)
 * - Head: write pointer
 * - Tail: read pointer
 * - Count: number of valid items
 * - Size: capacity (power of 2 for efficiency)
 */
typedef struct {
    uint8_t *buffer;
    volatile uint16_t head;      /* Next write position */
    volatile uint16_t tail;      /* Next read position */
    volatile uint16_t count;     /* Items in buffer */
    uint16_t size;               /* Total capacity */
} circular_buffer_t;

/**
 * Initialize circular buffer with pre-allocated memory
 * @param cb pointer to circular_buffer_t structure
 * @param buf pre-allocated buffer (e.g., uint8_t buf[256])
 * @param buf_size buffer capacity in bytes
 * @return true if successful, false if size not power-of-2
 */
bool cbuf_init(circular_buffer_t *cb, uint8_t *buf, uint16_t buf_size);

/**
 * Write single byte to buffer
 * @param cb pointer to circular_buffer_t structure
 * @param byte data to write
 * @return true if successful, false if buffer full
 */
bool cbuf_write(circular_buffer_t *cb, uint8_t byte);

/**
 * Write multiple bytes to buffer
 * @param cb pointer to circular_buffer_t structure
 * @param data pointer to data array
 * @param len number of bytes to write
 * @return number of bytes actually written (may be less than len if full)
 */
uint16_t cbuf_write_multi(circular_buffer_t *cb, const uint8_t *data, uint16_t len);

/**
 * Read single byte from buffer
 * @param cb pointer to circular_buffer_t structure
 * @param byte pointer to store read data
 * @return true if successful, false if buffer empty
 */
bool cbuf_read(circular_buffer_t *cb, uint8_t *byte);

/**
 * Read multiple bytes from buffer
 * @param cb pointer to circular_buffer_t structure
 * @param data pointer to output buffer
 * @param len number of bytes requested
 * @return number of bytes actually read (may be less than len if not enough data)
 */
uint16_t cbuf_read_multi(circular_buffer_t *cb, uint8_t *data, uint16_t len);

/**
 * Peek at data without removing it
 * @param cb pointer to circular_buffer_t structure
 * @param offset offset from tail (0 = next byte to read)
 * @param byte pointer to store peeked data
 * @return true if offset is valid, false otherwise
 */
bool cbuf_peek(const circular_buffer_t *cb, uint16_t offset, uint8_t *byte);

/**
 * Get number of items in buffer
 */
static inline uint16_t cbuf_count(const circular_buffer_t *cb) {
    return cb->count;
}

/**
 * Get available space in buffer
 */
static inline uint16_t cbuf_available(const circular_buffer_t *cb) {
    return cb->size - cb->count;
}

/**
 * Check if buffer is empty
 */
static inline bool cbuf_is_empty(const circular_buffer_t *cb) {
    return cb->count == 0;
}

/**
 * Check if buffer is full
 */
static inline bool cbuf_is_full(const circular_buffer_t *cb) {
    return cb->count == cb->size;
}

/**
 * Clear buffer (reset head, tail, count)
 */
void cbuf_clear(circular_buffer_t *cb);

#endif /* CIRCULAR_BUFFER_H */
