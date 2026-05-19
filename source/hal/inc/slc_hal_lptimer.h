/**
 * ****************************************************************************
 * @file slc_hal_lptimer.h
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __SLC_HAL_LPTIMER_H__
#define __SLC_HAL_LPTIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_LPTIMER0 = 0,
    HAL_LPTIMER1,

    HAL_LPTIMER_MAX,
} hal_lptimer_id_e;

/**
 * @brief 暂停lptimer, lptimer停止计数
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 */
void slc_hal_lptimer_stop(hal_lptimer_id_e lptimer_id);

/**
 * @brief 启动lptimer, lptimer开始计数
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 */
void slc_hal_lptimer_start(hal_lptimer_id_e lptimer_id);

/**
 * @brief 获取lptimer当前计数值
 * 
 * @param lptimer_id  timer的序号，具体见hal_lptimer_id_e
 * @return uint32_t 返回值为当前计数值
 */
uint32_t slc_hal_lptimer_get_count(hal_lptimer_id_e lptimer_id);

/**
 * @brief 初始化lptimer
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 * @param count      lptimer的重载计数周期，即定时周期，计数单位为32K晶振周期
 * @param enable_irq 是否使能lptimer中断；true为使能，false为不使能
 */
void slc_hal_lptimer_init(hal_lptimer_id_e lptimer_id, uint32_t count, bool enable_irq);

/**
 * @brief 使能lptimer中断
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 */
void slc_hal_lptimer_enable_irq(hal_lptimer_id_e lptimer_id);

/**
 * @brief 禁用lptimer中断
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 */
void slc_hal_lptimer_disable_irq(hal_lptimer_id_e lptimer_id);

/**
 * @brief 获取lptimer中断状态
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 * @return uint32_t lptimer中断状态，0表示无中断，1表示有中断
 */
uint32_t slc_hal_lptimer_get_irq_status(hal_lptimer_id_e lptimer_id);

/**
 * @brief 清除lptimer中断状态
 * 
 * @param lptimer_id lptimer的序号，具体见hal_lptimer_id_e
 * @param irq_status lptimer中断状态
 */
void slc_hal_lptimer_clear_irq(hal_lptimer_id_e lptimer_id, uint32_t irq_status);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_LPTIMER_H__ */
