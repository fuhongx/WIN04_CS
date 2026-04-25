/**
 * ****************************************************************************
 * @file reg_crc.h
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

#ifndef __REG_CRC_H__
#define __REG_CRC_H__

typedef struct
{
    __IOM uint32_t CRC_SCR;     /*!<0x00:*/
    __IOM uint32_t CRC_LEN;     /*!<0x01:*/
    __IOM uint32_t CRC_DEFAULT; /*!<0x02:*/
    __IOM uint32_t CRC_OLOGIC;  /*!<0x03:*/
    __IOM uint32_t CRC_DATA;    /*!<0x04:*/
    __IOM uint32_t CRC_IMR;     /*!<0x05:*/
    __IOM uint32_t CRC_IER;     /*!<0x06:*/
    __IOM uint32_t CRC_CRR;     /*!<0x07:*/
    __IOM uint32_t CRC_ISR;     /*!<0x08:*/

} stCrcHandle_t, *pstCrcHandle_t;

#define CRS_SCR_CRC_EN_MASK                 (0x1)
#define CRC_SCR_CRC_EN_SHIFT                (0)

#define CRC_SCR_DMA_EN_MASK                 (0x1)
#define CRC_SCR_DMA_EN_SHIFT                (1)

#define CRC_SCR_INPUT_DATA_REVERSE_EN_MASK  (0x1)
#define CRC_SCR_INPUT_DATA_REVERSE_EN_SHIFT (2)

#define CRC_SCR_OUTPUT_XOR_EN_MASK          (0x1)
#define CRC_SCR_OUTPUT_XOR_EN_SHIFT         (3)

#define CRC_SCR_OUTPUT_SEQ_MASK             (0x3)
#define CRC_SCR_OUTPUT_SEQ_SHIFT            (4)

#endif
