#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "reg_Dev.h"
#include "qmx_systick_test.h"
#include "qmx_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "qmx_hal_delay.h"
#include "qmx_hal_timer.h"
#include "qmx_hal_intc.h"

int qmx_systick_accuracy_test(void)
{
    uint32_t start_time, end_time;
    float cost_time_us, cost_time_ms;

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    qmx_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    qmx_hal_timer_start(HAL_TIMER0);

    PRINTF("Systick delay accuracy test start...\n");
    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    qmx_hal_systick_delay_us(10000);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);

    cost_time_us = (start_time - end_time) / 50.0f;

    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    qmx_hal_systick_delay_ms(1000);
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);

    cost_time_ms = (start_time - end_time) / 50000.0f;

    qmx_hal_timer_stop(HAL_TIMER0);

    if ((fabsf(cost_time_us - 10000.0f) > 500.0f) || (fabsf(cost_time_ms - 1000.0f) > 50.0f)) {
        PRINTF("systick delay test failed! us cost time: %.2f us, ms cost time: %.2f ms\n", cost_time_us, cost_time_ms);
        return -1;
    } else {
        PRINTF("systick delay test success! us cost time: %.2f us, ms cost time: %.2f ms\n", cost_time_us, cost_time_ms);
        return 0;
    }
}

uint32_t g_systick_irq_flag = 0;
void qmx_systick_irq_handler(void)
{
    g_systick_irq_flag++;
    PRINTF("Systick IRQ Handler triggered!\n");
}

int qmx_systick_intc_test(void)
{
    g_systick_irq_flag = 0;

    PRINTF("Systick interrupt test start...\n");
    qmx_hal_systick_enable_irq();
    qmx_hal_register_irq_handler(SysTick_IRQn, qmx_systick_irq_handler);
    QMX_HAL_ENABLE_PERIPHERAL_IRQ(SysTick_IRQn, 0x3);

    qmx_hal_systick_delay_s(1);

    qmx_hal_systick_disable_irq();
    NVIC_DisableIRQ(SysTick_IRQn);

    if (g_systick_irq_flag != 0) {
        PRINTF("Systick interrupt test success!\n");
        return 0;
    } else {
        PRINTF("Systick interrupt test failed!\n");
        return -1;
    }
}

int qmx_systick_maxtime_test(void)
{
    uint32_t start_time, end_time;
    float cost_time_us, expected_time_us;

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    qmx_hal_timer_init(HAL_TIMER0, 0xFFFFFFFF, false);
    qmx_hal_timer_start(HAL_TIMER0);

    PRINTF("Systick delay maxtime test start...\n");
    start_time = qmx_hal_timer_get_count(HAL_TIMER0);
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL = 0;
    // systick时钟源固定为内核时钟，配置外部时钟源不生效
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    end_time = qmx_hal_timer_get_count(HAL_TIMER0);

    cost_time_us = (float)(start_time - end_time) / (qmx_hal_sysctrl_get_system_clock() / 1000000);
    expected_time_us = (float)(0xFFFFFF) / (qmx_hal_sysctrl_get_system_clock() / 1000000);
    qmx_hal_timer_stop(HAL_TIMER0);

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
