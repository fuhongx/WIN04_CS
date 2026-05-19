/**
 * ****************************************************************************
 * @file slc_hal_wdt.h
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

#ifndef __SLC_HAL_WDT_H__
#define __SLC_HAL_WDT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HAL_WDT_INT_TIMEOUT_MASK            (1U << 0)
#define HAL_WDT_INT_WRITE_PROTECT_MASK      (1U << 1)

typedef enum
{
    HAL_WDT_MODE_NO_RST = 0,            /// 无复位
    HAL_WDT_MODE_RST,                   /// 复位
} hal_wdt_rst_mode_e;

typedef enum
{
    HAL_WDT_FEED_BY_REG = 0,            /// 通过寄存器喂狗
    HAL_WDT_FEED_BY_INT,                /// 通过中断喂狗
} hal_wdt_feed_mode_e;

typedef struct
{
    hal_wdt_rst_mode_e rst_mode;        /// 复位模式
    hal_wdt_feed_mode_e feed_mode;      /// 喂狗方式
    uint32_t timeout_cnt;               /// 超时时间
} hal_wdt_cfg_t;


/**
 * @brief WDT初始化，配置WDT工作模式，超时时间等
 * 
 * @param cfg 详细配置参数见hal_wdt_cfg_t
 */
void slc_hal_wdt_init(hal_wdt_cfg_t *cfg);

/**
 * @brief WDT去初始化，关闭WDT
 * 
 */
void slc_hal_wdt_deinit(void);

/**
 * @brief WDT喂狗(写寄存器喂狗)
 * 
 */
void slc_hal_wdt_feed(void);

/**
 * @brief 清除WDT中断
 * 
 * @param int_mask WDT中断掩码，见HAL_WDT_INT_TIMEOUT_MASK、HAL_WDT_INT_WRITE_PROTECT_MASK
 */
void slc_hal_wdt_clear_interrupt(uint32_t int_mask);

/**
 * @brief 获取WDT中断状态
 * 
 * @return uint32_t WDT中断状态，见HAL_WDT_INT_TIMEOUT_MASK、HAL_WDT_INT_WRITE_PROTECT_MASK
 */
uint32_t slc_hal_wdt_get_interrupt_status(void);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_WDT_H__ */
