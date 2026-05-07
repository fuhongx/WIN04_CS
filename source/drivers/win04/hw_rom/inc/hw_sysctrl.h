/**
 * ****************************************************************************
 * @file hw_sysctrl.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_SYSCTRL_H__
#define __HW_SYSCTRL_H__

#include <stdbool.h>
#include "reg_Dev.h"
#include "error_def.h"
#include "compiler.h"

#define SYS_CLK_FREQ_50M    (50000000U)
#define SYS_CLK_FREQ_25M    (25000000U)
#define SYS_CLK_FREQ_32K    (32768U)

#define SYSCTRL_APB1_LP_CLK_SHIFT       (0U)
#define SYSCTRL_APB1_LP_CLK_MASK        (0x1U << SYSCTRL_APB1_LP_CLK_SHIFT)
#define SYSCTRL_APB1_LP_CLK_VAL(x)      (((x) << SYSCTRL_APB1_LP_CLK_SHIFT) & SYSCTRL_APB1_LP_CLK_MASK)

#define SYSCTRL_PHY_FFT_CLK_SHIFT       (1U)
#define SYSCTRL_PHY_FFT_CLK_MASK        (0x1U << SYSCTRL_PHY_FFT_CLK_SHIFT)
#define SYSCTRL_PHY_FFT_CLK_VAL(x)      (((x) << SYSCTRL_PHY_FFT_CLK_SHIFT) & SYSCTRL_PHY_FFT_CLK_MASK)

#define SYSCTRL_PHY_PMU_LP_CLK_SHIFT    (2U)
#define SYSCTRL_PHY_PMU_LP_CLK_MASK     (0x1U << SYSCTRL_PHY_PMU_LP_CLK_SHIFT)
#define SYSCTRL_PHY_PMU_LP_CLK_VAL(x)   (((x) << SYSCTRL_PHY_PMU_LP_CLK_SHIFT) & SYSCTRL_PHY_PMU_LP_CLK_MASK)

extern volatile uint32_t SystemCoreClock;

// 读取寄存器需要至少2个32K时钟周期，跨时钟域的规避
#define LP_RW_DELAY         (((2 * 1000000) / SYS_CLK_FREQ_32K) + 5)

typedef enum
{
    EN_SYS_CLK_RC50M = 0,
    EN_SYS_CLK_TCXO25M = 1,
    EN_SYS_CLK_FDB50M = 2,
    EN_SYS_CLK_MAX,
} EN_SYSCTRL_SYS_CLK_T;

typedef enum
{
    EN_SYS_CLK_MCLK_DIV_NONE = 0,
    EN_SYS_CLK_MCLK_DIV2 = 8,
    EN_SYS_CLK_MCLK_DIV4 = 9,
    EN_SYS_CLK_MCLK_DIV8 = 10,
    EN_SYS_CLK_MCLK_DIV16 = 11,
    EN_SYS_CLK_MCLK_DIV32 = 12,
    EN_SYS_CLK_MCLK_DIV64 = 13,
    EN_SYS_CLK_MCLK_DIV128 = 14,
    EN_SYS_CLK_MCLK_DIV256 = 15,
    EN_SYS_CLK_MCLK_DIV_MAX,
} EN_SYSCTRL_SYS_CLK_DIV_T;

typedef enum
{
    EN_FLASH_CLK_MCLK_DIV_NONE = 0,
    EN_FLASH_CLK_MCLK_DIV2 = 4,
    EN_FLASH_CLK_MCLK_DIV4 = 5,
    EN_FLASH_CLK_MCLK_DIV8 = 6,
    EN_FLASH_CLK_MCLK_DIV16 = 7,
} EN_SYSCTRL_FLASH_CLK_T;

typedef enum
{
    EN_AHB_SYS_CLK_DIV_NONE = 0,
    EN_AHB_SYS_CLK_DIV2 = 4,
    EN_AHB_SYS_CLK_DIV4 = 5,
    EN_AHB_SYS_CLK_DIV8 = 6,
    EN_AHB_SYS_CLK_DIV16 = 7,
} EN_SYSCTRL_AHB_CLK_DIV_T;

typedef enum
{
    EN_APB0_SYS_CLK_DIV_NONE = 0,
    EN_APB0_SYS_CLK_DIV2 = 4,
    EN_APB0_SYS_CLK_DIV4 = 5,
    EN_APB0_SYS_CLK_DIV8 = 6,
    EN_APB0_SYS_CLK_DIV16 = 7,
} EN_SYSCTRL_APB0_CLK_DIV_T;

typedef enum
{
    EN_LP_SYS_CLK_RC32K = 0,
    EN_LP_SYS_CLK_DCXO32K = 1,
    EN_LP_SYS_CLK_APB1_RC32K_RTC_TCXO32K = 2,
    EN_LP_SYS_CLK_RTC_RC32K_APB1_TCXO32K = 3,

} EN_SYSCTRL_LP_CLK_T;

typedef enum
{
    EN_SYSCTRL_DMA =    0x1 << 0,
    EN_SYSCTRL_RAND =   0x1 << 1,
    EN_SYSCTRL_CRC16 =  0x1 << 2,
    EN_SYSCTRL_CRC24 =  0x1 << 3,
    EN_SYSCTRL_CRC32 =  0x1 << 4,
    EN_SYSCTRL_AES128 = 0x1 << 5,
    EN_SYSCTRL_GPIO =   0x1 << 6,
    EN_SYSCTRL_UART0 =  0x1 << 7,
    EN_SYSCTRL_UART1 =  0x1 << 8,
    EN_SYSCTRL_SPI0 =   0x1 << 10,
    EN_SYSCTRL_SPI1 =   0x1 << 11,
    EN_SYSCTRL_TIM0 =   0x1 << 12,
    EN_SYSCTRL_TIM1 =   0x1 << 13,
    EN_SYSCTRL_TIM2 =   0x1 << 14,
    EN_SYSCTRL_TIM3 =   0x1 << 15,
    EN_SYSCTRL_I2C0 =   0x1 << 16,
    EN_SYSCTRL_I2C1 =   0x1 << 17,
    EN_SYSCTRL_WDT =    0x1 << 18,
    EN_SYSCTRL_LPTIM0 = 0x1 << 19,
    EN_SYSCTRL_LPTIM1 = 0x1 << 20,
    EN_SYSCTRL_LPUART = 0x1 << 21,
} EN_SYSCTRL_PERIPHERAL_T;

typedef enum
{
    EN_CACHE_ENABLE = 0,
    EN_CACHE_DISABLE = 1,
    EN_CACHE_FLUSH = 2,
} EN_SYSCTRL_CACHE_MODE_T;

typedef enum
{
    EN_NORMAL_POWERUP   = 0,
    EN_RST_FROM_WDT     = 1,
    EN_RST_FROM_IWDT    = 2,
    EN_RST_FROM_SOFT    = 3, // software reset, NVIC_SystemReset
} EN_SYSCTRL_RST_SRC_T;

/**
 * @brief 初始化系统时钟
 *
 * @param enClk 系统时钟源, see EN_SYSCTRL_SYS_CLK_T for details
 * @param enDiv 系统时钟分频, see EN_SYSCTRL_SYS_CLK_DIV_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_sysctrl_system_clock_init(EN_SYSCTRL_SYS_CLK_T enClk, EN_SYSCTRL_SYS_CLK_DIV_T enDiv);

/**
 * @brief 设置总线时钟
 *
 * @param enAhbDiv AHB总线分频, see EN_SYSCTRL_AHB_CLK_DIV_T for details
 * @param enApbDiv APB总线分频, see EN_SYSCTRL_APB0_CLK_DIV_T for details
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_sysctrl_set_bus_clock(EN_SYSCTRL_AHB_CLK_DIV_T enAhbDiv, EN_SYSCTRL_APB0_CLK_DIV_T enApbDiv);

/**
 * @brief 外设时钟gating使能
 *
 * @param enPer  外设, see EN_SYSCTRL_PERIPHERAL_T for details
 * @param bEnable true: enable, false: disable
 * @return EN_ERR_STA_T 
 */
EN_ERR_STA_T rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_PERIPHERAL_T enPer, bool bEnable);

/**
 * @brief 复位外设时钟
 *
 * @param enPer 外设, see EN_SYSCTRL_PERIPHERAL_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_PERIPHERAL_T enPer);

/**
 * @brief 复位PHY
 *
 */
void rom_hw_sysctrl_reset_phy(void);

void rom_hw_sysctrl_reset_rffe(void);

/**
 * @brief 设置低功耗时钟
 *
 * @param enClk 低功耗时钟源, see EN_SYSCTRL_LP_CLK_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_sysctrl_set_lp_clock(EN_SYSCTRL_LP_CLK_T enClk);

/**
 * @brief 设置cache模式
 * 
 * @param enMode  cache模式, see EN_SYSCTRL_CACHE_MODE_T for details
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_sysctrl_set_cache_mode(EN_SYSCTRL_CACHE_MODE_T enMode);

/**
 * @brief cdc延时, 基于32K时钟, 单次延时为1个32K时钟周期
 * 
 * @param u32Delay 延时周期数，单次为1个32K时钟周期
 */
EN_ERR_STA_T rom_hw_cdc_delay(uint32_t u32Delay);

void rom_hw_sysctrl_set_lp_clk(bool xtal);
void rom_hw_sysctrl_set_phy_fft_clk(bool xtal);

uint8_t rom_hw_sysctrl_get_reset_src(void);

EN_ERR_STA_T rom_hw_sysctrl_set_boot_rst_flag(uint8_t rst_flag);

EN_ERR_STA_T rom_hw_sysctrl_get_boot_rst_flag(uint8_t *rst_flag);

#endif
