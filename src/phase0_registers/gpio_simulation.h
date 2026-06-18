/**
 * @file gpio_simulation.h
 * @brief Simulated GPIO register interface — Phase 0 learning project
 * 
 * Demonstrates:
 * - How GPIO registers work on real hardware
 * - Volatile reads/writes for hardware safety
 * - Bitfield operations on registers
 * - State tracking for pin modes
 * 
 * Learning goal: Understand register-level programming before buying hardware
 * 
 * NOTE: This is a SIMULATION. Real hardware:
 *   - Pins actually drive voltage (3.3V or 0V)
 *   - Input reads actual voltage on pin
 *   - Timing is deterministic and fast
 *   - Here we just simulate state changes
 */

#ifndef GPIO_SIMULATION_H
#define GPIO_SIMULATION_H

#include <stdint.h>
#include <stdbool.h>

/* Simulated GPIO Port Structure (like STM32F103) */

/**
 * GPIO Port Control Register Bit Layout (simplified)
 * Bits [1:0] for each pin (16 pins):
 *   00 = Input
 *   01 = Output (10 MHz)
 *   10 = Output (2 MHz)
 *   11 = Output (50 MHz)
 * 
 * In real hardware, these are separate CRL (pins 0-7) and CRH (pins 8-15)
 * We'll simulate both in one structure for simplicity.
 */
typedef struct {
    volatile uint32_t CRL;   /* Control Register Low (pins 0-7) */
    volatile uint32_t CRH;   /* Control Register High (pins 8-15) */
    volatile uint32_t IDR;   /* Input Data Register (read-only) */
    volatile uint32_t ODR;   /* Output Data Register (read/write) */
    volatile uint32_t BSRR;  /* Bit Set/Reset Register (write-only) */
} gpio_port_t;

/**
 * Global simulated ports (A, B, C)
 * In real hardware, these would be memory-mapped at fixed addresses
 * Here they're just structures for learning
 */
extern gpio_port_t GPIOA;
extern gpio_port_t GPIOB;
extern gpio_port_t GPIOC;

/* ==================== Pin Mode Configuration ==================== */

enum gpio_mode {
    GPIO_INPUT = 0x0,           /* 00 */
    GPIO_OUTPUT_10MHZ = 0x1,    /* 01 */
    GPIO_OUTPUT_2MHZ = 0x2,     /* 10 */
    GPIO_OUTPUT_50MHZ = 0x3     /* 11 */
};

enum gpio_cnf {
    GPIO_CNF_INPUT_ANALOG = 0x0,
    GPIO_CNF_INPUT_FLOATING = 0x1,
    GPIO_CNF_INPUT_PULLUPDOWN = 0x2,
    GPIO_CNF_OUTPUT_PUSHPULL = 0x0,
    GPIO_CNF_OUTPUT_OPENDRAIN = 0x1,
    GPIO_CNF_OUTPUT_AF_PUSHPULL = 0x2,
    GPIO_CNF_OUTPUT_AF_OPENDRAIN = 0x3
};

/**
 * Configure a GPIO pin mode
 * @param port pointer to GPIO port (e.g., &GPIOA)
 * @param pin pin number (0-15)
 * @param mode GPIO mode (input/output)
 * @param cnf configuration (analog/floating/pushpull/opendrain/etc)
 * 
 * This writes to the control register (CRL or CRH)
 */
void gpio_configure_pin(gpio_port_t *port, uint8_t pin, 
                        enum gpio_mode mode, enum gpio_cnf cnf);

/**
 * Set a pin to output logic 1 (or use BSRR)
 * @param port pointer to GPIO port
 * @param pin pin number (0-15)
 */
void gpio_set_pin(gpio_port_t *port, uint8_t pin);

/**
 * Clear a pin to output logic 0 (or use BSRR)
 */
void gpio_clear_pin(gpio_port_t *port, uint8_t pin);

/**
 * Toggle a pin
 */
void gpio_toggle_pin(gpio_port_t *port, uint8_t pin);

/**
 * Read pin input state
 * @param port pointer to GPIO port
 * @param pin pin number (0-15)
 * @return 1 if pin reads high, 0 if low
 * 
 * NOTE: In simulation, this reads whatever was last set
 *       In real hardware, this reads actual voltage on pin
 */
uint8_t gpio_read_pin(gpio_port_t *port, uint8_t pin);

/**
 * Set pin state directly (simulate external input)
 * @param port pointer to GPIO port
 * @param pin pin number (0-15)
 * @param state 0 or 1
 * 
 * This simulates an external input changing (e.g., button press)
 * In real hardware, this would happen automatically
 */
void gpio_set_input_state(gpio_port_t *port, uint8_t pin, uint8_t state);

/**
 * Get pin output state
 * @param port pointer to GPIO port
 * @param pin pin number (0-15)
 * @return 1 if output is set, 0 if cleared
 * 
 * Useful for reading what we just output (for debugging)
 */
uint8_t gpio_get_output_state(const gpio_port_t *port, uint8_t pin);

/**
 * Print register state for debugging
 * @param name port name (e.g., "GPIOA")
 * @param port pointer to GPIO port
 */
void gpio_print_state(const char *name, const gpio_port_t *port);

/**
 * Initialize simulated GPIO ports
 * Must be called once at startup
 */
void gpio_simulation_init(void);

#endif /* GPIO_SIMULATION_H */
