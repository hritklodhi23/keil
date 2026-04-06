/**
 * @file    sync_example.c
 * @brief   Mini RTOS – Mutex and Semaphore Synchronisation Example
 *
 * Demonstrates three synchronisation scenarios:
 *
 *   1. MUTEX EXAMPLE — Shared counter protected by a mutex.
 *      Two tasks both increment a shared counter 100 times each.
 *      Without the mutex, the final value would be unpredictable
 *      (classic read-modify-write race condition).  With the mutex
 *      the final value is always exactly 200.
 *
 *   2. SEMAPHORE (PRODUCER / CONSUMER) — One task produces data items
 *      and posts a semaphore for each; another task waits on the semaphore
 *      and consumes the items.  This decouples producer timing from
 *      consumer timing.
 *
 *   3. PRIORITY INHERITANCE DEMO — Three tasks at priorities 1, 3, and 5.
 *      The low-priority task holds the mutex; the high-priority task
 *      tries to acquire it.  Without priority inheritance the medium-
 *      priority task would starve the high-priority task (classic priority
 *      inversion).  With inheritance the low-priority task is temporarily
 *      boosted so it finishes and releases the mutex quickly.
 *
 * Target: ARM Cortex-M3 (STM32F103)
 * Toolchain: Keil MDK / ARM Compiler 6
 */

#include "rtos_kernel.h"
#include "task_manager.h"
#include "synchronization.h"
#include "config.h"

/* =========================================================================
 * Shared resources
 * ========================================================================= */

static rtos_mutex_t     g_counter_mutex;    /* Protects g_shared_counter  */
static rtos_semaphore_t g_data_sem;         /* Producer/consumer signal   */
static rtos_mutex_t     g_pi_mutex;         /* Priority-inheritance demo  */

static volatile uint32_t g_shared_counter = 0;   /* Protected by g_counter_mutex */
static volatile uint32_t g_data_items     = 0;   /* Produced item count          */

/* =========================================================================
 * Task Stacks
 * ========================================================================= */

static uint32_t stack_counter_a[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_counter_b[RTOS_DEFAULT_STACK_SIZE / sizeof(uint32_t)];
static uint32_t stack_producer[RTOS_DEFAULT_STACK_SIZE  / sizeof(uint32_t)];
static uint32_t stack_consumer[RTOS_DEFAULT_STACK_SIZE  / sizeof(uint32_t)];
static uint32_t stack_pi_low[RTOS_DEFAULT_STACK_SIZE    / sizeof(uint32_t)];
static uint32_t stack_pi_med[RTOS_DEFAULT_STACK_SIZE    / sizeof(uint32_t)];
static uint32_t stack_pi_high[RTOS_DEFAULT_STACK_SIZE   / sizeof(uint32_t)];

/* =========================================================================
 * Example 1: Mutex-protected shared counter
 * ========================================================================= */

/**
 * @brief  Counter task A — increments g_shared_counter 100 times.
 *
 * Each increment is wrapped in a mutex lock/unlock to prevent races.
 */
static void task_counter_a(void *arg)
{
    (void)arg;

    for (uint32_t i = 0; i < 100; i++) {
        rtos_mutex_lock(&g_counter_mutex, 0);

        /* Critical section — safe to read-modify-write */
        uint32_t val = g_shared_counter;
        rtos_delay(1);              /* Yield to increase chance of interleaving */
        g_shared_counter = val + 1;

        rtos_mutex_unlock(&g_counter_mutex);
    }

    /* When both tasks finish, g_shared_counter should equal 200. */
    for (;;) {
        rtos_delay(1000);
    }
}

/**
 * @brief  Counter task B — increments g_shared_counter 100 times.
 */
static void task_counter_b(void *arg)
{
    (void)arg;

    for (uint32_t i = 0; i < 100; i++) {
        rtos_mutex_lock(&g_counter_mutex, 0);

        uint32_t val = g_shared_counter;
        rtos_delay(1);
        g_shared_counter = val + 1;

        rtos_mutex_unlock(&g_counter_mutex);
    }

    for (;;) {
        rtos_delay(1000);
    }
}

/* =========================================================================
 * Example 2: Semaphore producer / consumer
 * ========================================================================= */

/**
 * @brief  Producer task — generates one data item every 200 ms.
 *
 * Posts the semaphore after "writing" each item.
 */
static void task_producer(void *arg)
{
    (void)arg;

    for (;;) {
        /* Simulate data production (e.g., reading a sensor). */
        rtos_delay(200);

        /* "Publish" the item by incrementing the count and signalling. */
        g_data_items++;
        rtos_sem_post(&g_data_sem);
    }
}

/**
 * @brief  Consumer task — processes items as they arrive.
 *
 * Blocks on the semaphore until the producer signals.
 */
static void task_consumer(void *arg)
{
    (void)arg;
    volatile uint32_t processed = 0;

    for (;;) {
        /* Block until an item is available (timeout = 0 → wait forever). */
        rtos_sem_wait(&g_data_sem, 0);

        /* Process the item (dummy work). */
        processed++;
        rtos_delay(50);   /* Simulate processing time */
    }
}

/* =========================================================================
 * Example 3: Priority inheritance demo
 * ========================================================================= */

/**
 * @brief  Low-priority task (priority 5) — holds the mutex for a long time.
 *
 * Without priority inheritance, the medium-priority task (priority 3) would
 * run while the low-priority task holds the mutex, blocking the high-priority
 * task (priority 1) indefinitely — this is priority inversion.
 *
 * With priority inheritance, when the high-priority task blocks on the mutex
 * the low-priority task's priority is raised to 1 so it completes quickly.
 */
static void task_pi_low(void *arg)
{
    (void)arg;

    for (;;) {
        /* Acquire the mutex at low priority */
        rtos_mutex_lock(&g_pi_mutex, 0);

        /* Simulate a long critical section */
        rtos_delay(500);

        rtos_mutex_unlock(&g_pi_mutex);

        /* Sleep before next attempt */
        rtos_delay(1000);
    }
}

/**
 * @brief  Medium-priority task (priority 3) — CPU-bound work (no mutex).
 *
 * With naive scheduling (no priority inheritance) this task runs while
 * task_pi_low holds the mutex, starving task_pi_high.  With inheritance
 * task_pi_low is boosted above this task's priority and finishes quickly.
 */
static void task_pi_med(void *arg)
{
    (void)arg;
    volatile uint32_t busy_count = 0;

    for (;;) {
        /* Busy-work to consume CPU and demonstrate preemption. */
        busy_count++;
        rtos_delay(10);
    }
}

/**
 * @brief  High-priority task (priority 1) — tries to acquire the mutex.
 *
 * Blocks on the mutex.  With priority inheritance it unblocks sooner
 * because task_pi_low is temporarily elevated to priority 1.
 */
static void task_pi_high(void *arg)
{
    (void)arg;

    for (;;) {
        rtos_delay(200);   /* Periodic activation */

        /* Try to acquire the mutex (may block if task_pi_low holds it). */
        rtos_mutex_lock(&g_pi_mutex, 0);

        /* Use the shared resource. */
        rtos_delay(50);

        rtos_mutex_unlock(&g_pi_mutex);
    }
}

/* =========================================================================
 * Synchronisation Example Entry Point
 * ========================================================================= */

/**
 * @brief  Initialise and start the synchronisation examples.
 *
 * Rename this to main() for a standalone build, or call it from your
 * main() before rtos_start().
 */
void sync_example_init(void)
{
    int rc = rtos_kernel_init();
    RTOS_ASSERT(rc == 0);

    /* ── Shared resource initialisation ──────────────────────────────── */
    rtos_mutex_init(&g_counter_mutex);
    rtos_sem_init(&g_data_sem, 0, 10);   /* Binary-style semaphore, max count 10 */
    rtos_mutex_init(&g_pi_mutex);

    /* ── Example 1: Mutex counter tasks ──────────────────────────────── */
    rtos_task_handle_t h;
    rc = rtos_task_create(task_counter_a, NULL, 3,
                          stack_counter_a, sizeof(stack_counter_a),
                          "counter_a", &h);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_counter_b, NULL, 3,
                          stack_counter_b, sizeof(stack_counter_b),
                          "counter_b", &h);
    RTOS_ASSERT(rc == 0);

    /* ── Example 2: Producer / consumer ──────────────────────────────── */
    rc = rtos_task_create(task_producer, NULL, 4,
                          stack_producer, sizeof(stack_producer),
                          "producer", &h);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_consumer, NULL, 4,
                          stack_consumer, sizeof(stack_consumer),
                          "consumer", &h);
    RTOS_ASSERT(rc == 0);

    /* ── Example 3: Priority inheritance ─────────────────────────────── */
    rc = rtos_task_create(task_pi_low,  NULL, 5,
                          stack_pi_low,  sizeof(stack_pi_low),
                          "pi_low",  &h);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_pi_med,  NULL, 3,
                          stack_pi_med,  sizeof(stack_pi_med),
                          "pi_med",  &h);
    RTOS_ASSERT(rc == 0);

    rc = rtos_task_create(task_pi_high, NULL, 1,
                          stack_pi_high, sizeof(stack_pi_high),
                          "pi_high", &h);
    RTOS_ASSERT(rc == 0);

    /* ── Start scheduler ──────────────────────────────────────────────── */
    rtos_start();
}
