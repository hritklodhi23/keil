/**
 * @file    main.c
 * @brief   Mini RTOS – Main Application Entry Point
 *
 * Demonstrates the core Mini RTOS features:
 *   • Creating tasks at different priority levels
 *   • rtos_delay() for time-based scheduling
 *   • rtos_yield() for cooperative hand-off within the same priority
 *   • Shared resource protection with a mutex
 *
 * Hardware assumed: STM32F103C8T6 ("Blue Pill") @ 72 MHz
 *   - LED on PC13 (active LOW on Blue Pill)
 *
 * To adapt to a different MCU, change:
 *   1. The GPIO/RCC register definitions at the top of this file.
 *   2. RTOS_SYSCLK_HZ in config.h.
 *   3. The SystemInit() call (or replace with your MCU's HAL init).
 *
 * Target: ARM Cortex-M3 (STM32F103)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "rtos_kernel.h"
#include "task_manager.h"
#include "synchronization.h"
#include "config.h"

/* =========================================================================
 * STM32F103 Minimal Register Definitions
 * (No HAL / CMSIS device headers required — keeps the example self-contained)
 * ========================================================================= */

/* RCC – Reset and Clock Control */
#define RCC_APB2ENR   (*((volatile uint32_t *)0x40021018U))
#define RCC_APB2ENR_IOPCEN  (1U << 4)   /* Enable GPIOC clock */

/* GPIOC */
#define GPIOC_CRH     (*((volatile uint32_t *)0x40011004U))   /* Config reg high */
#define GPIOC_ODR     (*((volatile uint32_t *)0x4001100CU))   /* Output data reg */
#define GPIO_PIN_13   (1U << 13)

/* Simple busy-wait delay (used only before the RTOS starts) */
static void hw_delay_ms(volatile uint32_t ms)
{
    /* Approximate: 8000 iterations ≈ 1 ms at 8 MHz (HSI).
     * Adjust the multiplier if you change the system clock. */
    while (ms--) {
        for (volatile uint32_t i = 0; i < 8000; i++) { (void)i; }
    }
}

/* =========================================================================
 * GPIO Helper Functions
 * ========================================================================= */

static void led_init(void)
{
    /* Enable GPIOC peripheral clock */
    RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;

    /* Configure PC13 as push-pull output, max 2 MHz
     * CRH bits [23:20] control pin 13: MODE=10 (output 2MHz), CNF=00 (push-pull) */
    GPIOC_CRH &= ~(0xFU << 20);   /* Clear CNF13 and MODE13 */
    GPIOC_CRH |=  (0x2U << 20);   /* MODE13 = 10 (Output 2 MHz) */
}

static inline void led_on(void)
{
    GPIOC_ODR &= ~GPIO_PIN_13;   /* Active LOW on Blue Pill */
}

static inline void led_off(void)
{
    GPIOC_ODR |=  GPIO_PIN_13;
}

static inline void led_toggle(void)
{
    GPIOC_ODR ^= GPIO_PIN_13;
}

/* =========================================================================
 * Task Stacks (statically allocated)
 * ========================================================================= */

static uint32_t stack_high[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_med[RTOS_DEFAULT_STACK_SIZE  / sizeof(uint32_t)];
static uint32_t stack_low[RTOS_DEFAULT_STACK_SIZE  / sizeof(uint32_t)];

/* Shared mutex for UART / LED access */
static rtos_mutex_t g_led_mutex;

/* =========================================================================
 * Task Bodies
 * ========================================================================= */

/**
 * @brief  High-priority task (priority 1).
 *
 * Blinks the LED quickly (200 ms on, 200 ms off) — simulates a
 * time-critical control loop.
 */
static void task_high_priority(void *arg)
{
    (void)arg;

    for (;;) {
        rtos_mutex_lock(&g_led_mutex, 0);
        led_on();
        rtos_delay(200);   /* 200 ticks = 200 ms with 1 ms tick */
        led_off();
        rtos_mutex_unlock(&g_led_mutex);

        rtos_delay(200);
    }
}

/**
 * @brief  Medium-priority task (priority 3).
 *
 * Blinks the LED at a medium rate (500 ms on, 500 ms off) — simulates
 * a background monitoring task.
 */
static void task_med_priority(void *arg)
{
    (void)arg;

    for (;;) {
        rtos_mutex_lock(&g_led_mutex, 0);
        led_toggle();
        rtos_mutex_unlock(&g_led_mutex);

        rtos_delay(500);
    }
}

/**
 * @brief  Low-priority task (priority 5).
 *
 * Runs a slow "housekeeping" loop — toggles the LED once per second and
 * then yields so higher-priority tasks can run immediately.
 */
static void task_low_priority(void *arg)
{
    (void)arg;

    for (;;) {
        rtos_mutex_lock(&g_led_mutex, 0);
        led_toggle();
        rtos_mutex_unlock(&g_led_mutex);

        rtos_delay(1000);
        rtos_yield();   /* Explicit yield — good practice for lowest-priority work */
    }
}

/* =========================================================================
 * main()
 * ========================================================================= */

int main(void)
{
    /* ── Hardware Initialisation ────────────────────────────────────── */
    led_init();
    led_off();

    /* Optional: Flash LED twice to confirm hardware works before RTOS
     * starts (useful when first bringing up the board). */
    led_on();  hw_delay_ms(100);
    led_off(); hw_delay_ms(100);
    led_on();  hw_delay_ms(100);
    led_off(); hw_delay_ms(100);

    /* ── RTOS Initialisation ────────────────────────────────────────── */
    int rc = rtos_kernel_init();
    RTOS_ASSERT(rc == 0);

    /* ── Shared Resource Initialisation ────────────────────────────── */
    rtos_mutex_init(&g_led_mutex);

    /* ── Task Creation ──────────────────────────────────────────────── */
    rtos_task_handle_t h_high, h_med, h_low;

    rc = rtos_task_create(task_high_priority,
                          NULL,
                          1,                /* priority 1 (high) */
                          stack_high,
                          sizeof(stack_high),
                          "task_high",
                          &h_high);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_med_priority,
                          NULL,
                          3,                /* priority 3 (medium) */
                          stack_med,
                          sizeof(stack_med),
                          "task_med",
                          &h_med);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_low_priority,
                          NULL,
                          5,                /* priority 5 (low) */
                          stack_low,
                          sizeof(stack_low),
                          "task_low",
                          &h_low);
    RTOS_ASSERT(rc == 0);

    /* ── Start the Scheduler ────────────────────────────────────────── */
    /* rtos_start() never returns. */
    rtos_start();

    /* Should never reach here */
    for (;;);
}
