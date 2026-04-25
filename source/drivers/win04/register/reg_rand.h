/**
 * ****************************************************************************
 * @file reg_rand.h
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

#ifndef __REG_RAND_H__
#define __REG_RAND_H__

typedef struct
{
    __IOM uint32_t RAND_CTRL;
    __IOM uint32_t REED_RSH;
    __IOM uint32_t REED_DR;
    __IOM uint32_t RAND_DR;
    __IOM uint32_t RAND_SR;
}stRandHandle_t;

#endif
