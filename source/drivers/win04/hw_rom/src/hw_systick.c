/**
 * ****************************************************************************
 * @file qmx_systick.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_sysctrl.h"
#include "hw_systick.h"

#define SYSTICK_MAX_MS_DELAY    300

void rom_hw_systick_delay_us(uint32_t us)
{
    uint32_t reload_cnt = (SystemCoreClock / 1000000) * us;

    if (reload_cnt > 0xFFFFFF || reload_cnt <= 1)
        return;

    SysTick->LOAD = reload_cnt - 1;
    SysTick->VAL = 0;
    // systick时钟源固定为内核时钟，配置外部时钟源不生效
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void rom_hw_systick_delay_ms(uint32_t ms)
{
    uint32_t reload_cnt = 0;

    if (ms == 0)
        return;

once:
    if (ms <= SYSTICK_MAX_MS_DELAY)
        reload_cnt = (SystemCoreClock / 1000) * ms;
    else
        reload_cnt = (SystemCoreClock / 1000) * SYSTICK_MAX_MS_DELAY;

    SysTick->LOAD = reload_cnt - 1;
    SysTick->VAL = 0;
    // systick时钟源固定为内核时钟，配置外部时钟源不生效
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0);
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

    if (ms > SYSTICK_MAX_MS_DELAY) {
        ms -= SYSTICK_MAX_MS_DELAY;
        goto once;
    }
}

void rom_hw_systick_delay_s(uint32_t s)
{
    while (s--)
        rom_hw_systick_delay_ms(1000);
}

void rom_hw_systick_enable_interrupt(void)
{
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

void rom_hw_systick_disable_interrupt(void)
{
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}
