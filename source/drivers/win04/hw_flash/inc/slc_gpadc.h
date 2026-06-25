/**
 * ****************************************************************************
 * @file qmx_hal_gpadc.h
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
 
#ifndef __QMX_HAL_GPADC_H__
#define __QMX_HAL_GPADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include "qmx_hal_gpio.h"


// ADC采集通道枚举
typedef enum {
    ADC_CH_VSENSOR,    // Vsensor 通道
    ADC_CH_TSENSOR,    // Tsensor 通道
    ADC_CH_TXPDT,      // TXPDT 通道
    ADC_CH_GPIO5,      // GPIO5 单端
    ADC_CH_GPIO6,      // GPIO6 单端
    ADC_CH_GPIO7,      // GPIO7 单端
    ADC_CH_GPIO8,      // GPIO8 单端
    ADC_CH_GPIO6_GPIO5,// GPIO6/GPIO5 差分
    ADC_CH_GPIO8_GPIO7 // GPIO8/GPIO7 差分
} ADC_ChannelTypeDef;

//#define GPADC_CTRL_REG    (*(volatile unsigned int *)0x40000000)

// 位域定义（严格匹配你的表格）
#define GPADC_BUFFER_SEL_Pos   4    //  主通道选择
#define GPADC_BUFFER_SEL_Msk   (0x3U << GPADC_BUFFER_SEL_Pos)

#define GPADC_VIP_SEL2_Pos     10    //  差分/单端选择
#define GPADC_VIP_SEL2_Msk     (0x1U << GPADC_VIP_SEL2_Pos)

#define GPADC_VIP_SEL_Pos      8    //  GPIO子通道选择
#define GPADC_VIP_SEL_Msk     (0x3U << GPADC_VIP_SEL_Pos)

#define GPADC_EN_Pos            4   //GPADC 使能寄存器 0x100 [4]
#define GPADC_EN_Msk            (0x1U << GPADC_EN_Pos)

void qmx_adc_init(ADC_ChannelTypeDef ch);
void adc_gpio_init(hal_gpio_pin_e pin);
void gpadc_test(void);




#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_GPADC_H__ */
