/**
 * @file circular_buffer.c
 * @brief Circular buffer implementation
 */

#include "circular_buffer.h"
#include <string.h>

/* Helper: check if value is power of 2 */
static bool is_power_of_2(uint16_t value) {
    return (value > 0) && ((value & (value - 1)) == 0);
}

bool cbuf_init(circular_buffer_t *cb, uint8_t *buf, uint16_t buf_size) {
    if (!cb || !buf || !is_power_of_2(buf_size)) {
        return false;
    }
    
    cb->buffer = buf;
    cb->head = 0;
    cb->tail = 0;
    cb->count = 0;
    cb->size = buf_size;
    
    return true;
}

bool cbuf_write(circular_buffer_t *cb, uint8_t byte) {
    if (cbuf_is_full(cb)) {
        return false;
    }
    
    cb->buffer[cb->head] = byte;
    cb->head = (cb->head + 1) % cb->size;
    cb->count++;
    
    return true;
}

uint16_t cbuf_write_multi(circular_buffer_t *cb, const uint8_t *data, uint16_t len) {
    uint16_t written = 0;
    
    if (!data) {
        return 0;
    }
    
    for (uint16_t i = 0; i < len && !cbuf_is_full(cb); i++) {
        if (cbuf_write(cb, data[i])) {
            written++;
        }
    }
    
    return written;
}

bool cbuf_read(circular_buffer_t *cb, uint8_t *byte) {
    if (cbuf_is_empty(cb) || !byte) {
        return false;
    }
    
    *byte = cb->buffer[cb->tail];
    cb->tail = (cb->tail + 1) % cb->size;
    cb->count--;
    
    return true;
}

uint16_t cbuf_read_multi(circular_buffer_t *cb, uint8_t *data, uint16_t len) {
    uint16_t read = 0;
    
    if (!data) {
        return 0;
    }
    
    for (uint16_t i = 0; i < len && !cbuf_is_empty(cb); i++) {
        if (cbuf_read(cb, &data[i])) {
            read++;
        }
    }
    
    return read;
}

bool cbuf_peek(const circular_buffer_t *cb, uint16_t offset, uint8_t *byte) {
    if (offset >= cb->count || !byte) {
        return false;
    }
    
    uint16_t index = (cb->tail + offset) % cb->size;
    *byte = cb->buffer[index];
    
    return true;
}

void cbuf_clear(circular_buffer_t *cb) {
    if (cb) {
        cb->head = 0;
        cb->tail = 0;
        cb->count = 0;
    }
}
