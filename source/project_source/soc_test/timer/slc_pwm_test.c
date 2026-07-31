#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "reg_Dev.h"
#include "slc_pwm_test.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_intc.h"
#include "slc_hal_pwm.h"
#include "slc_hal_gpio.h"
#include "slc_hal_delay.h"

volatile uint8_t g_pwm_timer_test_flag = 0;

void slc_pwm_timer_irq_handler(void)
{
    uint32_t sta = 0;

    sta = slc_hal_pwm_timer_get_irq_status();
    slc_hal_pwm_timer_clear_irq(sta);
    PRINTF("PWM timer irq status: 0x%x\n", sta);

    g_pwm_timer_test_flag = sta;
}

int slc_pwm_timer_accuracy_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    pwm_config.enable_irq = true;
    pwm_config.enable = false;
    pwm_config.polar = HAL_PWM_POL_HIGH;
    pwm_config.duty_cycle = slc_hal_sysctrl_get_system_clock();

    for (int i = HAL_PWM_CH0; i < HAL_PWM_CH_MAX; i++) {
        g_pwm_timer_test_flag = 0;
        pwm_config.period = slc_hal_sysctrl_get_system_clock() * 2;

        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

        slc_hal_pwm_timer_init((hal_pwm_ch_e)i, &pwm_config);
        slc_hal_register_irq_handler(PWM_IRQ, slc_pwm_timer_irq_handler);
        SLC_HAL_ENABLE_PERIPHERAL_IRQ(PWM_IRQ, 0x3);

        slc_hal_pwm_timer_start((hal_pwm_ch_e)i);
        slc_hal_nop_delay_ms(1950);

        if (g_pwm_timer_test_flag != 0) {
            PRINTF("PWM TIMER CH%d interrupt test fail!(counter 2s, sleep 1950ms)\n", i);
            return -1;
        }

        slc_hal_nop_delay_ms(100);

        if (g_pwm_timer_test_flag != BIT(i)) {
            PRINTF("PWM TIMER CH%d interrupt(0x%x) test fail!(counter 2s, sleep 2050ms)\n",
                    i, g_pwm_timer_test_flag);
            return -1;
        }

        slc_hal_pwm_timer_stop((hal_pwm_ch_e)i);
        SLC_HAL_DISABLE_PERIPHERAL_IRQ(PWM_IRQ);
        slc_hal_unregister_irq_handler(PWM_IRQ);
        PRINTF("PWM TIMER CH%d Accuracy counter open close test pass!\n", i);
    }

    return 0;
}

int slc_pwm_timer_pause_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    uint32_t count1;
    uint32_t count2;
    uint32_t count3;

    pwm_config.enable_irq = false;
    pwm_config.enable = false;
    pwm_config.polar = HAL_PWM_POL_HIGH;
    pwm_config.duty_cycle = slc_hal_sysctrl_get_system_clock();

    for (int i = HAL_PWM_CH0; i < HAL_PWM_CH_MAX; i++) {
        slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
        slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

        pwm_config.period = slc_hal_sysctrl_get_system_clock() * 50;
        slc_hal_pwm_timer_init((hal_pwm_ch_e)i, &pwm_config);

        slc_hal_pwm_timer_start((hal_pwm_ch_e)i);
        count1 = slc_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);

        slc_hal_nop_delay_ms(100);
        slc_hal_pwm_timer_stop((hal_pwm_ch_e)i);
        count2 = slc_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);
        if (count1 == count2) {
            PRINTF("PWM TIMER CH%d counter is equal to begin after delay 100ms when running! test fail!\n", i);
            return -1;
        }
        slc_hal_nop_delay_ms(100);
        count3 = slc_hal_pwm_timer_get_counter((hal_pwm_ch_e)i);

        if (count2 != count3) {
            PRINTF("PWM TIMER CH%d's counter is still change when stop! test fail!\n", i);
            return -1;
        }

        PRINTF("TIMER%d pause test pass!\n", i);
    }

    return 0;
}

int slc_pwm_timer_output_test(void)
{
    hal_pwm_config_t pwm_config = {0};
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN4, HAL_IOMUX_MODE6);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_MODE6);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN14, HAL_IOMUX_MODE6);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

    pwm_config.enable_irq = false;
    pwm_config.enable = true;

    // 50% duty cycle, 1MHz
    pwm_config.period = slc_hal_sysctrl_get_system_clock()/1000000-1;
    pwm_config.duty_cycle = slc_hal_sysctrl_get_system_clock()/1000000/2;
    pwm_config.polar = HAL_PWM_POL_HIGH;

    slc_hal_pwm_timer_init(HAL_PWM_CH0, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH0);

    // 20% duty cycle, 1KHz
    pwm_config.period = slc_hal_sysctrl_get_system_clock()/1000-1;
    pwm_config.duty_cycle = slc_hal_sysctrl_get_system_clock()/1000/5;
    pwm_config.polar = HAL_PWM_POL_LOW;
    slc_hal_pwm_timer_init(HAL_PWM_CH1, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH1);

    // 10% duty cycle, 1KHz
    pwm_config.period = slc_hal_sysctrl_get_system_clock()/1000-1;
    pwm_config.duty_cycle = slc_hal_sysctrl_get_system_clock()/1000/10;
    pwm_config.polar = HAL_PWM_POL_LOW;
    slc_hal_pwm_timer_init(HAL_PWM_CH2, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH2);

    while(1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

/* PIN16=PWM1(CH0), PIN17=PWM2(CH1), PIN14=PWM3(CH2), MODE6
 * 下列百分比表示 duty Test 在 POL_HIGH 下的高电平时间占比（常规定义）。
 * POL_HIGH: count>=duty 为高 → 比较值写 (100-high%)，示波器高电平≈high%
 * polar Test 复用同一组比较值，仅改 POL_LOW → 波形整体反相
 */
#define SLC_PWM_DUTY_FREQ_HZ        (1000U)
#define SLC_PWM_DUTY_CH0_PCT        (25U)   /* duty Test: high ≈ 25% */
#define SLC_PWM_DUTY_CH1_PCT        (50U)   /* duty Test: high ≈ 50% */
#define SLC_PWM_DUTY_CH2_PCT        (75U)   /* duty Test: high ≈ 75% */

static void slc_pwm_timer_pins_to_pwm_mode(void)
{
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN16, HAL_IOMUX_MODE6);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN17, HAL_IOMUX_MODE6);
    slc_hal_gpio_set_iomux(HAL_GPIO_PIN14, HAL_IOMUX_MODE6);
}

static uint32_t slc_pwm_timer_period_cnt(uint32_t freq_hz)
{
    uint32_t sysclk = slc_hal_sysctrl_get_system_clock();

    if ((freq_hz == 0U) || (sysclk < freq_hz)) {
        return 0U;
    }

    return (sysclk / freq_hz) - 1U;
}

/*
 * 按「POL_HIGH 下高电平占比」换算比较值。
 * duty / polar 两用例共用此比较值，只改 polar，才能看到波形反相。
 */
static uint32_t slc_pwm_timer_cmp_for_high_duty(uint32_t freq_hz, uint32_t high_pct)
{
    uint32_t sysclk = slc_hal_sysctrl_get_system_clock();
    uint32_t cmp_pct;

    if ((freq_hz == 0U) || (high_pct > 100U)) {
        return 0U;
    }

    cmp_pct = 100U - high_pct; /* POL_HIGH: low≈cmp%, high≈high% */
    return (sysclk / freq_hz) * cmp_pct / 100U;
}

static void slc_pwm_timer_start_channels(hal_pwm_polar_e polar)
{
    hal_pwm_config_t pwm_config = {0};
    uint32_t period = slc_pwm_timer_period_cnt(SLC_PWM_DUTY_FREQ_HZ);
    uint32_t duty_ch0 = slc_pwm_timer_cmp_for_high_duty(SLC_PWM_DUTY_FREQ_HZ, SLC_PWM_DUTY_CH0_PCT);
    uint32_t duty_ch1 = slc_pwm_timer_cmp_for_high_duty(SLC_PWM_DUTY_FREQ_HZ, SLC_PWM_DUTY_CH1_PCT);
    uint32_t duty_ch2 = slc_pwm_timer_cmp_for_high_duty(SLC_PWM_DUTY_FREQ_HZ, SLC_PWM_DUTY_CH2_PCT);

    slc_hal_sysctrl_peripheral_clk_enable(HAL_CLK_TIM3, true);
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_TIM3);

    pwm_config.enable_irq = false;
    pwm_config.enable = true;
    pwm_config.polar = polar;
    pwm_config.period = period;

    pwm_config.duty_cycle = duty_ch0;
    slc_hal_pwm_timer_init(HAL_PWM_CH0, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH0);

    pwm_config.duty_cycle = duty_ch1;
    slc_hal_pwm_timer_init(HAL_PWM_CH1, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH1);

    pwm_config.duty_cycle = duty_ch2;
    slc_hal_pwm_timer_init(HAL_PWM_CH2, &pwm_config);
    slc_hal_pwm_timer_start(HAL_PWM_CH2);

    PRINTF("PWM TIMER3 running: freq=%uHz, polar=%s\n",
           SLC_PWM_DUTY_FREQ_HZ, (polar == HAL_PWM_POL_HIGH) ? "HIGH" : "LOW");
    PRINTF("  PIN16=CH0, PIN17=CH1, PIN14=CH2 (same cmp regs)\n");
    if (polar == HAL_PWM_POL_HIGH) {
        PRINTF("  expect HIGH-level duty: CH0=%u%% CH1=%u%% CH2=%u%%\n",
               SLC_PWM_DUTY_CH0_PCT, SLC_PWM_DUTY_CH1_PCT, SLC_PWM_DUTY_CH2_PCT);
    } else {
        PRINTF("  expect inverted vs duty Test: CH0=%u%% CH1=%u%% CH2=%u%% high\n",
               100U - SLC_PWM_DUTY_CH0_PCT, 100U - SLC_PWM_DUTY_CH1_PCT, 100U - SLC_PWM_DUTY_CH2_PCT);
    }
}

/**
 * @brief PWM 占空比测试（TIMER3 / CH0~CH2）
 *
 * 设计逻辑
 * 1、IOMUX：PIN16/PIN17/PIN14 配为 PWM MODE6
 * 2、使能 TIMER3，CH0/CH1/CH2 高电平占空比 25%/50%/75%（1kHz，POL_HIGH）
 *
 * check逻辑
 * 1、示波器看高电平占比是否约为 25%/50%/75%
 */
int slc_pwm_timer_duty_test(void)
{
    PRINTF("PWM duty test: probe PIN16/PIN17/PIN14 with LA/scope\n");

    slc_pwm_timer_pins_to_pwm_mode();
    slc_pwm_timer_start_channels(HAL_PWM_POL_HIGH);

    PRINTF("PWM duty output keep running, check HIGH-level duty on scope then reset board\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

/**
 * @brief PWM 极性测试（TIMER3 / CH0~CH2）
 *
 * 设计逻辑
 * 1、引脚与比较寄存器值同 duty Test（不重新按低极性换算）
 * 2、仅将极性改为 LOW，波形相对 duty Test 反相
 *
 * check逻辑
 * 1、示波器上高/低整体翻转：原 high≈25/50/75% → 约 75/50/25%
 */
int slc_pwm_timer_polar_test(void)
{
    PRINTF("PWM polar test: same cmp as duty Test, polar=LOW (inverted wave)\n");
    PRINTF("Compare with PWM duty Test waveform on PIN16/PIN17/PIN14\n");

    slc_pwm_timer_pins_to_pwm_mode();
    slc_pwm_timer_start_channels(HAL_PWM_POL_LOW);

    PRINTF("PWM polar output keep running, check inverted wave then reset board\n");
    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}
