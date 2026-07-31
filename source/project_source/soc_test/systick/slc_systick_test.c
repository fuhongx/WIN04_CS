#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "reg_Dev.h"
#include "slc_systick_test.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_delay.h"
#include "slc_hal_timer.h"
#include "slc_hal_intc.h"


int slc_systick_accuracy_test(void)
{
    uint32_t start_time, end_time;
    float cost_time_us, cost_time_ms;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    PRINTF("Systick delay accuracy test start...\n");
    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_systick_delay_us(10000);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);

    cost_time_us = (start_time - end_time) / 50.0f;

    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_systick_delay_ms(1000);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);

    cost_time_ms = (start_time - end_time) / 50000.0f;

    if ((fabsf(cost_time_us - 10000.0f) > 500.0f) || (fabsf(cost_time_ms - 1000.0f) > 50.0f)) {
        slc_hal_timer_stop(HAL_TIMER0);
        PRINTF("systick delay test failed! us cost time: %.2f us, ms cost time: %.2f ms\n", cost_time_us, cost_time_ms);
        return -1;
    }
    PRINTF("systick delay test success! us cost time: %.2f us, ms cost time: %.2f ms\n", cost_time_us, cost_time_ms);

    /* Systick 开关测试：关闭后计数器停止，重新开启后延时恢复正常 */
    PRINTF("Systick switch test start...\n");
    SysTick->LOAD = 0xFFFFFF - 1;
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    slc_hal_nop_delay_ms(10);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;  /* 关闭 systick */

    uint32_t val_off = SysTick->VAL;
    slc_hal_nop_delay_ms(100);  /* 用 nop 延时，systick 不应继续计数 */
    if (val_off != SysTick->VAL) {
        slc_hal_timer_stop(HAL_TIMER0);
        PRINTF("Systick switch off test failed! val before: %u, val after: %u\n", val_off, SysTick->VAL);
        return -1;
    }

    /* 通过 delay 接口重新开启，检查 100ms 延时误差在 5% 以内 */
    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_systick_delay_ms(100);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_timer_stop(HAL_TIMER0);

    cost_time_ms = (start_time - end_time) / (slc_hal_sysctrl_get_system_clock() / 1000.0f);
    if (fabsf(cost_time_ms - 100.0f) > 5.0f) {
        PRINTF("Systick switch on test failed! cost time: %.2f ms\n", cost_time_ms);
        return -1;
    }

    PRINTF("Systick switch test success!\n");
    return 0;
}

uint32_t g_systick_irq_flag = 0;
void slc_systick_irq_handler(void)
{
    g_systick_irq_flag++;
    PRINTF("Systick IRQ Handler triggered!\n");
}

int slc_systick_intc_test(void)
{
    g_systick_irq_flag = 0;

    PRINTF("Systick interrupt test start...\n");
    slc_hal_systick_enable_irq();
    slc_hal_register_irq_handler(SysTick_IRQn, slc_systick_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(SysTick_IRQn, 0x3);

    slc_hal_systick_delay_s(1);

    slc_hal_systick_disable_irq();
    NVIC_DisableIRQ(SysTick_IRQn);

    if (g_systick_irq_flag != 0) {
        PRINTF("Systick interrupt test success!\n");
        return 0;
    } else {
        PRINTF("Systick interrupt test failed!\n");
        return -1;
    }
}

int slc_systick_maxtime_test(void)
{
    uint32_t start_time, end_time;
    float cost_time_us, expected_time_us;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    PRINTF("Systick delay maxtime test start...\n");
    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    // systick时钟源固定为内核时钟，配置外部时钟源不生效
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    end_time = slc_hal_timer_get_count(HAL_TIMER0);

    cost_time_us = (float)(start_time - end_time) / (slc_hal_sysctrl_get_system_clock() / 1000000);
    expected_time_us = (float)(0xFFFFFF) / (slc_hal_sysctrl_get_system_clock() / 1000000);
    slc_hal_timer_stop(HAL_TIMER0);

    if (fabsf(cost_time_us - expected_time_us) > (expected_time_us * 0.05f)) {
        PRINTF("systick delay maxtime test failed! us cost time: %.2f us, expect: %.2f us\n",
                cost_time_us, expected_time_us);
        return -1;
    } else {
        PRINTF("systick delay maxtime test success! us cost time: %.2f us, expect: %.2f us\n",
                cost_time_us, expected_time_us);
        return 0;
    }
}

static float slc_nop_delay_measure_ms(uint32_t delay_ms)
{
    uint32_t start_time;
    uint32_t end_time;
    float clk_khz;

    start_time = slc_hal_timer_get_count(HAL_TIMER0);
    slc_hal_nop_delay_ms(delay_ms);
    end_time = slc_hal_timer_get_count(HAL_TIMER0);

    clk_khz = slc_hal_sysctrl_get_system_clock() / 1000.0f;
    return (float)(start_time - end_time) / clk_khz;
}

static int slc_nop_delay_check_ms(uint32_t expect_ms, float tol_pct)
{
    float cost_ms;
    float err_ms;
    float tol_ms;

    cost_ms = slc_nop_delay_measure_ms(expect_ms);
    err_ms = cost_ms - (float)expect_ms;
    tol_ms = (float)expect_ms * tol_pct / 100.0f;

    PRINTF("nop_delay: expect=%ums, cost=%.3fms, err=%+.3fms, tol=±%.3fms\n",
           expect_ms, cost_ms, err_ms, tol_ms);

    if (fabsf(err_ms) > tol_ms) {
        return -1;
    }

    return 0;
}

/**
 * @brief RC50M 下验证 slc_hal_nop_delay_ms（底层 rom_utility_delay）精度。
 *
 * 设计逻辑
 * 1、切 RC50M，用 TIMER0 作参考计时
 * 2、依次调用 slc_hal_nop_delay_ms(1/10/100)，打印实测耗时
 * 3、测完恢复 FDB50M 并重新初始化 DEBUG UART
 *
 * check逻辑
 * 1、各档延时误差在 ±5% 以内
 */
int slc_nop_delay_rc50m_test(void)
{
    static const uint32_t delay_ms_list[] = {1U, 10U, 100U};
    uint32_t i;

    PRINTF("NOP delay RC50M test start\n");

    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);
    debug_printf_init();
    PRINTF("sysclk=%uHz (RC50M)\n", slc_hal_sysctrl_get_system_clock());

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);
    slc_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    slc_hal_timer_start(HAL_TIMER0);

    for (i = 0; i < (sizeof(delay_ms_list) / sizeof(delay_ms_list[0])); i++) {
        if (slc_nop_delay_check_ms(delay_ms_list[i], 5.0f) != 0) {
            slc_hal_timer_stop(HAL_TIMER0);
            slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);
            debug_printf_init();
            PRINTF("NOP delay RC50M test fail at %ums\n", delay_ms_list[i]);
            return -1;
        }
    }

    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);
    debug_printf_init();

    PRINTF("NOP delay RC50M test pass\n");
    return 0;
}
