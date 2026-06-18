/**
 * @file    led_blink_app.c
 * @brief   Mini RTOS – Multi-Task LED Blink Example
 *
 * This file shows how to use the Mini RTOS to blink multiple virtual LEDs
 * at different rates using separate tasks, demonstrating:
 *
 *   1. Task creation at different priority levels.
 *   2. rtos_delay() for non-blocking periodic timing.
 *   3. Task suspension and resumption at runtime.
 *   4. How the preemptive scheduler ensures high-priority tasks always
 *      interrupt lower-priority ones immediately.
 *
 * Virtual LED mapping (STM32F103 Blue Pill):
 *   LED1 → PC13  (on-board LED, active LOW)
 *   LED2 → PC14  (external LED, active HIGH — connect via 330 Ω resistor)
 *   LED3 → PC15  (external LED, active HIGH — connect via 330 Ω resistor)
 *
 * To use this file as the main application:
 *   • Remove or rename src/app/main.c.
 *   • Add a call to led_blink_app_init() from your startup code, then call
 *     rtos_start().  Or simply rename led_blink_main() to main().
 *
 * Target: ARM Cortex-M3 (STM32F103)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "rtos_kernel.h"
#include "task_manager.h"
#include "config.h"

/* =========================================================================
 * STM32F103 GPIO Minimal Definitions
 * ========================================================================= */

#define RCC_APB2ENR    (*((volatile uint32_t *)0x40021018U))
#define RCC_IOPCEN     (1U << 4)

#define GPIOC_CRH      (*((volatile uint32_t *)0x40011004U))
#define GPIOC_ODR      (*((volatile uint32_t *)0x4001100CU))

#define LED1_PIN       (1U << 13)   /* PC13 — active LOW  */
#define LED2_PIN       (1U << 14)   /* PC14 — active HIGH */
#define LED3_PIN       (1U << 15)   /* PC15 — active HIGH */

/* =========================================================================
 * GPIO Helpers
 * ========================================================================= */

static void gpio_init_leds(void)
{
    RCC_APB2ENR |= RCC_IOPCEN;

    /* Set PC13, PC14, PC15 as output 2 MHz push-pull.
     * CRH bits for pin N = [(N-8)*4 + 3 : (N-8)*4]
     *   Pin 13 → bits [23:20], Pin 14 → bits [27:24], Pin 15 → bits [31:28] */
    GPIOC_CRH &= ~(0xFFFU << 20);
    GPIOC_CRH |=  (0x222U << 20);   /* MODE=10 (2 MHz output) for each pin */

    /* Start with all LEDs off */
    GPIOC_ODR |=  LED1_PIN;           /* LED1 off (active LOW) */
    GPIOC_ODR &= ~(LED2_PIN | LED3_PIN); /* LED2, LED3 off (active HIGH) */
}

static inline void led1_toggle(void) { GPIOC_ODR ^=  LED1_PIN; }
static inline void led2_toggle(void) { GPIOC_ODR ^=  LED2_PIN; }
static inline void led3_toggle(void) { GPIOC_ODR ^=  LED3_PIN; }

/* =========================================================================
 * Task Stacks
 * ========================================================================= */

static uint32_t stack_led1[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_led2[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_led3[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_ctrl[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];

/* Global handles so the controller task can suspend/resume LED tasks */
static rtos_task_handle_t h_led1;
static rtos_task_handle_t h_led2;
static rtos_task_handle_t h_led3;

/* =========================================================================
 * LED Task Bodies
 *
 * Each task receives its blink period (in ticks) as the void* argument.
 * ========================================================================= */

/**
 * @brief  LED1 blink task — priority 2, fast blink (100 ms period).
 *
 * Represents a high-frequency status indicator (e.g., "heartbeat").
 */
static void task_led1(void *arg)
{
    uint32_t period_ticks = (uint32_t)(uintptr_t)arg;

    for (;;) {
        led1_toggle();
        rtos_delay(period_ticks);
    }
}

/**
 * @brief  LED2 blink task — priority 4, medium blink (500 ms period).
 *
 * Represents a secondary status indicator.
 */
static void task_led2(void *arg)
{
    uint32_t period_ticks = (uint32_t)(uintptr_t)arg;

    for (;;) {
        led2_toggle();
        rtos_delay(period_ticks);
    }
}

/**
 * @brief  LED3 blink task — priority 6, slow blink (1000 ms period).
 *
 * Represents a low-priority activity indicator.
 */
static void task_led3(void *arg)
{
    uint32_t period_ticks = (uint32_t)(uintptr_t)arg;

    for (;;) {
        led3_toggle();
        rtos_delay(period_ticks);
    }
}

/**
 * @brief  Controller task — priority 1 (highest).
 *
 * Demonstrates task suspension and resumption:
 *   • After 5 seconds, suspends LED3 (slow blinker stops).
 *   • After another 5 seconds, resumes LED3.
 *   • Repeats forever.
 *
 * Because this task has the highest priority it will preempt all LED
 * tasks the moment its rtos_delay() expires.
 */
static void task_controller(void *arg)
{
    (void)arg;

    for (;;) {
        /* Let the LEDs blink freely for 5 seconds. */
        rtos_delay(5000);

        /* Suspend LED3 — it stops blinking. */
        rtos_task_suspend(h_led3);

        /* Wait another 5 seconds. */
        rtos_delay(5000);

        /* Resume LED3 — it starts blinking again. */
        rtos_task_resume(h_led3);
    }
}

/* =========================================================================
 * Application Init — call this instead of main() if embedding in a larger
 * project; rename to main() for a standalone build.
 * ========================================================================= */

void led_blink_app_init(void)
{
    gpio_init_leds();

    int rc = rtos_kernel_init();
    RTOS_ASSERT(rc == 0);

    /* Create LED tasks.  Pass the blink period as the void* argument.
     * We cast the integer to a pointer (valid C pattern for small integers). */
    rc = rtos_task_create(task_led1,
                          (void *)(uintptr_t)100U,   /* 100 ms blink period */
                          2,
                          stack_led1,
                          sizeof(stack_led1),
                          "led1",
                          &h_led1);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_led2,
                          (void *)(uintptr_t)500U,   /* 500 ms blink period */
                          4,
                          stack_led2,
                          sizeof(stack_led2),
                          "led2",
                          &h_led2);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_led3,
                          (void *)(uintptr_t)1000U,  /* 1 s blink period */
                          6,
                          stack_led3,
                          sizeof(stack_led3),
                          "led3",
                          &h_led3);
    RTOS_ASSERT(rc == 0);

    /* Controller task — highest priority, manages LED3 suspension. */
    rtos_task_handle_t h_ctrl;
    rc = rtos_task_create(task_controller,
                          NULL,
                          1,
                          stack_ctrl,
                          sizeof(stack_ctrl),
                          "ctrl",
                          &h_ctrl);
    RTOS_ASSERT(rc == 0);

    /* Start the scheduler — never returns. */
    rtos_start();
}
