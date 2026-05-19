/**
 * ****************************************************************************
 * @file slc_intc.h
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

#ifndef __SLC_INTC_H__
#define __SLC_INTC_H__

#include "ARMCM0plus.h"

typedef void (*irq_handler_t)(void);

/**
 * @brief 中断向量表重定向
 *        1. 将中断向量表从Flash复制到RAM，并重映射到RAM地址。
 *        2. 搭配slc_register_irq_handler以实现自定义中断处理函数，函数名可自定义
 * @note 该函数必须在系统初始化时调用，确保中断向量表正确设置。
 *       通常在系统启动代码中调用此函数。
 */
void slc_intc_init(void);

/**
 * @brief 注册中断处理函数，如若不注册，默认使用startup_ARMCM0plus.s中定义的弱函数
 * 
 * @param irq_num 中断号，参考IRQn_Type枚举
 * @param handler 中断处理函数指针，函数名可自定义
 */
void slc_register_irq_handler(IRQn_Type irq_num, irq_handler_t handler);

/**
 * @brief 注销中断处理函数，此时中断向量为NULL，若再次触发会跑飞
 * 
 * @param irq_num 中断号，参考IRQn_Type枚举
 */
void slc_unregister_irq_handler(IRQn_Type irq_num);

#endif  /* __SLC_INTC_H__ */
