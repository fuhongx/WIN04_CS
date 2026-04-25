/**
 * ****************************************************************************
 * @file qmx_hal_intc.h
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

#ifndef __QMX_HAL_INTC_H__
#define __QMX_HAL_INTC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "reg_Dev.h"

typedef void (*hal_irq_handler_t)(void);

/**
 * @brief 中断向量表重定向
 *        1. 将中断向量表从Flash复制到RAM，并重映射到RAM地址。
 *        2. 搭配qmx_register_irq_handler以实现自定义中断处理函数，函数名可自定义
 * @note 该函数必须在系统初始化时调用，确保中断向量表正确设置。
 *       通常在系统启动代码中调用此函数。
 */
void qmx_hal_intc_init(void);

/**
 * @brief 注册中断处理函数
 *
 * @param irq_num 中断号，参考IRQn_Type枚举
 * @param handler 中断处理函数指针，函数名可自定义
 */
void qmx_hal_register_irq_handler(IRQn_Type irq_num, hal_irq_handler_t handler);

/**
 * @brief 注销中断处理函数，此时中断向量为NULL，若再次触发会跑飞
 *
 * @param irq_num 中断号，参考IRQn_Type枚举
 */
void qmx_hal_unregister_irq_handler(IRQn_Type irq_num);

/**
 * @brief 使能中断
 *
 * @param irq_num 中断号，参考IRQn_Type枚举
 * @param priority 中断优先级，支持0-3，数值越小优先级越高
 */
#define QMX_HAL_ENABLE_PERIPHERAL_IRQ(irq_num, priority)    \
    do {                                                    \
        NVIC_SetPriority((irq_num), (priority));            \
        NVIC_EnableIRQ((irq_num));                          \
    } while(0)

#define QMX_HAL_DISABLE_PERIPHERAL_IRQ(irq_num)     \
    do {                                            \
        NVIC_DisableIRQ((irq_num));                 \
    } while(0)

#define QMX_HAL_ENABLE_GLOBAL_IRQ() \
    do {                            \
        __enable_irq();             \
    } while(0)

#define QMX_HAL_DISABLE_GLOBAL_IRQ()    \
    do {                                \
        __disable_irq();                \
    } while(0)

// 中断屏蔽设置，1为屏蔽所有中断，0为不屏蔽
#define QMX_HAL_SET_PRIMASK(state)  \
    do {                            \
        __set_PRIMASK((state));     \
    } while(0)

// 获取当前PRIMASK状态
#define QMX_HAL_GET_PRIMASK()   \
    __get_PRIMASK()

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_INTC_H__ */
