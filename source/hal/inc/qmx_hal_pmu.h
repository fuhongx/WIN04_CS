/**
 * ****************************************************************************
 * @file qmx_hal_pmu.h
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

#ifndef __QMX_HAL_PMU_H__
#define __QMX_HAL_PMU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_PMU_LP_MODE_NORMAL = 0,

    HAL_PMU_LP_MODE_SLEEP,
    HAL_PMU_LP_MODE_STOP,
    HAL_PMU_LP_MODE_STANDBY,

    HAL_PMU_LP_MODE_SHUTDOWN,
} hal_pmu_lp_mode;

/* stop/standby 模式下唤醒源 */
#define HAL_PMU_LP_WAKEUP_SRC_MSK_PHY0          (1U << 0)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1      (1U << 1)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_RTC           (1U << 2)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0      (1U << 3)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART        (1U << 4)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_IWDT          (1U << 5)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO          (1U << 6)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_RF            (1U << 7)
#define HAL_PMU_LP_WAKEUP_SRC_MSK_ALL           (HAL_PMU_LP_WAKEUP_SRC_MSK_PHY0 | HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER1 | \
                                                 HAL_PMU_LP_WAKEUP_SRC_MSK_RTC | HAL_PMU_LP_WAKEUP_SRC_MSK_LPTIMER0 | \
                                                 HAL_PMU_LP_WAKEUP_SRC_MSK_LPUART | HAL_PMU_LP_WAKEUP_SRC_MSK_IWDT | \
                                                 HAL_PMU_LP_WAKEUP_SRC_MSK_LPIO | HAL_PMU_LP_WAKEUP_SRC_MSK_RF)


/**
 * @brief 设置低功耗模式
 * 
 * @param lp_mode 详情见 hal_pmu_lp_mode 枚举定义
 */
void qmx_hal_pmu_set_lp_mode(hal_pmu_lp_mode lp_mode);

/**
 * @brief 设置PMU 低功耗模式的唤醒源
 * 
 * @param lp_wakeup_src_msk 低功耗唤醒源，参考 HAL_PMU_LP_WAKEUP_SRC_MSK_XXX 宏定义
 */
void qmx_hal_pmu_set_lp_wakeup_source(uint32_t lp_wakeup_src_msk);

/**
 * @brief 获取低功耗模式下的唤醒源状态
 * 
 * @return uint32_t 唤醒源状态，每个bit代表一个唤醒源，详细参考 HAL_PMU_LP_WAKEUP_SRC_MSK_XXX 宏定义
 */
uint32_t qmx_hal_pmu_get_wakeup_src_sta(void);

/**
 * @brief 强制flash 在低功耗时保持正常模式，否则进入深睡模式
 * 
 * @param enable true: 强制flash 在低功耗时保持正常模式，false: 进入深睡模式
 */
void qmx_hal_pmu_force_flash_on(bool enable);

/**
 * @brief 使能 PHY 电源
 * 
 * @param enable true: 使能 PHY 电源，false: 关闭 PHY 电源
 * @return int 0: 成功，其他: 失败
 * @note 使能PHY电源后，PHY寄存器才能读写
 */
int qmx_hal_pmu_phy_power_enable(bool enable);

/**
 * @brief 控制低功耗时RF 的电源状态
 * 
 * @param enable true: 低功耗时RF保持正常上电模式，false: 低功耗时RF进入深睡模式
 */
void qmx_hal_pmu_rf_lp_bypass(bool enable);

/**
 * @brief 系统进入shutdown模式
 * 
 */
void qmx_hal_pmu_powerdown(void);

/**
 * @brief 使能/关闭LPIO 电源(进入低功耗前必须使能LPIO)
 * 
 * @param enable true: 使能LPIO电源，false: 关闭LPIO电源
 */
void qmx_hal_pmu_lpio_enable(bool enable);

/**
 * @brief 设置系统启动标志
 * 
 * @param boot_flag 系统低功耗启动标志，0: 正常启动，1: 低功耗唤醒启动
 */
void qmx_hal_pmu_set_boot_flag(uint8_t boot_flag);

/**
 * @brief 获取系统启动标志
 * 
 * @return uint8_t 0: 正常启动，1: 低功耗唤醒启动
 */
uint8_t qmx_hal_pmu_get_boot_flag(void);

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_PMU_H__ */
