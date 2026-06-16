#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_sysctrl.h"
#include "hw_flash.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "slc_cache_test.h"
#include "slc_hal_rng.h"
#include "slc_hal_delay.h"
#include "slc_hal_delay.h"
#include "slc_hal_timer.h"

static inline void cycle_counter_init(void)
{
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);
    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);
}

static inline void cycle_counter_start(void)
{
    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);
}

static inline uint32_t cycle_counter_stop(void)
{
    uint32_t t = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_timer_stop(HAL_TIMER0);
    return 0xFFFFFFFF-t;
}

__attribute__((noinline))
static uint32_t read_flash_1k(const uint32_t *src) {
    uint32_t sum = 0;
    for (uint32_t i = 0; i < 1024; i++) {
        sum += src[i];
    }
    return sum;
}

static const volatile uint32_t *test_data = (volatile uint32_t *)(0x080FE200);
static const volatile uint32_t *test_data2 = (volatile uint32_t *)(0x0801E200);

int slc_cache_normal_test(void)
{
    PRINTF("=== Cache Hit/Miss Test ===\n");
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    __DSB(); __ISB();

    // TODO: check

    volatile uint32_t dummy;
    for (int loop = 0; loop < 100; loop++) {
        dummy += test_data[0];
        dummy += test_data[1];
        dummy += test_data[2];
        dummy += test_data[3];
        dummy += test_data[4];
        dummy += test_data[5];
        dummy += test_data[6];
        dummy += test_data[7];
        dummy += test_data[8];
        dummy += test_data[9];
        dummy += test_data[10];
        dummy += test_data[11];
        dummy += test_data[12];
        dummy += test_data[13];
        dummy += test_data[14];
        dummy += test_data[15];
    }
        
    for (int loop = 0; loop < 50; loop++) {
        dummy += test_data2[16];
        dummy += test_data2[17];
        dummy += test_data2[18];
        dummy += test_data2[19];
    }

    (void)dummy;

    // TODO: check result
    uint32_t hits, misses, total;

    printf("Cache Hits  : %u\n", (unsigned int)hits);
    printf("Cache Misses: %u\n", (unsigned int)misses);
    if (total > 0) {
        PRINTF("Hit ratio: %.2f%%\n", 100.0f * hits / total);
    } else {
        PRINTF("No cache events recorded.\n");
        return -1;
    }
    return 0;
}

int slc_cache_flush_test(void)
{
    PRINTF("Cache Cold vs Hot Test ===\n");

    PRINTF("1. enable cache with empty cache line, and start read 1KB flash memory\n");
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);
    __DSB(); __ISB();

    cycle_counter_init();
    cycle_counter_start();
    volatile uint32_t sum1 = read_flash_1k(test_data);
    uint32_t cycles_cold = cycle_counter_stop();

    PRINTF("2. flush cache, and start read 1KB flash memory\n");
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_FLUSH);
    __DSB(); __ISB();

    cycle_counter_start();
    volatile uint32_t sum2 = read_flash_1k(test_data);
    uint32_t cycles_flushed = cycle_counter_stop();

    PRINTF("3. read same 1KB flash memory again\n");
    cycle_counter_start();
    volatile uint32_t sum3 = read_flash_1k(test_data);
    uint32_t cycles_hot = cycle_counter_stop();

    PRINTF("Cold  (1st read) : %u cycles (sum=%08x)\n", cycles_cold, (unsigned int)sum1);
    PRINTF("Flush (2nd read) : %u cycles (sum=%08x)\n", cycles_flushed, (unsigned int)sum2);
    PRINTF("Hot   (3rd read) : %u cycles (sum=%08x)\n", cycles_hot, (unsigned int)sum3);

    if (sum1 == sum2 && sum2 == sum3) {
        PRINTF("Data integrity OK.\n");
    } else {
        PRINTF("Data mismatch!\n");
        return -1;
    }
    PRINTF("Cache speedup: %.3fx (hot vs cold)\n", (float)cycles_cold / cycles_hot);
    return 0;
}
