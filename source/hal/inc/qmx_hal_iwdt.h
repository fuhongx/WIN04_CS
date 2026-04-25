/**
 * ****************************************************************************
 * @file qmx_hal_iwdt.h
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

#ifndef __QMX_HAL_IWDT_H__
#define __QMX_HAL_IWDT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define HAL_IWDT_INT_TIMEOUT_MASK           (1U << 0)
#define HAL_IWDT_INT_WRITE_PROTECT_MASK     (1U << 1)

typedef enum
{
    HAL_IWDT_MODE_NO_RST = 0,           /// 无复位
    HAL_IWDT_MODE_RST,                  /// 复位
} hal_iwdt_rst_mode_e;

typedef enum
{
    HAL_IWDT_FEED_BY_REG = 0,           /// 通过寄存器喂狗
    HAL_IWDT_FEED_BY_INT,               /// 通过中断喂狗
} hal_iwdt_feed_mode_e;

typedef struct
{
    hal_iwdt_rst_mode_e rst_mode;       /// 复位模式
    hal_iwdt_feed_mode_e feed_mode;     /// 喂狗方式
    uint32_t clk_div;                   /// 时钟分频, 预分频周期=RC时钟周期*(clk_div +1)
    uint32_t timeout_cnt;               /// 超时时间
} hal_iwdt_cfg_t;


/**
 * @brief IWDT初始化，配置IWDT工作模式，超时时间等
 * 
 * @param cfg 详细配置参数见hal_iwdt_cfg_t
 */
void qmx_hal_iwdt_init(hal_iwdt_cfg_t *cfg);

/**
 * @brief IWDT去初始化，关闭IWDT
 * 
 */
void qmx_hal_iwdt_deinit(void);

/**
 * @brief IWDT喂狗(写寄存器喂狗)
 * 
 */
void qmx_hal_iwdt_feed(void);

/**
 * @brief 清除IWDT中断
 * 
 * @param int_mask WDT中断掩码，见HAL_IWDT_INT_TIMEOUT_MASK、HAL_IWDT_INT_WRITE_PROTECT_MASK
 */
void qmx_hal_iwdt_clear_interrupt(uint32_t int_mask);

/**
 * @brief 获取IWDT中断状态
 * 
 * @return uint32_t IWDT中断状态，见HAL_IWDT_INT_TIMEOUT_MASK、HAL_IWDT_INT_WRITE_PROTECT_MASK
 */
uint32_t qmx_hal_iwdt_get_interrupt_status(void);

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_IWDT_H__ */
