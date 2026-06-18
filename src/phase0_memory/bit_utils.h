/**
 * @file bit_utils.h
 * @brief Bit manipulation utilities — Phase 0 learning project
 * 
 * Demonstrates:
 * - Bitwise operators: &, |, ^, ~, <<, >>
 * - Bit masks and shifts
 * - Common bit patterns (flag sets, bit fields)
 * - Hardware-style register operations
 * 
 * Learning goal: Become fluent with bitwise operations before register programming
 */

#ifndef BIT_UTILS_H
#define BIT_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/* ==================== Basic Bit Operations ==================== */

/**
 * Set a single bit in a value
 * @param value the value to modify
 * @param bit_pos bit position (0-31 for uint32_t)
 * @return value with bit set
 */
#define BIT_SET(value, bit_pos)     ((value) | (1u << (bit_pos)))

/**
 * Clear a single bit in a value
 */
#define BIT_CLEAR(value, bit_pos)   ((value) & ~(1u << (bit_pos)))

/**
 * Toggle a single bit in a value
 */
#define BIT_TOGGLE(value, bit_pos)  ((value) ^ (1u << (bit_pos)))

/**
 * Check if a bit is set
 */
#define BIT_IS_SET(value, bit_pos)  (((value) >> (bit_pos)) & 1u)

/**
 * Check if a bit is clear
 */
#define BIT_IS_CLEAR(value, bit_pos) (!BIT_IS_SET((value), (bit_pos)))

/* ==================== Bit Field Operations ==================== */

/**
 * Create a mask for consecutive bits
 * @param start_bit first bit position
 * @param num_bits number of consecutive bits
 * Example: BITMASK(4, 3) = 0x70 (bits 4,5,6)
 */
#define BITMASK(start_bit, num_bits) \
    (((1u << (num_bits)) - 1u) << (start_bit))

/**
 * Extract bits from value
 * @param value the value to extract from
 * @param start_bit first bit position
 * @param num_bits number of consecutive bits to extract
 * @return extracted value (right-aligned, bit 0)
 */
static inline uint32_t bit_extract(uint32_t value, uint8_t start_bit, uint8_t num_bits) {
    return (value >> start_bit) & ((1u << num_bits) - 1u);
}

/**
 * Insert bits into value
 * @param value the value to modify
 * @param start_bit first bit position
 * @param num_bits number of consecutive bits
 * @param new_bits the new bit pattern (right-aligned)
 * @return modified value
 */
static inline uint32_t bit_insert(uint32_t value, uint8_t start_bit, 
                                   uint8_t num_bits, uint32_t new_bits) {
    uint32_t mask = ((1u << num_bits) - 1u) << start_bit;
    return (value & ~mask) | ((new_bits << start_bit) & mask);
}

/**
 * Write bits to a field (alias for bit_insert)
 */
#define BIT_WRITE(value, start, num_bits, new_bits) \
    bit_insert((value), (start), (num_bits), (new_bits))

/**
 * Read bits from a field (alias for bit_extract)
 */
#define BIT_READ(value, start, num_bits) \
    bit_extract((value), (start), (num_bits))

/* ==================== Bit Counting & Manipulation ==================== */

/**
 * Count number of set bits (1s) in value (population count)
 * Uses Brian Kernighan's algorithm: O(number of set bits)
 */
uint8_t bit_popcount(uint32_t value);

/**
 * Find position of first set bit (LSB = 0)
 * @return position of first 1 bit, or 32 if no bits set
 */
uint8_t bit_find_first_set(uint32_t value);

/**
 * Find position of last set bit (MSB)
 * @return position of last 1 bit, or 32 if no bits set
 */
uint8_t bit_find_last_set(uint32_t value);

/**
 * Check if value has only one bit set
 */
static inline bool bit_is_power_of_2(uint32_t value) {
    return (value > 0) && ((value & (value - 1)) == 0);
}

/**
 * Reverse bit order in value
 * @param num_bits number of bits to reverse (8, 16, or 32)
 */
uint32_t bit_reverse(uint32_t value, uint8_t num_bits);

/**
 * Rotate left
 * @param value the value to rotate
 * @param num_bits number of bits to rotate left
 */
static inline uint32_t bit_rotate_left(uint32_t value, uint8_t num_bits) {
    return (value << num_bits) | (value >> (32 - num_bits));
}

/**
 * Rotate right
 */
static inline uint32_t bit_rotate_right(uint32_t value, uint8_t num_bits) {
    return (value >> num_bits) | (value << (32 - num_bits));
}

/* ==================== Register-Style Operations ==================== */

/**
 * Structure for treating consecutive bits as a field
 * Useful for documenting bit layouts in registers
 */
typedef struct {
    volatile uint32_t *reg;  /* Pointer to register */
    uint8_t start_bit;       /* Starting bit position */
    uint8_t num_bits;        /* Number of bits in field */
} bitfield_t;

/**
 * Create a bitfield reference
 */
static inline bitfield_t bitfield_create(volatile uint32_t *reg, 
                                         uint8_t start_bit, uint8_t num_bits) {
    return (bitfield_t) { .reg = reg, .start_bit = start_bit, .num_bits = num_bits };
}

/**
 * Read bitfield value
 */
static inline uint32_t bitfield_read(const bitfield_t *bf) {
    if (!bf || !bf->reg) return 0;
    return bit_extract(*bf->reg, bf->start_bit, bf->num_bits);
}

/**
 * Write bitfield value
 */
static inline void bitfield_write(bitfield_t *bf, uint32_t value) {
    if (!bf || !bf->reg) return;
    *bf->reg = bit_insert(*bf->reg, bf->start_bit, bf->num_bits, value);
}

/**
 * Set all bits in bitfield
 */
static inline void bitfield_set(bitfield_t *bf) {
    if (!bf || !bf->reg) return;
    uint32_t mask = ((1u << bf->num_bits) - 1u) << bf->start_bit;
    *bf->reg |= mask;
}

/**
 * Clear all bits in bitfield
 */
static inline void bitfield_clear(bitfield_t *bf) {
    if (!bf || !bf->reg) return;
    uint32_t mask = ((1u << bf->num_bits) - 1u) << bf->start_bit;
    *bf->reg &= ~mask;
}

#endif /* BIT_UTILS_H */
