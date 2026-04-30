/**
 * ****************************************************************************
 * @file hw_gpio.c
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
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_sysctrl.h"
#include "hw_pmu.h"
#include "compiler.h"

EN_ERR_STA_T rom_hw_pmu_set_lp_mode(EN_PMU_LP_MODE_T enMode)
{
    PMU->LP_MODE = enMode & PMU_LP_MODE_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_get_phy_power_status(uint8_t *pu8Sta)
{
    if (NULL == pu8Sta)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Sta = PMU->PHY_PWR & PMU_PHY_PWR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_set_wakeup_source(uint8_t u8Mask)
{
    PMU->WKP_MSK = u8Mask & PMU_WKP_MSK_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_get_stop_wakeup_flag(uint8_t *pu8Flag)
{
    if (NULL == pu8Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = PMU->WKP_STA & PMU_WKP_MSK_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_enable_rf_powerdown(bool bEnable)
{
    PMU->PWR_DWN = bEnable & PMU_PWR_DWN_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_enable_flash_in_standby(bool bEnable)
{
    PMU->LP_MAN = bEnable & PMU_LP_MAN_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_get_flash_pd_status(uint8_t *flash_force_on)
{
    *flash_force_on = PMU->LP_MAN & PMU_LP_MAN_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_enable_lpio(bool bEnable)
{
    PMU->LPIO_EN = bEnable & PMU_LPIO_EN_REG_MASK;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_get_lpio_status(uint8_t *lpio_en)
{
    *lpio_en = PMU->LPIO_EN & PMU_LPIO_EN_REG_MASK;
    return EN_ERROR_STA_OK;
}

volatile uint8_t g_boot_lpwr_flag __ROM_RAM_SHARED = 0; // 0: normal boot, else: low power boot
EN_ERR_STA_T rom_hw_pmu_set_boot_lpwr_flag(uint8_t lpwr_flag)
{
    g_boot_lpwr_flag = lpwr_flag;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_get_boot_lpwr_flag(uint8_t *lpwr_flag)
{
    *lpwr_flag = g_boot_lpwr_flag;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_pmu_enable_phy_power(bool bEnable)
{
    uint32_t timeout = 0;

    PMU->PHY_PWR = bEnable & PMU_PHY_PWR_REG_MASK;

    rom_utility_delay_us(LP_RW_DELAY);
    while (PMU->PHY_PWR != bEnable) {
        // 跨时钟域等待
        rom_utility_delay_us(LP_RW_DELAY);
        timeout += LP_RW_DELAY;
        if (timeout >= 50000)
            return EN_ERROR_STA_ERROR;
    }

    return EN_ERROR_STA_OK;
}

void rom_hw_pmu_rf_lp_bypass(bool enable)
{
    if (enable)
        PMU->LP_MAN |= QMX_PMU_RF_LP_BYPASS_VAL(1);
    else
        PMU->LP_MAN &= ~QMX_PMU_RF_LP_BYPASS_MASK;
}

void rom_hw_pmu_force_flash_on(bool enable)
{
    if (enable)
        PMU->LP_MAN |= QMX_PMU_FORCE_FLASH_ON_VAL(1);
    else
        PMU->LP_MAN &= ~QMX_PMU_FORCE_FLASH_ON_MASK;
}
