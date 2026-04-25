#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_win04.h"
#include "qmx_hal_sysctrl.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_timer.h"
#include "qmx_hal_lptimer.h"
#include "qmx_timer_test.h"
#include "qmx_hal_delay.h"

volatile uint8_t g_qmx_timer_test_flag = 0;
volatile uint8_t g_qmx_timer_index = 0;

void qmx_timer_irq_handler(void)
{
    uint32_t sta = 0;

    if (g_qmx_timer_index == 0) {
        sta = qmx_hal_timer_get_irq_status(HAL_TIMER0);
        qmx_hal_timer_clear_irq(HAL_TIMER0, sta);
    } else if (g_qmx_timer_index == 1) {
        sta = qmx_hal_timer_get_irq_status(HAL_TIMER1);
        qmx_hal_timer_clear_irq(HAL_TIMER1, sta);
    } else if (g_qmx_timer_index == 2) {
        sta = qmx_hal_timer_get_irq_status(HAL_TIMER2);
        qmx_hal_timer_clear_irq(HAL_TIMER2, sta);
    }

    g_qmx_timer_test_flag = 1;
}

int qmx_timer_accuracy_test(void)
{
    uint8_t irq_num[3] = {TIMER0_IRQ, TIMER1_IRQ, TIMER2_IRQ};
    uint32_t sysctrl_peripheral[3] = {HAL_CLK_TIM0, HAL_CLK_TIM1, HAL_CLK_TIM2};

    for (int i = HAL_TIMER0; i < HAL_TIMER_MAX; i++) {
        g_qmx_timer_test_flag = 0;
        g_qmx_timer_index = i;

        qmx_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        qmx_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        qmx_hal_timer_init((hal_timer_id_e)i, qmx_hal_sysctrl_get_system_clock() * 2, true);

        qmx_hal_register_irq_handler((IRQn_Type)irq_num[i], qmx_timer_irq_handler);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i], 0x3);

        qmx_hal_timer_start((hal_timer_id_e)i);
        qmx_hal_nop_delay_ms(1950);

        if (g_qmx_timer_test_flag == 1) {
            PRINTF("TIMER%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        qmx_hal_nop_delay_ms(100);

        if (g_qmx_timer_test_flag == 0) {
            PRINTF("TIMER%d interrupt test fail!(counter 2s, sleep 2050ms)\n", i);
            return -1;
        }

        qmx_hal_timer_stop((hal_timer_id_e)i);
        QMX_HAL_DISABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i]);
        qmx_hal_unregister_irq_handler((IRQn_Type)irq_num[i]);
        PRINTF("TIMER%d Accuracy counter open close test pass!\n", i);
    }

    return 0;
}

int qmx_timer_pause_test(void)
{
    uint32_t sysctrl_peripheral[3] = {HAL_CLK_TIM0, HAL_CLK_TIM1, HAL_CLK_TIM2};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    for (int i = HAL_TIMER0; i < HAL_TIMER_MAX; i++) {
        qmx_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        qmx_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        qmx_hal_timer_init((hal_timer_id_e)i, qmx_hal_sysctrl_get_system_clock() * 50, false);

        qmx_hal_timer_start((hal_timer_id_e)i);
        count1 = qmx_hal_timer_get_count((hal_timer_id_e)i);

        qmx_hal_nop_delay_ms(100);
        qmx_hal_timer_stop((hal_timer_id_e)i);
        count2 = qmx_hal_timer_get_count((hal_timer_id_e)i);
        if (count1 == count2) {
            PRINTF("TIMER%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        qmx_hal_nop_delay_ms(100);
        count3 = qmx_hal_timer_get_count((hal_timer_id_e)i);

        if (count2 != count3) {
            PRINTF("TIMER%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("TIMER%d pause test pass!\n", i);
    }

    return 0;
}

void qmx_lptimer_irq_handler(void)
{
    uint32_t sta = 0;

    if (g_qmx_timer_index == 0) {
        sta = qmx_hal_lptimer_get_irq_status(HAL_LPTIMER0);
        qmx_hal_lptimer_clear_irq(HAL_LPTIMER0, sta);
    } else {
        sta = qmx_hal_lptimer_get_irq_status(HAL_LPTIMER1);
        qmx_hal_lptimer_clear_irq(HAL_LPTIMER1, sta);
    }

    g_qmx_timer_test_flag = 1;
}

int qmx_lptimer_accuracy_test(void)
{
    uint8_t irq_num[2] = {LPTIMER0_IRQ, LPTIMER1_IRQ};
    uint32_t sysctrl_peripheral[2] = {HAL_CLK_LPTIM0, HAL_CLK_LPTIM1};

    for (int i = HAL_LPTIMER0; i < HAL_LPTIMER_MAX; i++) {
        g_qmx_timer_test_flag = 0;
        g_qmx_timer_index = i;

        qmx_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        qmx_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        qmx_hal_lptimer_init((hal_lptimer_id_e)i, HAL_SYS_CLK_FREQ_32K * 2, true);

        qmx_hal_register_irq_handler((IRQn_Type)irq_num[i], qmx_lptimer_irq_handler);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i], 0x3);

        qmx_hal_lptimer_start((hal_lptimer_id_e)i);
        qmx_hal_nop_delay_ms(1950);

        if (g_qmx_timer_test_flag == 1) {
            PRINTF("LpTimer%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        qmx_hal_nop_delay_ms(100);

        if (g_qmx_timer_test_flag == 0) {
            PRINTF("LpTimer%d interrupt test fail!(counter 2s, sleep 2050ms)\n", i);
            return -1;
        }

        qmx_hal_lptimer_stop((hal_lptimer_id_e)i);
        QMX_HAL_DISABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i]);
        qmx_hal_unregister_irq_handler((IRQn_Type)irq_num[i]);
        PRINTF("LpTimer%d Accuracy test pass!\n", i);
    }

    return 0;
}

int qmx_lptimer_pause_test(void)
{
    uint32_t sysctrl_peripheral[2] = {HAL_CLK_LPTIM0, HAL_CLK_LPTIM1};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    for (int i = HAL_LPTIMER0; i < HAL_LPTIMER_MAX; i++) {
        qmx_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        qmx_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        qmx_hal_lptimer_init((hal_lptimer_id_e)i, HAL_SYS_CLK_FREQ_32K * 50, false);

        qmx_hal_lptimer_start((hal_lptimer_id_e)i);
        count1 = qmx_hal_lptimer_get_count((hal_lptimer_id_e)i);

        qmx_hal_nop_delay_ms(100);
        qmx_hal_lptimer_stop((hal_lptimer_id_e)i);
        count2 = qmx_hal_lptimer_get_count((hal_lptimer_id_e)i);
        if (count1 == count2) {
            PRINTF("LpTimer%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        qmx_hal_nop_delay_ms(100);
        count3 = qmx_hal_lptimer_get_count((hal_lptimer_id_e)i);

        if (count2 != count3) {
            PRINTF("LpTimer%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("LpTimer%d pause test pass!\n", i);
    }

    return 0;
}
