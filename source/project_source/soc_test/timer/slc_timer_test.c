#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_win04.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "debug.h"
#include "utility.h"
#include "hw_sysctrl.h"
#include "slc_hal_intc.h"
#include "slc_hal_timer.h"
#include "slc_hal_lptimer.h"
#include "slc_timer_test.h"
#include "slc_hal_delay.h"

volatile uint8_t g_slc_timer_test_flag = 0;
volatile uint8_t g_slc_timer_index = 0;

void slc_timer_irq_handler(void)
{
    uint32_t sta = 0;

    if (g_slc_timer_index == 0) {
        sta = slc_hal_timer_get_irq_status(HAL_TIMER0);
        slc_hal_timer_clear_irq(HAL_TIMER0, sta);
    } else if (g_slc_timer_index == 1) {
        sta = slc_hal_timer_get_irq_status(HAL_TIMER1);
        slc_hal_timer_clear_irq(HAL_TIMER1, sta);
    } else if (g_slc_timer_index == 2) {
        sta = slc_hal_timer_get_irq_status(HAL_TIMER2);
        slc_hal_timer_clear_irq(HAL_TIMER2, sta);
    }

    g_slc_timer_test_flag = 1;
}

int slc_timer_accuracy_test(void)
{
    /* SYS_CTRL PHY_FCLK_SEL寄存器无法读取, 增加用例验证芯片设计修改 2026.6.15 */
    uint32_t old = SYS_CTRL->LP_CLK_SEL;
    uint32_t mask = SYSCTRL_PHY_PMU_LP_CLK_MASK | SYSCTRL_APB1_LP_CLK_MASK;
    SYS_CTRL->LP_CLK_SEL = (old & (~mask)) | ((~old) & mask);
    uint32_t changed = SYS_CTRL->LP_CLK_SEL;
    if ((old ^ changed) != mask){
        PRINTF("SYS_CTRL(0x%02X,0x%02X):PHY_FCLK_SEL/APB1_SYS_CLK bit w/r fail!\n", old, changed);
        return -2;
    }
    SYS_CTRL->LP_CLK_SEL = old;
    PRINTF("SYS_CTRL:PHY_FCLK_SEL/APB1_SYS_CLK bit w/r PASS!\n");
    
    /* Original test */
    uint8_t irq_num[3] = {TIMER0_IRQ, TIMER1_IRQ, TIMER2_IRQ};
    uint32_t sysctrl_peripheral[3] = {HAL_CLK_TIM0, HAL_CLK_TIM1, HAL_CLK_TIM2};

    for (int i = HAL_TIMER0; i < HAL_TIMER_MAX; i++) {
        g_slc_timer_test_flag = 0;
        g_slc_timer_index = i;

        slc_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        slc_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        slc_hal_timer_init((hal_timer_id_e)i, slc_hal_sysctrl_get_system_clock() * 2, true);

        slc_hal_register_irq_handler((IRQn_Type)irq_num[i], slc_timer_irq_handler);
        SLC_HAL_ENABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i], 0x3);

        slc_hal_timer_start((hal_timer_id_e)i);
        slc_hal_nop_delay_ms(1950);

        if (g_slc_timer_test_flag == 1) {
            PRINTF("TIMER%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        slc_hal_nop_delay_ms(100);

        if (g_slc_timer_test_flag == 0) {
            PRINTF("TIMER%d interrupt test fail!(counter 2s, sleep 2050ms)\n", i);
            return -1;
        }

        slc_hal_timer_stop((hal_timer_id_e)i);
        SLC_HAL_DISABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i]);
        slc_hal_unregister_irq_handler((IRQn_Type)irq_num[i]);
        PRINTF("TIMER%d Accuracy counter open close test pass!\n", i);
    }

    return 0;
}

int slc_timer_pause_test(void)
{
    uint32_t sysctrl_peripheral[3] = {HAL_CLK_TIM0, HAL_CLK_TIM1, HAL_CLK_TIM2};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    for (int i = HAL_TIMER0; i < HAL_TIMER_MAX; i++) {
        slc_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        slc_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        slc_hal_timer_init((hal_timer_id_e)i, slc_hal_sysctrl_get_system_clock() * 50, false);

        slc_hal_timer_start((hal_timer_id_e)i);
        count1 = slc_hal_timer_get_count((hal_timer_id_e)i);

        slc_hal_nop_delay_ms(100);
        slc_hal_timer_stop((hal_timer_id_e)i);
        count2 = slc_hal_timer_get_count((hal_timer_id_e)i);
        if (count1 == count2) {
            PRINTF("TIMER%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        slc_hal_nop_delay_ms(100);
        count3 = slc_hal_timer_get_count((hal_timer_id_e)i);

        if (count2 != count3) {
            PRINTF("TIMER%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("TIMER%d pause test pass!\n", i);
    }

    return 0;
}

void slc_lptimer_irq_handler(void)
{
    uint32_t sta = 0;

    if (g_slc_timer_index == 0) {
        sta = slc_hal_lptimer_get_irq_status(HAL_LPTIMER0);
        slc_hal_lptimer_clear_irq(HAL_LPTIMER0, sta);
    } else {
        sta = slc_hal_lptimer_get_irq_status(HAL_LPTIMER1);
        slc_hal_lptimer_clear_irq(HAL_LPTIMER1, sta);
    }

    g_slc_timer_test_flag = 1;
}

int slc_lptimer_accuracy_test(void)
{
    uint8_t irq_num[2] = {LPTIMER0_IRQ, LPTIMER1_IRQ};
    uint32_t sysctrl_peripheral[2] = {HAL_CLK_LPTIM0, HAL_CLK_LPTIM1};

    for (int i = HAL_LPTIMER0; i < HAL_LPTIMER_MAX; i++) {
        g_slc_timer_test_flag = 0;
        g_slc_timer_index = i;

        slc_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        slc_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        slc_hal_lptimer_init((hal_lptimer_id_e)i, HAL_SYS_CLK_FREQ_32K * 2, true);

        slc_hal_register_irq_handler((IRQn_Type)irq_num[i], slc_lptimer_irq_handler);
        SLC_HAL_ENABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i], 0x3);

        slc_hal_lptimer_start((hal_lptimer_id_e)i);
        slc_hal_nop_delay_ms(1950);

        if (g_slc_timer_test_flag == 1) {
            PRINTF("LpTimer%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        slc_hal_nop_delay_ms(100);

        if (g_slc_timer_test_flag == 0) {
            PRINTF("LpTimer%d interrupt test fail!(counter 2s, sleep 2050ms)\n", i);
            return -1;
        }

        slc_hal_lptimer_stop((hal_lptimer_id_e)i);
        SLC_HAL_DISABLE_PERIPHERAL_IRQ((IRQn_Type)irq_num[i]);
        slc_hal_unregister_irq_handler((IRQn_Type)irq_num[i]);
        PRINTF("LpTimer%d Accuracy test pass!\n", i);
    }

    return 0;
}

int slc_lptimer_pause_test(void)
{
    uint32_t sysctrl_peripheral[2] = {HAL_CLK_LPTIM0, HAL_CLK_LPTIM1};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    for (int i = HAL_LPTIMER0; i < HAL_LPTIMER_MAX; i++) {
        slc_hal_sysctrl_peripheral_clk_enable((hal_peripheral_clk_e)sysctrl_peripheral[i], true);
        slc_hal_sysctrl_peripheral_mod_reset((hal_peripheral_clk_e)sysctrl_peripheral[i]);

        slc_hal_lptimer_init((hal_lptimer_id_e)i, HAL_SYS_CLK_FREQ_32K * 50, false);

        slc_hal_lptimer_start((hal_lptimer_id_e)i);
        count1 = slc_hal_lptimer_get_count((hal_lptimer_id_e)i);

        slc_hal_nop_delay_ms(100);
        slc_hal_lptimer_stop((hal_lptimer_id_e)i);
        count2 = slc_hal_lptimer_get_count((hal_lptimer_id_e)i);
        if (count1 == count2) {
            PRINTF("LpTimer%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        slc_hal_nop_delay_ms(100);
        count3 = slc_hal_lptimer_get_count((hal_lptimer_id_e)i);

        if (count2 != count3) {
            PRINTF("LpTimer%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("LpTimer%d pause test pass!\n", i);
    }

    return 0;
}
