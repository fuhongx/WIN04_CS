/**
 * ****************************************************************************
 * @file hw_sysctrl.c
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#include <stdint.h>
#include <stdio.h>

#include "utility.h"
#include "hw_sysctrl.h"
#include "compiler.h"

volatile uint32_t SystemCoreClock __ROM_RAM_SHARED = SYS_CLK_FREQ_50M;

EN_ERR_STA_T rom_hw_sysctrl_update_systemclock(void)
{
    uint32_t u32Clk = (SYS_CTRL->MCLK_CFG & SYSCTRL_MCLK_CFG_CLK_SRC_SEL_MAKS) >> SYSCTRL_MCLK_CFG_CLK_SRC_SEL_SHIFT;

    uint32_t u32Clock = SYS_CLK_FREQ_50M;

    if (u32Clk == EN_SYS_CLK_TCXO25M)
    {
        u32Clock = SYS_CLK_FREQ_25M;
    }

    switch (SYS_CTRL->SYS_CLK_CFG & SYSCTRL_SYS_CLK_CFG_REG_MASK)
    {
    case EN_SYS_CLK_MCLK_DIV_NONE:
    {
        break;
    }

    case EN_SYS_CLK_MCLK_DIV2:
    {
        u32Clock = u32Clock / 2;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV4:
    {
        u32Clock = u32Clock / 4;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV8:
    {
        u32Clock = u32Clock / 8;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV16:
    {
        u32Clock = u32Clock / 16;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV32:
    {
        u32Clock = u32Clock / 32;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV64:
    {
        u32Clock = u32Clock / 64;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV128:
    {
        u32Clock = u32Clock / 128;
        break;
    }

    case EN_SYS_CLK_MCLK_DIV256:
    {
        u32Clock = u32Clock / 256;
        break;
    }

    default:
        return EN_ERROR_STA_ERROR;
        break;
    }

    SystemCoreClock = u32Clock;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_system_clock_init(EN_SYSCTRL_SYS_CLK_T enClk, EN_SYSCTRL_SYS_CLK_DIV_T enDiv)
{
    if ((enClk >= EN_SYS_CLK_MAX) | (enDiv >= EN_SYS_CLK_MCLK_DIV_MAX))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t timeout = 0;
    // XTAL25M/FDB50M need wait for TCXO ready
    if (enClk != EN_SYS_CLK_RC50M) {
        do {
            if (SYS_CTRL->MCLK_CFG & SYSCTRL_MCLK_CFG_XO_RDY_MASK) {
                break;
            }

            rom_utility_delay_us(10);
            timeout += 10;
        } while (timeout < 100000);

        if (timeout >= 100000) {
            return EN_ERROR_STA_TIMEOUT;
        }
    }

    SYS_CTRL->MCLK_CFG &= ~(SYSCTRL_MCLK_CFG_CLK_SRC_SEL_MAKS << SYSCTRL_MCLK_CFG_CLK_SRC_SEL_SHIFT);
    SYS_CTRL->MCLK_CFG |= ((enClk & SYSCTRL_MCLK_CFG_CLK_SRC_SEL_MAKS) << SYSCTRL_MCLK_CFG_CLK_SRC_SEL_SHIFT);

    uint32_t u32Cfg = SYS_CTRL->SYS_CLK_CFG;
    u32Cfg &= ~(SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_MASK << SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_SHIFT);
    u32Cfg |= ((enDiv & SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_MASK) << SYSCTRL_SYS_CLK_CFG_SYS_CLK_DIV_SHIFT);
    SYS_CTRL->SYS_CLK_CFG = u32Cfg;

    rom_hw_sysctrl_update_systemclock();

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_set_bus_clock(EN_SYSCTRL_AHB_CLK_DIV_T enAhbDiv, EN_SYSCTRL_APB0_CLK_DIV_T enApbDiv)
{
    SYS_CTRL->PER_CLK_CFG = ((enAhbDiv & SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_MASK) << SYSCTRL_PER_CLK_CFG_AHB_CLK_DIV_SHIFT) |
                            ((enApbDiv & SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_MASK) << SYSCTRL_PER_CLK_CFG_APB0_CLK_DIV_SHIFT);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_PERIPHERAL_T enPer, bool bEnable)
{
    if (bEnable)
    {
        SYS_CTRL->AHB_CLK_CFG |= enPer & SYSCTRL_AHB_CLK_EN_REG_MASK;
        SYS_CTRL->APB0_CLK_CFG |= (enPer >> SYSCTRL_AHB_CLK_EN_REG_LEN) & SYSCTRL_APB0_CLK_CFG_REG_MASK;
        SYS_CTRL->LP_CLK_CFG |= (enPer >> (SYSCTRL_AHB_CLK_EN_REG_LEN + SYSCTRL_APB0_CLK_CFG_REG_LEN)) & SYSCTRL_LP_CLK_CFG_REG_MASK;
    }
    else
    {
        SYS_CTRL->AHB_CLK_CFG &= ~(enPer & SYSCTRL_AHB_CLK_EN_REG_MASK);
        SYS_CTRL->APB0_CLK_CFG &= (~(enPer >> SYSCTRL_AHB_CLK_EN_REG_LEN)) & SYSCTRL_APB0_CLK_CFG_REG_MASK;
        SYS_CTRL->LP_CLK_CFG &= (~(enPer >> (SYSCTRL_AHB_CLK_EN_REG_LEN + SYSCTRL_APB0_CLK_CFG_REG_LEN)) & SYSCTRL_LP_CLK_CFG_REG_MASK);
    }

    if (enPer & (EN_SYSCTRL_LPTIM0 | EN_SYSCTRL_LPTIM1 | EN_SYSCTRL_LPUART))
        rom_utility_delay_us(150);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_PERIPHERAL_T enPer)
{
    SYS_CTRL->AHB_SW_RST = enPer & SYSCTRL_AHB_SW_RST_REG_MASK;
    SYS_CTRL->APB0_SW_RST = (enPer >> SYSCTRL_AHB_SW_RST_LEN) & SYSCTRL_APB0_SW_RST_REG_MASK;
    SYS_CTRL->LP_RST_CFG = (enPer >> (SYSCTRL_AHB_SW_RST_LEN + SYSCTRL_APB0_SW_RST_REG_LEN)) & SYSCTRL_LP_RST_CFG_REG_MASK;

    // Soft clean after reset
    SYS_CTRL->AHB_SW_RST &= ~(enPer)&SYSCTRL_AHB_SW_RST_REG_MASK;
    SYS_CTRL->APB0_SW_RST &= ~(enPer >> SYSCTRL_AHB_SW_RST_LEN) & SYSCTRL_APB0_SW_RST_REG_MASK;
    SYS_CTRL->LP_RST_CFG &= ~(enPer >> (SYSCTRL_AHB_SW_RST_LEN + SYSCTRL_APB0_SW_RST_REG_LEN)) & SYSCTRL_LP_RST_CFG_REG_MASK;

    if (enPer & (EN_SYSCTRL_LPTIM0 | EN_SYSCTRL_LPTIM1 | EN_SYSCTRL_LPUART))
        rom_utility_delay_us(150);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_set_lp_clock(EN_SYSCTRL_LP_CLK_T enClk)
{
    switch (enClk)
    {
    case EN_LP_SYS_CLK_RC32K:
    {
        SYS_CTRL->LP_CLK_SEL = 0;
        break;
    }

    case EN_LP_SYS_CLK_DCXO32K:
    {
        SYS_CTRL->LP_CLK_SEL = (SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_SHIFT) |
                               (SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_SHIFT);
        break;
    }

    case EN_LP_SYS_CLK_APB1_RC32K_RTC_TCXO32K:
    {
        SYS_CTRL->LP_CLK_SEL = (((~SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK) & SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK) << SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_SHIFT) |
                               (SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_SHIFT);
        break;
    }

    case EN_LP_SYS_CLK_RTC_RC32K_APB1_TCXO32K:
    {
        SYS_CTRL->LP_CLK_SEL = (SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_MASK << SYSCTRL_LP_CLK_SEL_APB1_CLK_SEL_SHIFT) |
                               (((~SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK) & SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_MASK) << SYSCTRL_LP_CLK_SEL_RTC_CLK_SEL_SHIFT);
        break;
    }

    default:
        return EN_ERROR_STA_INVALID;
        break;
    }
    return EN_ERROR_STA_OK;
}

//------------------------------------------------------------------------------
// Cache setting & functions
//------------------------------------------------------------------------------

EN_ERR_STA_T rom_hw_sysctrl_set_cache_mode(EN_SYSCTRL_CACHE_MODE_T enMode)
{
    switch (enMode)
    {
        case EN_CACHE_ENABLE:
        {
            SYS_CTRL->CACHE_CTRL = 0;
            break;
        }

        case EN_CACHE_DISABLE:
        {
            SYS_CTRL->CACHE_CTRL = SYSCTRL_CACHE_CTRL_BYPASS_EN_MASK << SYSCTRL_CACHE_CTRL_BYPASS_EN_SHIFT;
            break;
        }

        case EN_CACHE_FLUSH:
        {
            // Flush cache, need write 1 then write 0 to this bit
            SYS_CTRL->CACHE_CTRL |= SYSCTRL_CACHE_CTRL_FLUSH_EN_MASK << SYSCTRL_CACHE_CTRL_FLUSH_EN_SHIFT;
            SYS_CTRL->CACHE_CTRL &= ~(SYSCTRL_CACHE_CTRL_FLUSH_EN_MASK << SYSCTRL_CACHE_CTRL_FLUSH_EN_SHIFT);
            break;
        }

        default:
            return EN_ERROR_STA_INVALID;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_get_cache_total_req(uint32_t *pu32Buffer)
{
    if (NULL == pu32Buffer)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Buffer = SYS_CTRL->CACHE_TOTAL_REQ;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_get_cache_hit_counter(uint32_t *pu32Buffer)
{
    if (NULL == pu32Buffer)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Buffer = SYS_CTRL->CACHE_HIT_CNT;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_get_prefetch_counter(uint32_t *pu32Buffer)
{
    if (NULL == pu32Buffer)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Buffer = SYS_CTRL->CACHE_PREFETCH_CNT;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_get_down_req(uint32_t *pu32Buffer)
{
    if (NULL == pu32Buffer)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Buffer = SYS_CTRL->CACHE_DOWN_REQ;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_cdc_delay(uint32_t u32Delay)
{
    uint32_t clk32k_cnt = 0;
    for (clk32k_cnt = 0; clk32k_cnt < u32Delay; clk32k_cnt++) {
        SYS_CTRL->LP_CDC_ICR = SYSCTRL_LP_CDC_ICR_CDC_INT_EN_MASK << SYSCTRL_LP_CDC_ICR_CDC_INT_EN_SHIFT;
        while ((SYS_CTRL->LP_CDC_ICR & (SYSCTRL_LP_CDC_ICR_CDC_ISR_MASK << SYSCTRL_LP_CDC_ICR_CDC_ISR_SHIFT)) == 0);
        SYS_CTRL->LP_CDC_ICR = (SYSCTRL_LP_CDC_ICR_CDC_ISR_MASK << SYSCTRL_LP_CDC_ICR_CDC_ISR_SHIFT);
    }

    return EN_ERROR_STA_OK;
}

void rom_hw_sysctrl_set_lp_clk(bool xtal)
{
    SYS_CTRL->LP_CLK_SEL &= ~(SYSCTRL_APB1_LP_CLK_MASK | SYSCTRL_PHY_PMU_LP_CLK_MASK);
    if (xtal) {
        SYS_CTRL->LP_CLK_SEL |= (SYSCTRL_APB1_LP_CLK_VAL(1) | SYSCTRL_PHY_PMU_LP_CLK_VAL(1));
    }
}

void rom_hw_sysctrl_set_phy_fft_clk(bool xtal)
{
    SYS_CTRL->LP_CLK_SEL &= ~SYSCTRL_PHY_FFT_CLK_MASK;
    if (xtal) {
        SYS_CTRL->LP_CLK_SEL |= SYSCTRL_PHY_FFT_CLK_VAL(1);
    }
}

void rom_hw_sysctrl_reset_phy(void)
{
    SYS_CTRL->PHY_SW_RST = 1;
    rom_utility_delay_us(100);
    SYS_CTRL->PHY_SW_RST = 0;
}

void rom_hw_sysctrl_reset_rffe(void)
{
    SYS_CTRL->PHY_SW_RST = 0x2;
    rom_utility_delay_us(100);
    SYS_CTRL->PHY_SW_RST = 0;
}

uint8_t rom_hw_sysctrl_get_reset_src(void)
{
    return (uint8_t)(SYS_CTRL->RST_SRC & 0x7);
}

volatile uint8_t g_boot_rst_src __ROM_RAM_SHARED = 0;
EN_ERR_STA_T rom_hw_sysctrl_set_boot_rst_flag(uint8_t rst_flag)
{
    g_boot_rst_src = rst_flag;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_sysctrl_get_boot_rst_flag(uint8_t *rst_flag)
{
    *rst_flag = g_boot_rst_src;
    return EN_ERROR_STA_OK;
}
