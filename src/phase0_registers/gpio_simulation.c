/**
 * @file gpio_simulation.c
 * @brief Simulated GPIO register implementation
 */

#include "gpio_simulation.h"
#include <stdio.h>
#include <string.h>

/* Global GPIO ports */
gpio_port_t GPIOA = {0};
gpio_port_t GPIOB = {0};
gpio_port_t GPIOC = {0};

void gpio_configure_pin(gpio_port_t *port, uint8_t pin, 
                        enum gpio_mode mode, enum gpio_cnf cnf) {
    if (!port || pin > 15) {
        return;
    }
    
    /* Combine mode (2 bits) and cnf (2 bits) into 4-bit field */
    uint32_t config = (cnf << 2) | mode;
    
    if (pin < 8) {
        /* CRL controls pins 0-7 */
        uint32_t shift = pin * 4;
        uint32_t mask = 0xF << shift;
        port->CRL = (port->CRL & ~mask) | ((config & 0xF) << shift);
    } else {
        /* CRH controls pins 8-15 */
        uint32_t shift = (pin - 8) * 4;
        uint32_t mask = 0xF << shift;
        port->CRH = (port->CRH & ~mask) | ((config & 0xF) << shift);
    }
}

void gpio_set_pin(gpio_port_t *port, uint8_t pin) {
    if (!port || pin > 15) {
        return;
    }
    
    /* Set bit in ODR (Output Data Register) */
    port->ODR |= (1u << pin);
    /* Also update BSRR (Bit Set/Reset Register) — lower 16 bits set */
    port->BSRR = (1u << pin);
}

void gpio_clear_pin(gpio_port_t *port, uint8_t pin) {
    if (!port || pin > 15) {
        return;
    }
    
    /* Clear bit in ODR */
    port->ODR &= ~(1u << pin);
    /* Update BSRR — upper 16 bits (+ 16) clear */
    port->BSRR = (1u << (pin + 16));
}

void gpio_toggle_pin(gpio_port_t *port, uint8_t pin) {
    if (!port || pin > 15) {
        return;
    }
    
    port->ODR ^= (1u << pin);
}

uint8_t gpio_read_pin(gpio_port_t *port, uint8_t pin) {
    if (!port || pin > 15) {
        return 0;
    }
    
    /* Read from IDR (Input Data Register) */
    return (port->IDR >> pin) & 1u;
}

void gpio_set_input_state(gpio_port_t *port, uint8_t pin, uint8_t state) {
    if (!port || pin > 15) {
        return;
    }
    
    if (state) {
        port->IDR |= (1u << pin);
    } else {
        port->IDR &= ~(1u << pin);
    }
}

uint8_t gpio_get_output_state(const gpio_port_t *port, uint8_t pin) {
    if (!port || pin > 15) {
        return 0;
    }
    
    return (port->ODR >> pin) & 1u;
}

void gpio_print_state(const char *name, const gpio_port_t *port) {
    if (!port) {
        return;
    }
    
    printf("\n=== %s State ===\n", name);
    printf("CRL:  0x%08lX\n", port->CRL);
    printf("CRH:  0x%08lX\n", port->CRH);
    printf("IDR:  0x%04X (inputs)\n", (unsigned int)(port->IDR & 0xFFFF));
    printf("ODR:  0x%04X (outputs)\n", (unsigned int)(port->ODR & 0xFFFF));
    printf("BSRR: 0x%08lX\n", port->BSRR);
    
    /* Print individual pin states */
    printf("Pins: ");
    for (int i = 0; i < 16; i++) {
        uint8_t output = (port->ODR >> i) & 1u;
        uint8_t input = (port->IDR >> i) & 1u;
        printf("[%d:%d/%d] ", i, output, input);
    }
    printf("\n");
}

void gpio_simulation_init(void) {
    /* Initialize all ports to zero */
    memset(&GPIOA, 0, sizeof(gpio_port_t));
    memset(&GPIOB, 0, sizeof(gpio_port_t));
    memset(&GPIOC, 0, sizeof(gpio_port_t));
}
