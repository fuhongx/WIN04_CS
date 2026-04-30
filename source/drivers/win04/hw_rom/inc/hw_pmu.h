/**
 * ****************************************************************************
 * @file hw_pmu.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED. 
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 * 
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __HW_PMU_H__
#define __HW_PMU_H__

#define QMX_PMU_FORCE_FLASH_ON_SHIFT    (0U)
#define QMX_PMU_FORCE_FLASH_ON_MASK     (1U << QMX_PMU_FORCE_FLASH_ON_SHIFT)
#define QMX_PMU_FORCE_FLASH_ON_VAL(x)   (((x) << QMX_PMU_FORCE_FLASH_ON_SHIFT) & QMX_PMU_FORCE_FLASH_ON_MASK)
#define QMX_PMU_RF_LP_BYPASS_SHIFT      (1U)
#define QMX_PMU_RF_LP_BYPASS_MASK       (1U << QMX_PMU_RF_LP_BYPASS_SHIFT)
#define QMX_PMU_RF_LP_BYPASS_VAL(x)     (((x) << QMX_PMU_RF_LP_BYPASS_SHIFT) & QMX_PMU_RF_LP_BYPASS_MASK)

typedef enum
{
    EN_PMU_LP_MODE_NORMAL          =    PMU_LP_MODE_NORMAL,
    EN_PMU_LP_MODE_STOP            =    PMU_LP_MODE_STOP,
    EN_PMU_LP_MODE_STANDBY         =    PMU_LP_MODE_STANDBY,
    EN_PMU_LP_MODE_SHUTDOWN         =   PMU_LP_MODE_SHUTDOWN,
}EN_PMU_LP_MODE_T;

typedef enum
{
    EN_PMU_WAKEUP_SRC_PHY_INT0      =   PMU_WKP_MSK_WAKEUP_PHY_INT0_MASK << PMU_WKP_MSK_WAKEUP_PHY_INT0_SHIFT,
    EN_PMU_WAKEUP_SRC_LPTIMER1      =   PMU_WKP_MSK_WAKEUP_LPTIMER1_MASK << PMU_WKP_MSK_WAKEUP_LPTIMER1_SHIFT,
    EN_PMU_WAKEUP_SRC_RTC           =   PMU_WKP_MSK_WAKEUP_RTC_MASK << PMU_WKP_MSK_WAKEUP_RTC_SHIFT,
    EN_PMU_WAKEUP_SRC_LPTIMER0      =   PMU_WKP_MSK_WAKEUP_LPTIMER0_MASK << PMU_WKP_MSK_WAKEUP_LPTIMER0_SHIFT,
    EN_PMU_WAKEUP_SRC_LPUART        =   PMU_WKP_MSK_WAKEUP_LPUART_MASK << PMU_WKP_MSK_WAKEUP_LPUART_SHIFT,
    EN_PMU_WAKEUP_SRC_IWDT          =   PMU_WKP_MSK_WAKEUP_IWDT_MASK << PMU_WKP_MSK_WAKEUP_IWDT_SHIFT,
    EN_PMU_WAKEUP_SRC_GPIO          =   PMU_WKP_MSK_WAKEUP_LPIO_MASK << PMU_WKP_MSK_WAKEUP_LPIO_SHIFT,
    EN_PMU_WAKEUP_SRC_RF            =   PMU_WKP_MSK_WAKEUP_RF_MASK << PMU_WKP_MSK_WAKEUP_RF_SHIFT,
}EN_PMU_WAKEUP_SRC_T;

EN_ERR_STA_T rom_hw_pmu_set_lp_mode(EN_PMU_LP_MODE_T enMode);
EN_ERR_STA_T rom_hw_pmu_enable_phy_power(bool bEnable);
EN_ERR_STA_T rom_hw_pmu_get_phy_power_status(uint8_t *pu8Sta);
EN_ERR_STA_T rom_hw_pmu_set_wakeup_source(uint8_t u8Mask);
EN_ERR_STA_T rom_hw_pmu_get_stop_wakeup_flag(uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_pmu_enable_rf_powerdown(bool bEnable);
EN_ERR_STA_T rom_hw_pmu_enable_flash_in_standby(bool bEnable);
EN_ERR_STA_T rom_hw_pmu_get_flash_pd_status(uint8_t *flash_force_on);
EN_ERR_STA_T rom_hw_pmu_enable_lpio(bool bEnable);
EN_ERR_STA_T rom_hw_pmu_get_lpio_status(uint8_t *lpio_en);
EN_ERR_STA_T rom_hw_pmu_set_boot_lpwr_flag(uint8_t lpwr_flag);
EN_ERR_STA_T rom_hw_pmu_get_boot_lpwr_flag(uint8_t *lpwr_flag);

void rom_hw_pmu_force_flash_on(bool enable);
void rom_hw_pmu_rf_lp_bypass(bool enable);

#endif
