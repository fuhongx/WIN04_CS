/**
 * ****************************************************************************
 * @file reg_dma.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __REG_DMA_H__
#define __REG_DMA_H__

/**
 * @brief DMA mode and channle map.
 *
UART1   0   UART1_RX    UART1_RX    UART1_RX    UART1_RX    UART1_RX    UART1_RX    UART1_RX    UART1_RX
        1   UART1_TX    UART1_TX    UART1_TX    UART1_TX    UART1_TX    UART1_TX    UART1_TX    UART1_TX
UART2   2   UART2_RX    UART2_RX    UART2_RX    UART2_RX    UART2_RX    UART2_RX    UART2_RX    UART2_RX
        3   UART2_TX    UART2_TX    UART2_TX    UART2_TX    UART2_TX    UART2_TX    UART2_TX    UART2_TX
UART3   4   UART3_RX    UART3_RX    UART3_RX    UART3_RX    UART3_RX    UART3_RX    UART3_RX    UART3_RX
        5   UART3_TX    UART3_TX    UART3_TX    UART3_TX    UART3_TX    UART3_TX    UART3_TX    UART3_TX
UART4   6   LPUART_RX   LPUART_RX   LPUART_RX   LPUART_RX   LPUART_RX   LPUART_RX   LPUART_RX   LPUART_RX
        7   LPUART_TX   LPUART_TX   LPUART_TX   LPUART_TX   LPUART_TX   LPUART_TX   LPUART_TX   LPUART_TX
SPI1    8   SPI1_RX     SPI1_RX     SPI1_RX     SPI1_RX     SPI1_RX     SPI1_RX     SPI1_RX     SPI1_RX
        9   SPI1_TX     SPI1_TX     SPI1_TX     SPI1_TX     SPI1_TX     SPI1_TX     SPI1_TX     SPI1_TX
SPI2    10  SPI2_RX     SPI2_RX     SPI2_RX     SPI2_RX     SPI2_RX     SPI2_RX     SPI2_RX     SPI2_RX
        11  SPI2_TX     SPI2_TX     SPI2_TX     SPI2_TX     SPI2_TX     SPI2_TX     SPI2_TX     SPI2_TX
I2C0    12  I2C0_RX     I2C0_RX     I2C0_RX     I2C0_RX     I2C0_RX     I2C0_RX     I2C0_RX     I2C0_RX
        13  I2C0_TX     I2C0_TX     I2C0_TX     I2C0_TX     I2C0_TX     I2C0_TX     I2C0_TX     I2C0_TX
I2C1    14  I2C1_RX     I2C1_RX     I2C1_RX     I2C1_RX     I2C1_RX     I2C1_RX     I2C1_RX     I2C1_RX
        15  I2C1_TX     I2C1_TX     I2C1_TX     I2C1_TX     I2C1_TX     I2C1_TX     I2C1_TX     I2C1_TX
*/

typedef struct
{
    __IOM uint32_t SRC_ADDR;      // 0x00
    __IOM uint32_t DST_ADDR;      // 0x04
    __IOM uint32_t TRANS_NUM;     // 0x08
    __IOM uint32_t CTRL_CFG;      // 0x0C
    __IOM uint32_t FINE_BYTE_NUM; // 0x10
} stDMAChannel_t, *pstDMAChannel;

typedef struct
{
    stDMAChannel_t stDMA_CH1;        // 0x00
    stDMAChannel_t stDMA_CH2;        // 0x14
    stDMAChannel_t stDMA_CH3;        // 0x28
    stDMAChannel_t stDMA_CH4;        // 0x3C
    stDMAChannel_t stDMA_CH5;        // 0x50
    stDMAChannel_t stDMA_CH6;        // 0x64
    stDMAChannel_t stDMA_CH7;        // 0x78
    stDMAChannel_t stDMA_CH8;        // 0x8C - 0x9C
    __IOM uint32_t DMA_SOFT_REQ;     // 0xA0
    __IOM uint32_t DMA_REQ_SET;      // 0xA4
    __IOM uint32_t DMA_PRIORITY_SET; // 0xA8
    __IM uint32_t DMA_INT_STATE;     // 0xAC
    __IOM uint32_t DMA_INT_CLR;      // 0xB0
    __IOM uint32_t DMA_REQ_MAP;      // 0xB4
    __IOM uint32_t DMA_CHL1_EN;      // 0xB8
    __IOM uint32_t DMA_CHL2_EN;      // 0xBC
    __IOM uint32_t DMA_CHL3_EN;      // 0xC0
    __IOM uint32_t DMA_CHL4_EN;      // 0xC4
    __IOM uint32_t DMA_CHL5_EN;      // 0xC8
    __IOM uint32_t DMA_CHL6_EN;      // 0xCC
    __IOM uint32_t DMA_CHL7_EN;      // 0xD0
    __IOM uint32_t DMA_CHL8_EN;      // 0xD4
    __IOM uint32_t CHL_RST_SW_CFG;   // 0xD8
    __IOM uint32_t DMA_DEBUG;        // 0xDC
    __IOM uint32_t DMA_REQ2;         // 0xE0
} stDMAHandle_t, *pstDMAHandle_t;

#define DMA_CHX_BASE_ADDR(ch)   (ADDR_DMA_BASE + 0x14 * (ch))
#define DMA_CHX_EN_REG(ch)      (ADDR_DMA_BASE + 0xB8 + 0x4 * (ch))

#define DMA_MAX_CHANNEL_NUM (8)

//------------------------------------------------------------------------------
// DMA Channel define
//------------------------------------------------------------------------------
#define DMA_CH1_SHIFT (0)
#define DMA_CH2_SHIFT (1)
#define DMA_CH3_SHIFT (2)
#define DMA_CH4_SHIFT (3)
#define DMA_CH5_SHIFT (4)
#define DMA_CH6_SHIFT (5)
#define DMA_CH7_SHIFT (6)
#define DMA_CH8_SHIFT (7)

//------------------------------------------------------------------------------
// DMA_SRC_ADDR
//------------------------------------------------------------------------------
#define DMA_SRC_ADDR_REG_MASK (0xFFFFFFFF)

//------------------------------------------------------------------------------
// DMA_DST_ADDR
//------------------------------------------------------------------------------
#define DMA_DST_ADDR_REG_MASK (0xFFFFFFFF)

//------------------------------------------------------------------------------
// DMA_TRANS_NUM
//------------------------------------------------------------------------------
// BIT[0:11]
#define DMA_TRANS_NUM_MASK  (0xFFF)
#define DMA_TRANS_NUM_SHIFT (12)

// BIT[12:23]
#define DMA_TRIG_NUM_MASK   (0xFFF)
#define DMA_TRIG_NUM_SHIFT  (0)

#define DMA_TRANS_NUM_REG_MASK ((DMA_TRANS_NUM_MASK << DMA_TRANS_NUM_SHIFT) | \
                                (DMA_TRIG_NUM_MASK << DMA_TRIG_NUM_SHIFT))

//------------------------------------------------------------------------------
// DMA_CTRL_CFG
//------------------------------------------------------------------------------

#define DMA_CTRL_CFG_ERR_STOP_EN_MASK       (0x1)
#define DMA_CTRL_CFG_ERR_STOP_EN_SHIFT      (0)

#define DMA_CTRL_CFG_FINISH_INT_EN_MASK     (0x1)
#define DMA_CTRL_CFG_FINISH_INT_EN_SHIFT    (1)

#define DMA_CTRL_CFG_SRC_ADDR_RISE_EN_MASK  (0x1)
#define DMA_CTRL_CFG_SRC_ADDR_RISE_EN_SHIFT (2)

#define DMA_CTRL_CFG_DST_ADDR_RISE_EN_MASK  (0x1)
#define DMA_CTRL_CFG_DST_ADDR_RISE_EN_SHIFT (3)

#define DMA_CTRL_CFG_SRC_DATA_TYPE_MASK     (0x3)
#define DMA_CTRL_CFG_SRC_DATA_TYPE_SHIFT    (4)

#define DMA_CTRL_CFG_DST_DATA_TYPE_MASK     (0x3)
#define DMA_CTRL_CFG_DST_DATA_TYPE_SHIFT    (6)

#define DMA_CTRL_CFG_MUTI_TRIG_EN_MASK      (0x1)
#define DMA_CTRL_CFG_MUTI_TRIG_EN_SHIFT     (8)

#define DMA_CTRL_CFG_TIMEOUT_EN_MASK        (0x1)
#define DMA_CTRL_CFG_TIMEOUT_EN_SHIFT       (9)

#define DMA_CTRL_CFG_TIMEOUT_VAL_MASK       (0xFFFFU)
#define DMA_CTRL_CFG_TIMEOUT_VAL_SHIFT      (16)

#define DMA_CTRL_CFG_REG_MASK ((DMA_CTRL_CFG_ERR_STOP_EN_MASK << DMA_CTRL_CFG_ERR_STOP_EN_SHIFT) |           \
                               (DMA_CTRL_CFG_FINISH_INT_EN_MASK << DMA_CTRL_CFG_FINISH_INT_EN_SHIFT) |       \
                               (DMA_CTRL_CFG_SRC_ADDR_RISE_EN_MASK << DMA_CTRL_CFG_SRC_ADDR_RISE_EN_SHIFT) | \
                               (DMA_CTRL_CFG_DST_ADDR_RISE_EN_MASK << DMA_CTRL_CFG_DST_ADDR_RISE_EN_SHIFT) | \
                               (DMA_CTRL_CFG_SRC_DATA_TYPE_MASK << DMA_CTRL_CFG_SRC_DATA_TYPE_SHIFT) |       \
                               (DMA_CTRL_CFG_DST_DATA_TYPE_MASK << DMA_CTRL_CFG_DST_DATA_TYPE_SHIFT) |       \
                               (DMA_CTRL_CFG_MUTI_TRIG_EN_MASK << DMA_CTRL_CFG_MUTI_TRIG_EN_SHIFT)  |        \
                               (DMA_CTRL_CFG_TIMEOUT_EN_MASK << DMA_CTRL_CFG_TIMEOUT_EN_SHIFT) |             \
                               (DMA_CTRL_CFG_TIMEOUT_VAL_MASK << DMA_CTRL_CFG_TIMEOUT_VAL_SHIFT))

//------------------------------------------------------------------------------
// DMA_FINE_NUM
//------------------------------------------------------------------------------
#define DMA_FINE_NUM_REG_MASK (0x3FFF)

//------------------------------------------------------------------------------
// 0xA0 SOFT_REQ
//------------------------------------------------------------------------------
#define DMA_SOFT_REQ_CH1_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH1_EN_SHIFT (0)

#define DMA_SOFT_REQ_CH2_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH2_EN_SHIFT (1)

#define DMA_SOFT_REQ_CH3_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH3_EN_SHIFT (2)

#define DMA_SOFT_REQ_CH4_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH4_EN_SHIFT (3)

#define DMA_SOFT_REQ_CH5_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH5_EN_SHIFT (4)

#define DMA_SOFT_REQ_CH6_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH6_EN_SHIFT (5)

#define DMA_SOFT_REQ_CH7_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH7_EN_SHIFT (6)

#define DMA_SOFT_REQ_CH8_EN_MASK (0x1)
#define DMA_SOFT_REQ_CH8_EN_SHIFT (7)

#define DMA_SOFT_REQ_REG_MASK ((DMA_SOFT_REQ_CH1_EN_MASK << DMA_SOFT_REQ_CH1_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH2_EN_MASK << DMA_SOFT_REQ_CH2_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH3_EN_MASK << DMA_SOFT_REQ_CH3_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH4_EN_MASK << DMA_SOFT_REQ_CH4_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH5_EN_MASK << DMA_SOFT_REQ_CH5_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH6_EN_MASK << DMA_SOFT_REQ_CH6_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH7_EN_MASK << DMA_SOFT_REQ_CH7_EN_SHIFT) | \
                               (DMA_SOFT_REQ_CH8_EN_MASK << DMA_SOFT_REQ_CH8_EN_SHIFT))

//------------------------------------------------------------------------------
// 0xA4 REQ_SET
//------------------------------------------------------------------------------
#define DMA_REQ_SET_REG_MASK (DMA_SOFT_REQ_REG_MASK)

//------------------------------------------------------------------------------
// 0xA8 DMA_PRIORITY_SET
//------------------------------------------------------------------------------
#define DMA_PRIORITY_SET_REG_MASK (DMA_SOFT_REQ_REG_MASK)

//------------------------------------------------------------------------------
// 0xAC INT_STATE
//------------------------------------------------------------------------------
#define DMA_INT_CH0_DONE_SHIFT (0)
#define DMA_INT_CH1_DONE_SHIFT (1)
#define DMA_INT_CH2_DONE_SHIFT (2)
#define DMA_INT_CH3_DONE_SHIFT (3)
#define DMA_INT_CH4_DONE_SHIFT (4)
#define DMA_INT_CH5_DONE_SHIFT (5)
#define DMA_INT_CH6_DONE_SHIFT (6)
#define DMA_INT_CH7_DONE_SHIFT (7)

#define DMA_INT_CH0_CFG_ERR_SHIFT (8)
#define DMA_INT_CH1_CFG_ERR_SHIFT (9)
#define DMA_INT_CH2_CFG_ERR_SHIFT (10)
#define DMA_INT_CH3_CFG_ERR_SHIFT (11)
#define DMA_INT_CH4_CFG_ERR_SHIFT (12)
#define DMA_INT_CH5_CFG_ERR_SHIFT (13)
#define DMA_INT_CH6_CFG_ERR_SHIFT (14)
#define DMA_INT_CH7_CFG_ERR_SHIFT (15)
#define DMA_INT_FALT_SHIFT        (16)

#define DMA_INT_STATE_REG_MASK (0x1FFFF)

//------------------------------------------------------------------------------
// 0xB0 INT_CLR
//------------------------------------------------------------------------------
#define DMA_INT_CLR_REG_MASK (DMA_INT_STATE_REG_MASK)

//------------------------------------------------------------------------------
// 0xB4 REQ_MAP0
//------------------------------------------------------------------------------
#define DMA_REQ_MAP_BIT_MASK (0x1F)
#define DMA_REQ_MAP_BIT_NUM (5)

#define DMA_REQ_MAP0_LAST_BIT_NUM (2)
#define DMA_REQ_MAP0_LAST_BIT_MASK (0x3)

#define DMA_REQ_MAP0_REG_MASK (0xFFFFFFFF)

//------------------------------------------------------------------------------
// 0xB8 DMA_CHL1_EN
//------------------------------------------------------------------------------
#define DMA_CHL1_EN_REG_MASK (0xFFFFFFF)

//------------------------------------------------------------------------------
// Other CH_EN
//------------------------------------------------------------------------------
#define DMA_CH_EN_REG_MASK (0x1)

//------------------------------------------------------------------------------
// 0xD8 CHL_RST_SW_CFG
//------------------------------------------------------------------------------
#define DMA_CHL_RST_SW_CFG_REG_MASK (0xFF)

//------------------------------------------------------------------------------
// 0xDC DEBUG
//------------------------------------------------------------------------------
#define DMA_DEBUG_REG_MASK (0xFFFFFFFF)

#endif
