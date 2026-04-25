/**
 * ****************************************************************************
 * @file qmx_lpwr_ctrl.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __LPWR_CTRL_H__
#define __LPWR_CTRL_H__

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "qmx_hal_pmu.h"

typedef void (*lp_after_cb)(void);
typedef bool (*lp_before_cb)(void);

typedef struct
{
    uint32_t lp_wakeup_src_msk;         /// 低功耗模式唤醒源配置
    bool phy_power_enable;              /// 低功耗时，PHY电源状态
    bool rf_power_enable;               /// 低功耗时，RF电源状态
    bool flash_force_on;                /// 低功耗时，flash电源状态
    lp_before_cb lp_before_cb_func;     /// 回调函数，在进入低功耗模式前调用
    lp_after_cb lp_after_cb_func;       /// 回调函数，在低功耗模式唤醒后调用
} qmx_lpwr_ctrl_cfg;

/**
 * @brief 低功耗模块初始化
 * 
 * @param lp_cfg 低功耗配置，包括低功耗模式配置、PHY电源状态、RF电源状态、flash电源状态、回调函数等
 */
void qmx_lpwr_ctrl_init(qmx_lpwr_ctrl_cfg *lp_cfg);

/**
 * @brief 进入低功耗模式
 * 
 * @param lp_mode 低功耗模式, 详情见hal_pmu_lp_mode
 */
void qmx_lpwr_ctrl_sleep(hal_pmu_lp_mode lp_mode);

/**
 * @brief 检查是否是从低功耗模式唤醒, 并清除标志, 在主函数开头调用，供standby模式使用
 * 
 * @return uint8_t 0: 正常启动; 1: standby模式唤醒
 */
uint8_t qmx_check_boot_status(void);

#endif
