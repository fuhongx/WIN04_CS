/**
 * ****************************************************************************
 * @file reg_lpio.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 * 
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */


#ifndef __REG_LPIO_H__
#define __REG_LPIO_H__

typedef struct
{
    __IM uint32_t INPUT_LEVEL;              // 0x00
    __IOM uint32_t OUTPUT_LEVEL;            // 0x04
    __IOM uint32_t OUTPUT_EN;               // 0x08
    __IOM uint32_t INPUT_EN;                // 0x0C
    __IOM uint32_t PULL_EN;                 // 0x10
    __IOM uint32_t IO_MUX_EN;               // 0x14
    __IOM uint32_t LEVEL_INT_SEL;           // 0x18
    __IOM uint32_t INT_EN;                  // 0x1C
    __IOM uint32_t INT_STA;                 // 0x20
}stLpioHandle_t, *pstLpioHandle_t;

#endif
