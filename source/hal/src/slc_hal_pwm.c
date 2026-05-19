/**
 * ****************************************************************************
 * @file slc_hal_pwm.c
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "slc_hal_pwm.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_timer.h"

EN_PWM_TIMER_CHANNEL_T get_pwm_ch(hal_pwm_ch_e pwm_ch)
{
    switch (pwm_ch) {
        case HAL_PWM_CH0:
            return EN_PWM_TIMER_CH0;

        case HAL_PWM_CH1:
            return EN_PWM_TIMER_CH1;

        case HAL_PWM_CH2:
            return EN_PWM_TIMER_CH2;

        default:
            break;
    }

    return (EN_PWM_TIMER_CHANNEL_T)0;
}

void slc_hal_pwm_timer_init(hal_pwm_ch_e pwm_ch, hal_pwm_config_t *config)
{
    EN_PWM_TIMER_CHANNEL_T ch = get_pwm_ch(pwm_ch);

    if (ch == 0) {
        return;
    }

    rom_hw_pwm_timer_stop_counter(ch);
    rom_hw_pwm_timer_set_pwm_mode(ch, config->enable, (EN_PWM_POL_MODE_T)config->polar);
    rom_hw_pwm_timer_set_conter_reload_value(ch, config->period);
    rom_hw_pwm_timer_set_pwm_duty_cycle(ch, config->duty_cycle);

    if (config->enable_irq) {
        rom_hw_pwm_timer_set_interrupt_mask(BIT(pwm_ch), false);
        rom_hw_pwm_timer_enable_interrupt((EN_PWN_TIMER_INT_T)BIT(pwm_ch));
    } else {
        rom_hw_pwm_timer_set_interrupt_mask(BIT(pwm_ch), true);
        rom_hw_pwm_timer_disable_interrupt((EN_PWN_TIMER_INT_T)BIT(pwm_ch));
    }
}

void slc_hal_pwm_timer_start(hal_pwm_ch_e pwm_ch)
{
    EN_PWM_TIMER_CHANNEL_T ch = get_pwm_ch(pwm_ch);

    if (ch == 0) {
        return;
    }

    rom_hw_pwm_timer_start_counter(ch);
}

void slc_hal_pwm_timer_stop(hal_pwm_ch_e pwm_ch)
{
    EN_PWM_TIMER_CHANNEL_T ch = get_pwm_ch(pwm_ch);

    if (ch == 0) {
        return;
    }

    rom_hw_pwm_timer_stop_counter(ch);
}

void slc_hal_pwm_timer_enable_irq(hal_pwm_ch_e pwm_ch)
{
    if (pwm_ch >= HAL_PWM_CH_MAX) {
        return;
    }

    rom_hw_pwm_timer_set_interrupt_mask(BIT(pwm_ch), false);
    rom_hw_pwm_timer_enable_interrupt((EN_PWN_TIMER_INT_T)BIT(pwm_ch));
}

void slc_hal_pwm_timer_disable_irq(hal_pwm_ch_e pwm_ch)
{
    if (pwm_ch >= HAL_PWM_CH_MAX) {
        return;
    }

    rom_hw_pwm_timer_set_interrupt_mask(BIT(pwm_ch), true);
    rom_hw_pwm_timer_disable_interrupt((EN_PWN_TIMER_INT_T)BIT(pwm_ch));
}

uint32_t slc_hal_pwm_timer_get_counter(hal_pwm_ch_e pwm_ch)
{
    EN_PWM_TIMER_CHANNEL_T ch = get_pwm_ch(pwm_ch);
    uint32_t counter = 0;

    if (ch == 0) {
        return 0;
    }

    rom_hw_pwm_timer_get_conter(ch, &counter);

    return counter;
}

uint32_t slc_hal_pwm_timer_get_irq_status(void)
{
    uint8_t sta;
    rom_hw_pwm_timer_get_interrupt_flag(&sta);
    return sta;
}

void slc_hal_pwm_timer_clear_irq(uint32_t status)
{
    rom_hw_pwm_timer_clear_interrupt_flag(status);
}
