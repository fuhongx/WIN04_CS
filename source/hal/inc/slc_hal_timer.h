/**
 * ****************************************************************************
 * @file slc_hal_timer.h
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

#ifndef __SLC_HAL_TIMER_H__
#define __SLC_HAL_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_TIMER0 = 0,
    HAL_TIMER1,
    HAL_TIMER2,

    HAL_TIMER_MAX,
} hal_timer_id_e;

/**
 * @brief 暂停timer, timer停止计数
 * 
 * @param timer_id timer的序号，具体见hal_timer_id_e
 */
void slc_hal_timer_stop(hal_timer_id_e timer_id);

/**
 * @brief 启动timer, timer开始计数
 * 
 * @param timer_id timer的序号，具体见hal_timer_id_e
 */
void slc_hal_timer_start(hal_timer_id_e timer_id);

/**
 * @brief 获取timer当前计数值
 * 
 * @param timer_id  timer的序号，具体见hal_timer_id_e
 * @return uint32_t 返回值为当前计数值
 */
uint32_t slc_hal_timer_get_count(hal_timer_id_e timer_id);

/**
 * @brief 初始化timer
 * 
 * @param timer_id   timer的序号，具体见hal_timer_id_e
 * @param count      timer的重载计数周期，即定时周期，计数单位为系统时钟周期
 * @param enable_irq 是否使能timer中断；true为使能，false为不使能
 */
void slc_hal_timer_init(hal_timer_id_e timer_id, uint32_t count, bool enable_irq);

/**
 * @brief 使能timer中断
 * 
 * @param timer_id timer的序号，具体见hal_timer_id_e
 */
void slc_hal_timer_enable_irq(hal_timer_id_e timer_id);

/**
 * @brief 禁用timer中断
 * 
 * @param timer_id timer的序号，具体见hal_timer_id_e
 */
void slc_hal_timer_disable_irq(hal_timer_id_e timer_id);

/**
 * @brief 获取timer中断状态
 * 
 * @param timer_id  timer的序号，具体见hal_timer_id_e
 * @return uint32_t timer中断状态，0表示无中断，1表示有中断
 */
uint32_t slc_hal_timer_get_irq_status(hal_timer_id_e timer_id);

/**
 * @brief 清除timer中断状态
 * 
 * @param timer_id timer的序号，具体见hal_timer_id_e
 * @param irq_status timer中断状态
 */
void slc_hal_timer_clear_irq(hal_timer_id_e timer_id, uint32_t irq_status);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_TIMER_H__ */
