/**
 * ****************************************************************************
 * @file qmx_hal_sysctrl.h
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __QMX_HAL_SYSCTRL_H__
#define __QMX_HAL_SYSCTRL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define HAL_SYS_CLK_FREQ_50M    (50000000U)
#define HAL_SYS_CLK_FREQ_25M    (25000000U)
#define HAL_SYS_CLK_FREQ_32K    (32768U)

typedef enum
{
    HAL_SYSCLK_RC50M = 0,
    HAL_SYSCLK_TCXO25M,

    HAL_SYSCLK_MAX,
} hal_sysctrl_src_e;

typedef enum
{
    HAL_SYSCLK_LP_RC32K = 0,
    HAL_SYSCLK_LP_XTAL32K,

    HAL_SYSCLK_LP_MAX,
} hal_sysctrl_lp_src_e;

typedef enum
{
    HAL_SYSCLK_DIV_NONE = 0,
    HAL_SYSCLK_DIV2,
    HAL_SYSCLK_DIV4,
    HAL_SYSCLK_DIV8,
    HAL_SYSCLK_DIV16,
    HAL_SYSCLK_DIV32,
    HAL_SYSCLK_DIV64,

    HAL_SYSCLK_DIV_MAX,
} hal_sysctrl_div_e;

typedef enum
{
    HAL_CLK_DMA = 0,
    HAL_CLK_RAND,
    HAL_CLK_CRC16,
    HAL_CLK_CRC24,
    HAL_CLK_CRC32,
    HAL_CLK_AES128,
    HAL_CLK_GPIO,
    HAL_CLK_UART0,
    HAL_CLK_UART1,

    HAL_CLK_SPI0 = 10,
    HAL_CLK_SPI1,
    HAL_CLK_TIM0,
    HAL_CLK_TIM1,
    HAL_CLK_TIM2,
    HAL_CLK_TIM3,
    HAL_CLK_I2C0,
    HAL_CLK_I2C1,
    HAL_CLK_WDT,
    HAL_CLK_LPTIM0,
    HAL_CLK_LPTIM1,
    HAL_CLK_LPUART,
} hal_peripheral_clk_e;

typedef enum
{
    HAL_CACHE_ENABLE = 0,
    HAL_CACHE_DISABLE = 1,
    HAL_CACHE_FLUSH = 2,
} hal_cache_mode_e;

/**
 * @brief System clock source, 系统时钟选择和分频配置
 * 
 * @param clk_src 系统时钟源选择，参考 hal_sysctrl_src_e 枚举
 * @param clk_div 系统分频系数，参考 hal_sysctrl_div_e 枚举，最大仅支持64分频
 */
void qmx_hal_sysctrl_system_clock_init(hal_sysctrl_src_e clk_src, hal_sysctrl_div_e clk_div);

/**
 * @brief Get system clock, 获取系统时钟
 * 
 * @return uint32_t 系统时钟频率
 */
uint32_t qmx_hal_sysctrl_get_system_clock(void);

/**
 * @brief Enable or disable peripheral clock, 使能或禁用外设时钟
 * 
 * @param peripheral 外设选择，参考 hal_peripheral_clk_e 枚举
 * @param enable     是否使能，true为使能，false为禁用
 */
void qmx_hal_sysctrl_peripheral_clk_enable(hal_peripheral_clk_e peripheral, bool enable);

/**
 * @brief Reset peripheral, 复位外设, 功能和寄存器值都会被复位至初始状态
 * 
 * @param peripheral 外设选择，参考 hal_peripheral_clk_e 枚举
 */
void qmx_hal_sysctrl_peripheral_mod_reset(hal_peripheral_clk_e peripheral);

/**
 * @brief Set cache mode, 设置缓存模式
 * 
 * @param mode 缓存模式，参考 hal_cache_mode_e 枚举, enable为使能，disable为禁用, flush为刷新cache
 */
void qmx_hal_sysctrl_cache_mode_set(hal_cache_mode_e mode);

/**
 * @brief Set low power clock, 设置低功耗时钟源
 * 
 * @param clk_src 低功耗时钟源选择，参考 hal_sysctrl_lp_src_e 枚举
 */
void qmx_hal_sysctrl_set_lp_clk(hal_sysctrl_lp_src_e clk_src);

void qmx_hal_sysctrl_reset_phy(void);

void qmx_hal_sysctrl_disable_ahb_apb(void);

#ifdef __cplusplus
}
#endif

#endif /* __QMX_HAL_SYSCTRL_H__ */
