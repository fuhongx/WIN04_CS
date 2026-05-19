#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "slc_nvic_test.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_timer.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"

uint8_t g_slc_nvic_test_flag = 0;
uint8_t g_slc_nvic_nest_flag = 0;
uint8_t g_slc_nvic_priority_test_flag = 0;

void slc_timer0_irq_handler(void)
{
    uint32_t sta = 0;

    sta = slc_hal_timer_get_irq_status(HAL_TIMER0);
    slc_hal_timer_clear_irq(HAL_TIMER0, sta);

    g_slc_nvic_test_flag++;
    g_slc_nvic_priority_test_flag = 3;

    // wait nested interrupt
    if (g_slc_nvic_nest_flag)
        slc_hal_nop_delay_ms(200);

    PRINTF("Timer0 IRQ Handler sta: %d\n", sta);
}

void slc_timer1_irq_handler(void)
{
    uint32_t sta = 0;

    sta = slc_hal_timer_get_irq_status(HAL_TIMER1);
    slc_hal_timer_clear_irq(HAL_TIMER1, sta);

    PRINTF("Timer1 IRQ Handler sta: %d\n", sta);

    if ((g_slc_nvic_test_flag != 0) && (g_slc_nvic_priority_test_flag == 3)) {
        g_slc_nvic_priority_test_flag = 2;
    }

    g_slc_nvic_test_flag++;
}

int slc_nvic_intc_test(void)
{
    g_slc_nvic_test_flag = 0;
    g_slc_nvic_nest_flag = 0;
    g_slc_nvic_priority_test_flag = 0;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, slc_hal_sysctrl_get_system_clock(), true);

    slc_hal_register_irq_handler(TIMER0_IRQ, slc_timer0_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(TIMER0_IRQ, 0x3);

    slc_hal_timer_start(HAL_TIMER0);

    slc_hal_nop_delay_ms(1200);

    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_timer_disable_irq(HAL_TIMER0);
    NVIC_DisableIRQ(TIMER0_IRQ);

    slc_hal_nop_delay_ms(1200);

    if (g_slc_nvic_test_flag == 1) {
        PRINTF("NVIC intc test passed! Timer0 IRQ triggered\n");
        return 0;
    } else {
        PRINTF("NVIC intc test failed! Timer0 IRQ not triggered\n");
        return -1;
    }
}

int slc_nvic_nest_and_priority_test(void)
{
    int ret = 0;
    g_slc_nvic_test_flag = 0;
    g_slc_nvic_nest_flag = 1;
    g_slc_nvic_priority_test_flag = 0;

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, slc_hal_sysctrl_get_system_clock(), true);
    slc_hal_register_irq_handler(TIMER0_IRQ, slc_timer0_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(TIMER0_IRQ, 0x3);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM1);

    slc_hal_timer_init(HAL_TIMER1, slc_hal_sysctrl_get_system_clock() + slc_hal_sysctrl_get_system_clock() / 10, true);
    slc_hal_register_irq_handler(TIMER1_IRQ, slc_timer1_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(TIMER1_IRQ, 0x2);

    slc_hal_timer_start(HAL_TIMER0);
    slc_hal_timer_start(HAL_TIMER1);

    slc_hal_nop_delay_ms(1600);

    if (g_slc_nvic_test_flag == 2 && g_slc_nvic_priority_test_flag == 2) {
        PRINTF("NVIC nest and priority test passed! Timer0 IRQ nested Timer1 IRQ\n");
        ret = 0;
    } else {
        PRINTF("NVIC nest and priority test failed! nvic_test_flag 0x%X, priority_test_flag 0x%X\n",
               g_slc_nvic_test_flag, g_slc_nvic_priority_test_flag);
        ret = -1;
    }

    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_timer_disable_irq(HAL_TIMER0);
    NVIC_DisableIRQ(TIMER0_IRQ);

    slc_hal_timer_stop(HAL_TIMER1);
    slc_hal_timer_disable_irq(HAL_TIMER1);
    NVIC_DisableIRQ(TIMER1_IRQ);

    return ret;
}

int slc_nvic_primask_test(void)
{
    g_slc_nvic_test_flag = 0;
    g_slc_nvic_nest_flag = 0;
    g_slc_nvic_priority_test_flag = 0;

    SLC_HAL_SET_PRIMASK(1);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM0, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM0);

    slc_hal_timer_init(HAL_TIMER0, slc_hal_sysctrl_get_system_clock(), true);
    slc_hal_register_irq_handler(TIMER0_IRQ, slc_timer0_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(TIMER0_IRQ, 0x2);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM1, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM1);

    slc_hal_timer_init(HAL_TIMER1, slc_hal_sysctrl_get_system_clock() + slc_hal_sysctrl_get_system_clock() / 10, true);
    slc_hal_register_irq_handler(TIMER1_IRQ, slc_timer1_irq_handler);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(TIMER1_IRQ, 0x3);

    slc_hal_timer_start(HAL_TIMER0);
    slc_hal_timer_start(HAL_TIMER1);

    slc_hal_nop_delay_ms(1200);

    if (g_slc_nvic_test_flag == 0 && g_slc_nvic_priority_test_flag == 0) {
        PRINTF("NVIC primask test passed!\n");
    } else {
        PRINTF("NVIC primask test failed!\n");
        return -1;
    }

    write32(0x20000001, 0x2);   // trigger hardfault to exit PRIMASK

    slc_hal_timer_stop(HAL_TIMER0);
    slc_hal_timer_disable_irq(HAL_TIMER0);
    slc_hal_timer_clear_irq(HAL_TIMER0, 1);
    NVIC_DisableIRQ(TIMER0_IRQ);
    NVIC_ClearPendingIRQ(TIMER0_IRQ);

    slc_hal_timer_stop(HAL_TIMER1);
    slc_hal_timer_disable_irq(HAL_TIMER1);
    slc_hal_timer_clear_irq(HAL_TIMER1, 1);
    NVIC_DisableIRQ(TIMER1_IRQ);
    NVIC_ClearPendingIRQ(TIMER1_IRQ);

    SLC_HAL_SET_PRIMASK(0);

    return 0;
}
