/**
 * ****************************************************************************
 * @file reg_ila.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __REG_ILA_H__
#define __REG_ILA_H__

typedef struct
{
    __IOM uint32_t ILA_SSR;     /*!<0x00:*/
    __IOM uint32_t ILA_SCR;     /*!<0x04:*/
    __IOM uint32_t ILA_TCR;     /*!<0x08:*/
    __IOM uint32_t ILA_MCR;     /*!<0x0C:*/
    __IOM uint32_t ILA_RCR;     /*!<0x10:*/
    __IOM uint32_t ILA_IER;     /*!<0x14:*/
    __IOM uint32_t ILA_ISR;     /*!<0x18:*/
} qmx_ila_reg_t;

#define QMX_ILA_CLK_SEL_SHIFT       (0)
#define QMX_ILA_CLK_SEL_MASK        (0x1U << QMX_ILA_CLK_SEL_SHIFT)
#define QMX_ILA_CLK_SEL_VAL(x)      (((uint32_t)(x) << QMX_ILA_CLK_SEL_SHIFT) & QMX_ILA_CLK_SEL_MASK)
#define QMX_ILA_TRIG_SEL_SHIFT      (3)
#define QMX_ILA_TRIG_SEL_MASK       (0x1FU << QMX_ILA_TRIG_SEL_SHIFT)
#define QMX_ILA_TRIG_SEL_VAL(x)     (((uint32_t)(x) << QMX_ILA_TRIG_SEL_SHIFT) & QMX_ILA_TRIG_SEL_MASK)
#define QMX_ILA_DATA_SEL_SHIFT      (8)
#define QMX_ILA_DATA_SEL_MASK       (0xFU << QMX_ILA_DATA_SEL_SHIFT)
#define QMX_ILA_DATA_SEL_VAL(x)     (((uint32_t)(x) << QMX_ILA_DATA_SEL_SHIFT) & QMX_ILA_DATA_SEL_MASK)

#define QMX_ILA_MEM_ADDR_SHIFT      (0)
#define QMX_ILA_MEM_ADDR_MASK       (0x3FFFFFFFU << QMX_ILA_MEM_ADDR_SHIFT)
#define QMX_ILA_MEM_ADDR_VAL(x)     (((uint32_t)(x) << QMX_ILA_MEM_ADDR_SHIFT) & QMX_ILA_MEM_ADDR_MASK)
#define QMX_ILA_MEM_FORMAT_SHIFT    (30)
#define QMX_ILA_MEM_FORMAT_MASK     (0x1U << QMX_ILA_MEM_FORMAT_SHIFT)
#define QMX_ILA_MEM_FORMAT_VAL(x)   (((uint32_t)(x) << QMX_ILA_MEM_FORMAT_SHIFT) & QMX_ILA_MEM_FORMAT_MASK)

#define QMX_ILA_SAMPLE_NUM_SHIFT    (0)
#define QMX_ILA_SAMPLE_NUM_MASK     (0xFFFFU << QMX_ILA_SAMPLE_NUM_SHIFT)
#define QMX_ILA_SAMPLE_NUM_VAL(x)   (((uint32_t)(x) << QMX_ILA_SAMPLE_NUM_SHIFT) & QMX_ILA_SAMPLE_NUM_MASK)

#define QMX_ILA_TRIG_DELAY_SHIFT    (0)
#define QMX_ILA_TRIG_DELAY_MASK     (0xFFFFU << QMX_ILA_TRIG_DELAY_SHIFT)
#define QMX_ILA_TRIG_DELAY_VAL(x)   (((uint32_t)(x) << QMX_ILA_TRIG_DELAY_SHIFT) & QMX_ILA_SAMPLE_NUM_MASK)
#define QMX_ILA_TRIG_TYPE_SHIFT     (16)
#define QMX_ILA_TRIG_TYPE_MASK      (0x1U << QMX_ILA_TRIG_TYPE_SHIFT)
#define QMX_ILA_TRIG_TYPE_VAL(x)    (((uint32_t)(x) << QMX_ILA_TRIG_TYPE_SHIFT) & QMX_ILA_TRIG_TYPE_MASK)

#define QMX_ILA_INT_EN_SHIFT        (0)
#define QMX_ILA_INT_EN_MASK         (0x1U << QMX_ILA_INT_EN_SHIFT)

#define QMX_ILA_START_RUN_SHIFT     (0)
#define QMX_ILA_START_RUN_MASK      (0x1U << QMX_ILA_START_RUN_SHIFT)

#endif
