#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "qmx_pwm_test.h"
#include "qmx_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "qmx_hal_intc.h"
#include "qmx_hal_pwm.h"
#include "qmx_hal_gpio.h"
#include "qmx_hal_delay.h"

volatile uint8_t g_pwm_timer_test_flag = 0;

void qmx_pwm_timer_irq_handler(void)
{
    uint32_t sta = 0;

    sta = qmx_hal_pwm_timer_get_irq_status();
    qmx_hal_pwm_timer_clear_irq(sta);
    PRINTF("PWM timer irq status: 0x%x\n", sta);

    g_pwm_timer_test_flag = sta;
}

int qmx_pwm_timer_accuracy_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    pwm_config.enable_irq = true;
    pwm_config.enable = false;
    pwm_config.polar = HAL_PWM_POL_HIGH;
    pwm_config.duty_cycle = qmx_hal_sysctrl_get_system_clock();

    for (int i = HAL_PWM_CH0; i < HAL_PWM_CH_MAX; i++) {
        g_pwm_timer_test_flag = 0;
        pwm_config.period = qmx_hal_sysctrl_get_system_clock() * 2;

        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

        qmx_hal_pwm_timer_init((hal_pwm_ch_e)i, &pwm_config);
        qmx_hal_register_irq_handler(PWM_IRQ, qmx_pwm_timer_irq_handler);
        QMX_HAL_ENABLE_PERIPHERAL_IRQ(PWM_IRQ, 0x3);

        qmx_hal_pwm_timer_start((hal_pwm_ch_e)i);
        qmx_hal_nop_delay_ms(1950);

        if (g_pwm_timer_test_flag != 0) {
            PRINTF("PWM TIMER CH%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        qmx_hal_nop_delay_ms(100);

        if (g_pwm_timer_test_flag != BIT(i)) {
            PRINTF("PWM TIMER CH%d interrupt(0x%x) test fail!(counter 2s, sleep 2050ms)\n",
                    i, g_pwm_timer_test_flag);
            return -1;
        }

        qmx_hal_pwm_timer_stop((hal_pwm_ch_e)i);
        QMX_HAL_DISABLE_PERIPHERAL_IRQ(PWM_IRQ);
        qmx_hal_unregister_irq_handler(PWM_IRQ);
        PRINTF("PWM TIMER CH%d Accuracy counter open close test pass!\n", i);
    }

    return 0;
}

int qmx_pwm_timer_pause_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    pwm_config.enable_irq = false;
    pwm_config.enable = false;
    pwm_config.polar = HAL_PWM_POL_HIGH;
    pwm_config.duty_cycle = qmx_hal_sysctrl_get_system_clock();

    for (int i = HAL_PWM_CH0; i < HAL_PWM_CH_MAX; i++) {
        qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
        qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

        pwm_config.period = qmx_hal_sysctrl_get_system_clock() * 50;
        qmx_hal_pwm_timer_init((hal_pwm_ch_e)i, &pwm_config);

        qmx_hal_pwm_timer_start((hal_pwm_ch_e)i);
        count1 = qmx_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);

        qmx_hal_nop_delay_ms(100);
        qmx_hal_pwm_timer_stop((hal_pwm_ch_e)i);
        count2 = qmx_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);
        if (count1 == count2) {
            PRINTF("PWM TIMER CH%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        qmx_hal_nop_delay_ms(100);
        count3 = qmx_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);

        if (count2 != count3) {
            PRINTF("PWM TIMER CH%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("TIMER%d pause test pass!\n", i);
    }

    return 0;
}

int qmx_pwm_timer_output_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE6);
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_MODE6);
    qmx_hal_gpio_set_iomux(HAL_GPIO_PIN14, HAL_IOMUX_MODE6);

    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

    pwm_config.enable_irq = false;
    pwm_config.enable = true;

    // 50% duty cycle, 1MHz
    pwm_config.period = qmx_hal_sysctrl_get_system_clock()/1000000-1;
    pwm_config.duty_cycle = qmx_hal_sysctrl_get_system_clock()/1000000/2;
    pwm_config.polar = HAL_PWM_POL_HIGH;

    qmx_hal_pwm_timer_init(HAL_PWM_CH0, &pwm_config);
    qmx_hal_pwm_timer_start(HAL_PWM_CH0);

    // 20% duty cycle, 1KHz
    pwm_config.period = qmx_hal_sysctrl_get_system_clock()/1000-1;
    pwm_config.duty_cycle = qmx_hal_sysctrl_get_system_clock()/1000/5;
    pwm_config.polar = HAL_PWM_POL_LOW;
    qmx_hal_pwm_timer_init(HAL_PWM_CH1, &pwm_config);
    qmx_hal_pwm_timer_start(HAL_PWM_CH1);

    // 10% duty cycle, 1KHz
    pwm_config.period = qmx_hal_sysctrl_get_system_clock()/1000-1;
    pwm_config.duty_cycle = qmx_hal_sysctrl_get_system_clock()/1000/10;
    pwm_config.polar = HAL_PWM_POL_LOW;
    qmx_hal_pwm_timer_init(HAL_PWM_CH2, &pwm_config);
    qmx_hal_pwm_timer_start(HAL_PWM_CH2);

    while(1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}
