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
} slc_ila_reg_t;

#define SLC_ILA_CLK_SEL_SHIFT       (0)
#define SLC_ILA_CLK_SEL_MASK        (0x1U << SLC_ILA_CLK_SEL_SHIFT)
#define SLC_ILA_CLK_SEL_VAL(x)      (((uint32_t)(x) << SLC_ILA_CLK_SEL_SHIFT) & SLC_ILA_CLK_SEL_MASK)
#define SLC_ILA_TRIG_SEL_SHIFT      (3)
#define SLC_ILA_TRIG_SEL_MASK       (0x1FU << SLC_ILA_TRIG_SEL_SHIFT)
#define SLC_ILA_TRIG_SEL_VAL(x)     (((uint32_t)(x) << SLC_ILA_TRIG_SEL_SHIFT) & SLC_ILA_TRIG_SEL_MASK)
#define SLC_ILA_DATA_SEL_SHIFT      (8)
#define SLC_ILA_DATA_SEL_MASK       (0xFU << SLC_ILA_DATA_SEL_SHIFT)
#define SLC_ILA_DATA_SEL_VAL(x)     (((uint32_t)(x) << SLC_ILA_DATA_SEL_SHIFT) & SLC_ILA_DATA_SEL_MASK)

#define SLC_ILA_MEM_ADDR_SHIFT      (0)
#define SLC_ILA_MEM_ADDR_MASK       (0x3FFFFFFFU << SLC_ILA_MEM_ADDR_SHIFT)
#define SLC_ILA_MEM_ADDR_VAL(x)     (((uint32_t)(x) << SLC_ILA_MEM_ADDR_SHIFT) & SLC_ILA_MEM_ADDR_MASK)
#define SLC_ILA_MEM_FORMAT_SHIFT    (30)
#define SLC_ILA_MEM_FORMAT_MASK     (0x1U << SLC_ILA_MEM_FORMAT_SHIFT)
#define SLC_ILA_MEM_FORMAT_VAL(x)   (((uint32_t)(x) << SLC_ILA_MEM_FORMAT_SHIFT) & SLC_ILA_MEM_FORMAT_MASK)

#define SLC_ILA_SAMPLE_NUM_SHIFT    (0)
#define SLC_ILA_SAMPLE_NUM_MASK     (0xFFFFU << SLC_ILA_SAMPLE_NUM_SHIFT)
#define SLC_ILA_SAMPLE_NUM_VAL(x)   (((uint32_t)(x) << SLC_ILA_SAMPLE_NUM_SHIFT) & SLC_ILA_SAMPLE_NUM_MASK)

#define SLC_ILA_TRIG_DELAY_SHIFT    (0)
#define SLC_ILA_TRIG_DELAY_MASK     (0xFFFFU << SLC_ILA_TRIG_DELAY_SHIFT)
#define SLC_ILA_TRIG_DELAY_VAL(x)   (((uint32_t)(x) << SLC_ILA_TRIG_DELAY_SHIFT) & SLC_ILA_SAMPLE_NUM_MASK)
#define SLC_ILA_TRIG_TYPE_SHIFT     (16)
#define SLC_ILA_TRIG_TYPE_MASK      (0x1U << SLC_ILA_TRIG_TYPE_SHIFT)
#define SLC_ILA_TRIG_TYPE_VAL(x)    (((uint32_t)(x) << SLC_ILA_TRIG_TYPE_SHIFT) & SLC_ILA_TRIG_TYPE_MASK)

#define SLC_ILA_INT_EN_SHIFT        (0)
#define SLC_ILA_INT_EN_MASK         (0x1U << SLC_ILA_INT_EN_SHIFT)

#define SLC_ILA_START_RUN_SHIFT     (0)
#define SLC_ILA_START_RUN_MASK      (0x1U << SLC_ILA_START_RUN_SHIFT)

#endif
