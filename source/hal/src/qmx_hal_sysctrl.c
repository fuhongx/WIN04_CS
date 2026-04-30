/**
 * ****************************************************************************
 * @file qmx_hal_sysctrl.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "utility.h"
#include "qmx_hal_sysctrl.h"
#include "reg_Dev.h"
#include "hw_sysctrl.h"

void qmx_hal_sysctrl_system_clock_init(hal_sysctrl_src_e clk_src, hal_sysctrl_div_e clk_div)
{
    if ((clk_src >= HAL_SYSCLK_MAX) || (clk_div >= HAL_SYSCLK_DIV_MAX))
        return;

    rom_hw_sysctrl_system_clock_init((EN_SYSCTRL_SYS_CLK_T)clk_src,
        (clk_div == HAL_SYSCLK_DIV_NONE) ? EN_SYS_CLK_MCLK_DIV_NONE : (EN_SYSCTRL_SYS_CLK_DIV_T)(clk_div + 7));
}

void qmx_hal_sysctrl_peripheral_clk_enable(hal_peripheral_clk_e peripheral, bool enable)
{
    rom_hw_sysctrl_enable_clock_gate((EN_SYSCTRL_PERIPHERAL_T)BIT(peripheral), enable);
}

void qmx_hal_sysctrl_peripheral_mod_reset(hal_peripheral_clk_e peripheral)
{
    rom_hw_sysctrl_reset_peripheral((EN_SYSCTRL_PERIPHERAL_T)BIT(peripheral));
}

void qmx_hal_sysctrl_cache_mode_set(hal_cache_mode_e mode)
{
    rom_hw_sysctrl_set_cache_mode((EN_SYSCTRL_CACHE_MODE_T)mode);
}

uint32_t qmx_hal_sysctrl_get_system_clock(void)
{
    return SystemCoreClock;
}

void qmx_hal_sysctrl_disable_ahb_apb(void)
{
    rom_hw_sysctrl_enable_clock_gate((EN_SYSCTRL_PERIPHERAL_T)0x7FFFF, false);
}

void qmx_hal_sysctrl_set_lp_clk(hal_sysctrl_lp_src_e clk_src)
{
    if (clk_src >= HAL_SYSCLK_LP_MAX)
        return;

    rom_hw_sysctrl_set_lp_clk((clk_src == HAL_SYSCLK_LP_XTAL32K) ? true : false);
}

void qmx_hal_sysctrl_reset_phy(void)
{
    rom_hw_sysctrl_reset_phy();
}

uint8_t qmx_hal_sysctrl_get_reset_src(void)
{
    return rom_hw_sysctrl_get_reset_src();
}

uint8_t qmx_hal_sysctrl_sw_get_reset_src(void)
{
    uint8_t reset_src = 0;
    rom_hw_sysctrl_get_boot_rst_flag(&reset_src);
    return reset_src;
}
