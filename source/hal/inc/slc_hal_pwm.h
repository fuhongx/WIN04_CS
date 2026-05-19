/**
 * ****************************************************************************
 * @file slc_hal_pwm.h
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

#ifndef __SLC_HAL_PWM_H__
#define __SLC_HAL_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HAL_PWM_IRQ_STA_CH0  (0x01 << 0)
#define HAL_PWM_IRQ_STA_CH1  (0x01 << 1)
#define HAL_PWM_IRQ_STA_CH2  (0x01 << 2)

typedef enum
{
    HAL_PWM_CH0 = 0,
    HAL_PWM_CH1,
    HAL_PWM_CH2,

    HAL_PWM_CH_MAX,
} hal_pwm_ch_e;

typedef enum
{
    HAL_PWM_POL_HIGH = 0,    // 当前计数值大于等于duty时，输出高电平
    HAL_PWM_POL_LOW,         // 当前计数值小于duty时，输出高电平
} hal_pwm_polar_e;

typedef struct
{
    uint32_t period;        // PWM的计时周期cnt值, 单位为系统时钟周期
    uint32_t duty_cycle;    // PWM的占空比cnt值, 单位为系统时钟周期, 占空比=duty/period
    hal_pwm_polar_e polar;  // PWM输出极性
    bool enable;            // 是否使能PWM输出
    bool enable_irq;        // 是否使能定时中断
} hal_pwm_config_t;

/**
 * @brief 初始化PWM定时器, 设置PWM的计时周期cnt值, 占空比cnt值, 输出极性, 输出使能, 中断使能
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 * @param config PWM配置参数，具体见hal_pwm_config_t
 */
void slc_hal_pwm_timer_init(hal_pwm_ch_e pwm_ch, hal_pwm_config_t *config);

/**
 * @brief 启动PWM定时器
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 */
void slc_hal_pwm_timer_start(hal_pwm_ch_e pwm_ch);

/**
 * @brief 停止PWM定时器
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 */
void slc_hal_pwm_timer_stop(hal_pwm_ch_e pwm_ch);

/**
 * @brief 获取PWM定时器的当前计数值
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 * @return uint32_t 当前计数值
 */
uint32_t slc_hal_pwm_timer_get_counter(hal_pwm_ch_e pwm_ch);

/**
 * @brief 使能PWM定时器中断
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 */
void slc_hal_pwm_timer_enable_irq(hal_pwm_ch_e pwm_ch);

/**
 * @brief 禁用PWM定时器中断
 * 
 * @param pwm_ch PWM通道，具体见hal_pwm_ch_e
 */
void slc_hal_pwm_timer_disable_irq(hal_pwm_ch_e pwm_ch);

/**
 * @brief 获取PWM定时器中断状态
 * 
 * @return uint32_t 中断状态，具体见HAL_PWM_IRQ_STA_xx
 */
uint32_t slc_hal_pwm_timer_get_irq_status(void);

/**
 * @brief 清除PWM定时器中断状态
 * 
 * @param status 需要清除的中断状态位，具体见HAL_PWM_IRQ_STA_xx
 */
void slc_hal_pwm_timer_clear_irq(uint32_t status);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_PWM_H__ */
