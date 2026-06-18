/**
 * @file bit_utils.c
 * @brief Bit manipulation utilities implementation
 */

#include "bit_utils.h"

uint8_t bit_popcount(uint32_t value) {
    uint8_t count = 0;
    while (value) {
        value &= value - 1;  /* Clear the least significant set bit */
        count++;
    }
    return count;
}

uint8_t bit_find_first_set(uint32_t value) {
    if (value == 0) {
        return 32;
    }
    
    uint8_t position = 0;
    while ((value & 1u) == 0) {
        value >>= 1;
        position++;
    }
    
    return position;
}

uint8_t bit_find_last_set(uint32_t value) {
    if (value == 0) {
        return 32;
    }
    
    uint8_t position = 31;
    while ((value & (1u << position)) == 0) {
        position--;
    }
    
    return position;
}

uint32_t bit_reverse(uint32_t value, uint8_t num_bits) {
    uint32_t result = 0;
    
    for (uint8_t i = 0; i < num_bits; i++) {
        result = (result << 1) | (value & 1u);
        value >>= 1;
    }
    
    return result;
}
