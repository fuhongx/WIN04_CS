/**
 * ****************************************************************************
 * @file qmx_hal_pmu.c
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
#include "qmx_hal_pmu.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "compiler.h"
#include "hw_pmu.h"

void qmx_hal_pmu_set_lp_mode(hal_pmu_lp_mode lp_mode)
{
    EN_PMU_LP_MODE_T lp_mode_temp = EN_PMU_LP_MODE_NORMAL;

    switch (lp_mode)
    {
        case HAL_PMU_LP_MODE_NORMAL:
        case HAL_PMU_LP_MODE_SLEEP:
            lp_mode_temp = EN_PMU_LP_MODE_NORMAL;
            break;
        case HAL_PMU_LP_MODE_STOP:
            lp_mode_temp = EN_PMU_LP_MODE_STOP;
            break;
        case HAL_PMU_LP_MODE_STANDBY:
            lp_mode_temp = EN_PMU_LP_MODE_STANDBY;
            break;
        case HAL_PMU_LP_MODE_SHUTDOWN:
            lp_mode_temp = EN_PMU_LP_MODE_SHUTDOWN;
            break;
        default:
            break;
    }

    rom_hw_pmu_set_lp_mode(lp_mode_temp);
}

void qmx_hal_pmu_set_lp_wakeup_source(uint32_t lp_wakeup_src_msk)
{
    rom_hw_pmu_set_wakeup_source(lp_wakeup_src_msk);
}

uint32_t qmx_hal_pmu_get_wakeup_src_sta(void)
{
    uint8_t lp_sta = 0;
    rom_hw_pmu_get_stop_wakeup_flag(&lp_sta);
    return lp_sta;
}

void qmx_hal_pmu_force_flash_on(bool enable)
{
    rom_hw_pmu_force_flash_on(enable);
}

int qmx_hal_pmu_phy_power_enable(bool enable)
{
    return rom_hw_pmu_enable_phy_power(enable);
}

void qmx_hal_pmu_rf_lp_bypass(bool enable)
{
    rom_hw_pmu_rf_lp_bypass(enable);
}

void qmx_hal_pmu_powerdown(void)
{
    rom_hw_pmu_enable_rf_powerdown(true);
}

void qmx_hal_pmu_lpio_enable(bool enable)
{
    rom_hw_pmu_enable_lpio(enable);
}

void qmx_hal_pmu_set_boot_flag(uint8_t boot_flag)
{
    rom_hw_pmu_set_boot_lpwr_flag(boot_flag);
}

uint8_t qmx_hal_pmu_get_boot_flag(void)
{
    uint8_t flag;
    rom_hw_pmu_get_boot_lpwr_flag(&flag);
    return flag;
}
