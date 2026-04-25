/**
 * ****************************************************************************
 * @file reg_WDT.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */


#ifndef __REG_WDT_H__
#define __REG_WDT_H__

typedef struct
{
    __IOM uint32_t WDT_WPR; // 0x00
    __IOM uint32_t WDT_SCR; // 0x04
    __IOM uint32_t WDT_LDR; // 0x08
    __IOM uint32_t WDT_CDR; // 0x0C
    __IOM uint32_t WDT_IER; // 0x10
    __IOM uint32_t WDT_IMR; // 0x14
    __IOM uint32_t WDT_ISR; // 0x18
    __IOM uint32_t WDT_TSR; // 0x1C
    __IOM uint32_t WDT_FDR; // 0x20
    __IOM uint32_t IWDT_DLL;    // 0x24
    __IOM uint32_t IWDT_TER;    // 0x28
}stWdtHandle_t, *pstWdtHandle_t;

//------------------------------------------------------------------------------
//WDT_WPR
//------------------------------------------------------------------------------
#define WDT_WRP_REG_MASK                (0x1)

//------------------------------------------------------------------------------
//WDT_SCR
//------------------------------------------------------------------------------
//BIT[0]
#define WDT_SCR_RST_EN_MASK             (0x1)
#define WDT_SCR_RST_EN_SHIFT            (0)

//BIT[1]
#define WDT_SCR_FEED_MODE_MASK          (0x1)
#define WDT_SCR_FEED_MODE_SHIFT         (1)

#define WDT_SCR_REG_MASK            ((WDT_SCR_RST_EN_MASK << WDT_SCR_RST_EN_SHIFT)|\
                                    (WDT_SCR_FEED_MODE_MASK << WDT_SCR_FEED_MODE_SHIFT))

//------------------------------------------------------------------------------
//WDT_LDR
//------------------------------------------------------------------------------
//BIT[0:1]
#define WDT_LDR_REG_MASK                (0x3)

//------------------------------------------------------------------------------
//WDT_CDR
//------------------------------------------------------------------------------
#define WDT_CDR_REG_MASK                (0xFFFFFFFF)

//------------------------------------------------------------------------------
//WDT_IER
//------------------------------------------------------------------------------
//BIT[0]
#define WDT_IER_INT_MATCH_EN_MASK       (0x1)
#define WDT_IER_INT_MATCH_EN_SHIFT      (0)

//BIT[1]
#define WDT_IER_INT_WP_EN_MASK          (0x1)
#define WDT_IER_INT_WP_EN_SHIFT         (1)

#define WDT_IER_REG_MASK                ((WDT_IER_INT_MATCH_EN_MASK << WDT_IER_INT_MATCH_EN_SHIFT)|\
                                        (WDT_IER_INT_WP_EN_MASK << WDT_IER_INT_WP_EN_SHIFT))

//------------------------------------------------------------------------------
//WDT_IMR
//------------------------------------------------------------------------------

#define WDT_IMR_REG_MASK                (WDT_IER_REG_MASK)

//------------------------------------------------------------------------------
//WDT_ISR
//------------------------------------------------------------------------------
#define WDT_ISR_REG_MASK                (WDT_IER_REG_MASK)

//------------------------------------------------------------------------------
//WDT_TSR
//------------------------------------------------------------------------------
//BIT[0]
#define WDT_TSR_WP_MASK                 (0x1)
#define WDT_TSR_WP_SHIFT                (0)

//BIT[1]
#define WDT_TSR_OVERFLOW_MASK           (0x1)
#define WDT_TSR_OVERFLOW_SHIFT          (1)

#define WDT_TSR_REG_MASK                ((WDT_TSR_WP_MASK << WDT_TSR_WP_SHIFT)|\
                                        (WDT_TSR_OVERFLOW_MASK << WDT_TSR_OVERFLOW_SHIFT))

//------------------------------------------------------------------------------
//WDT_FDR
//------------------------------------------------------------------------------
#define IWDT_FDR_WRITE_VAL              (0xAAAA5555)

#define WDT_FDR_REG_MASK                (0xFFFFFFFF)

//------------------------------------------------------------------------------
//IWDT_DLL
//------------------------------------------------------------------------------
#define IWDT_DLL_REG_MASK               (0xFFFFFFFF)


//------------------------------------------------------------------------------
//IWDT_TER
//------------------------------------------------------------------------------
#define IWDT_TER_REG_MASK               (0x1)



#endif

