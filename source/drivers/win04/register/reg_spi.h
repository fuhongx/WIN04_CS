/**
 * ****************************************************************************
 * @file reg_spi.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#ifndef __REG_SPI_H__
#define __REG_SPI_H__

typedef struct
{
    __IOM uint32_t SPI_TCR;         // 0x00
    __IOM uint32_t SPI_THR;         // 0x04
    __IOM uint32_t SPI_RBR;         // 0x08
    __IOM uint32_t MCS_CTRL;        // 0x0C
    __IOM uint32_t SPI_IMR;         // 0x10
    __IOM uint32_t SPI_ISR;         // 0x14
    __IOM uint32_t SPI_TSR;         // 0x18
    __IOM uint32_t SPI_IER;         // 0x1C
    __IOM uint32_t SPI_CTR;         // 0x20
    __IOM uint32_t SPI_FTR;         // 0x24
}stSpiHandle_t, *pstSpiHandle_t;

//------------------------------------------------------------------------------
//SPI_TCR 
//------------------------------------------------------------------------------
//BIT[0:3]
#define SPI_TCR_SPI_CLK_DIV_MASK         (0xF)
#define SPI_TCR_SPI_CLK_DIV_SHIFT        (0)

//BIT[4]
#define SPI_TCR_SPI_MODE_FLAG_MASK       (0x1)
#define SPI_TCR_SPI_MODE_FLAG_SHIFT      (4)

//BIT[5]
#define SPI_TCR_SPI_CLK_ADJUST_MASK     (0x1)
#define SPI_TCR_SPI_CLK_ADJUST_SHIFT    (5)

//BIT[6]
#define SPI_TCR_SPI_DATA_MODE_MASK      (0x1)
#define SPI_TCR_SPI_DATA_MODE_SHIFT     (6)

//BIT[10]
#define SPI_TCR_DMA_TX_EN_MASK          (0x1)
#define SPI_TCR_DMA_TX_EN_SHIFT         (10)

//BIT[11]
#define SPI_TCR_DMA_RX_EN_MASK          (0x1)
#define SPI_TCR_DMA_RX_EN_SHIFT         (11)

//BIT[12]
#define SPI_TCR_CS_MODE_MASK            (0x1)
#define SPI_TCR_CS_MODE_SHIFT           (12)

//BIT[13]
#define SPI_TCR_SPI_NOISE_MODE_MASK     (0x1)
#define SPI_TCR_SPI_NOISE_MODE_SHIFT    (13)

//BIT[16]
#define SPI_CLKMOD_CPOL_REG_MASK            (0x1)
#define SPI_CLKMOD_CPOL_REG_SHIFT           (16)
#define SPI_CLKMOD_CPOL_REG_VAL(x)          (((x) & SPI_CLKMOD_CPOL_REG_MASK) << SPI_CLKMOD_CPOL_REG_SHIFT)

//BIT[17]
#define SPI_CLKMODE_CPHA_REG_MASK           (0x1)
#define SPI_CLKMODE_CPHA_REG_SHIFT          (17)
#define SPI_CLKMODE_CPHA_REG_VAL(x)          (((x) & SPI_CLKMODE_CPHA_REG_MASK) << SPI_CLKMODE_CPHA_REG_SHIFT)

#define SPI_SNOISE_EN_MASK                  (0x1)
#define SPI_SNOISE_EN_SHIFT                 (18)

#define SPI_TCR_REG_MASK            ((SPI_TCR_SPI_CLK_DIV_MASK << SPI_TCR_SPI_CLK_DIV_SHIFT)|\
                                    (SPI_TCR_SPI_MODE_FLAG_MASK << SPI_TCR_SPI_MODE_FLAG_SHIFT)|\
                                    (SPI_TCR_SPI_CLK_ADJUST_MASK << SPI_TCR_SPI_CLK_ADJUST_SHIFT)|\
                                    (SPI_TCR_SPI_DATA_MODE_MASK << SPI_TCR_SPI_DATA_MODE_SHIFT)|\
                                    (SPI_TCR_DMA_TX_EN_MASK << SPI_TCR_DMA_TX_EN_SHIFT)|\
                                    (SPI_TCR_DMA_RX_EN_MASK << SPI_TCR_DMA_RX_EN_SHIFT)|\
                                    (SPI_TCR_CS_MODE_MASK << SPI_TCR_CS_MODE_SHIFT)|\
                                    (SPI_TCR_SPI_NOISE_MODE_MASK << SPI_TCR_SPI_NOISE_MODE_SHIFT)|\
                                    (SPI_CLKMOD_CPOL_REG_MASK << SPI_CLKMOD_CPOL_REG_SHIFT)|\
                                    (SPI_CLKMODE_CPHA_REG_MASK << SPI_CLKMODE_CPHA_REG_SHIFT)|\
                                    (SPI_SNOISE_EN_MASK << SPI_SNOISE_EN_SHIFT))


//------------------------------------------------------------------------------
//SPI_THR 
//------------------------------------------------------------------------------
#define SPI_THR_REG_MASK            (0xFF)

//------------------------------------------------------------------------------
//SPI_RBR 
//------------------------------------------------------------------------------
#define SPI_RBR_REG_MASK            (0xFF)

//------------------------------------------------------------------------------
//MCS_CTRL 
//------------------------------------------------------------------------------
#define SPI_MCS_CTRL_REG_MASK       (0x1)

//------------------------------------------------------------------------------
//SPI_IMR 
//------------------------------------------------------------------------------
//BIT[0]
#define SPI_IMR_TX_FIFO_EMPTY_MASK      (0x1)
#define SPI_IMR_TX_FIFO_EMPTY_SHIFT     (0)
#define SPI_IMR_TX_FIFO_EMPTY_VAL(x)    (((x) & SPI_IMR_TX_FIFO_EMPTY_MASK) << SPI_IMR_TX_FIFO_EMPTY_SHIFT)

#define SPI_IMR_TX_FIFO_PEMPTY_MASK     (0x1)
#define SPI_IMR_TX_FIFO_PEMPTY_SHIFT    (1)
#define SPI_IMR_TX_FIFO_PEMPTY_VAL(x)   (((x) & SPI_IMR_TX_FIFO_PEMPTY_MASK) << SPI_IMR_TX_FIFO_PEMPTY_SHIFT)

#define SPI_IMR_RX_FIFO_FULL_MASK       (0x1)
#define SPI_IMR_RX_FIFO_FULL_SHIFT      (2)
#define SPI_IMR_RX_FIFO_FULL_VAL(x)     (((x) & SPI_IMR_RX_FIFO_FULL_MASK) << SPI_IMR_RX_FIFO_FULL_SHIFT)

#define SPI_IMR_RX_FIFO_PFULL_MASK      (0x1)
#define SPI_IMR_RX_FIFO_PFULL_SHIFT     (3)
#define SPI_IMR_RX_FIFO_PFULL_VAL(x)    (((x) & SPI_IMR_RX_FIFO_PFULL_MASK) << SPI_IMR_RX_FIFO_PFULL_SHIFT)

#define SPI_IMR_TX_OVERWRITE_MASK       (0x1)
#define SPI_IMR_TX_OVERWRITE_SHIFT      (4)
#define SPI_IMR_TX_OVERWRITE_VAL(x)     (((x) & SPI_IMR_TX_OVERWRITE_MASK) << SPI_IMR_TX_OVERWRITE_SHIFT)

#define SPI_IMR_TX_BYTE_DONE_MASK       (0x1)
#define SPI_IMR_TX_BYTE_DONE_SHIFT      (5)
#define SPI_IMR_TX_BYTE_DONE_VAL(x)     (((x) & SPI_IMR_TX_BYTE_DONE_MASK) << SPI_IMR_TX_BYTE_DONE_SHIFT)

#define SPI_IMR_SLV_RX_FRAME_ERR_MASK   (0x1)
#define SPI_IMR_SLV_RX_FRAME_ERR_SHIFT  (6)
#define SPI_IMR_SLV_RX_FRAME_ERR_VAL(x) (((x) & SPI_IMR_SLV_RX_FRAME_ERR_MASK) << SPI_IMR_SLV_RX_FRAME_ERR_SHIFT)

#define SPI_IMR_SLV_RX_DONE_MASK        (0x1)
#define SPI_IMR_SLV_RX_DONE_SHIFT       (7)
#define SPI_IMR_SLV_RX_DONE_VAL(x)      (((x) & SPI_IMR_SLV_RX_DONE_MASK) << SPI_IMR_SLV_RX_DONE_SHIFT)

#define SPI_IMR_RX_OVERWRITE_MASK       (0x1)
#define SPI_IMR_RX_OVERWRITE_SHIFT      (8)
#define SPI_IMR_RX_OVERWRITE_VAL(x)     (((x) & SPI_IMR_RX_OVERWRITE_MASK) << SPI_IMR_RX_OVERWRITE_SHIFT)

//------------------------------------------------------------------------------
//SPI_ISR 
//------------------------------------------------------------------------------
#define SPI_ISR_TX_FIFO_EMPTY_MASK      (0x1)
#define SPI_ISR_TX_FIFO_EMPTY_SHIFT     (0)
#define SPI_ISR_TX_FIFO_EMPTY_VAL(x)    (((x) & SPI_ISR_TX_FIFO_EMPTY_MASK) << SPI_ISR_TX_FIFO_EMPTY_SHIFT)

#define SPI_ISR_TX_FIFO_PEMPTY_MASK     (0x1)
#define SPI_ISR_TX_FIFO_PEMPTY_SHIFT    (1)
#define SPI_ISR_TX_FIFO_PEMPTY_VAL(x)   (((x) & SPI_ISR_TX_FIFO_PEMPTY_MASK) << SPI_ISR_TX_FIFO_PEMPTY_SHIFT)

#define SPI_ISR_RX_FIFO_FULL_MASK       (0x1)
#define SPI_ISR_RX_FIFO_FULL_SHIFT      (2)
#define SPI_ISR_RX_FIFO_FULL_VAL(x)     (((x) & SPI_ISR_RX_FIFO_FULL_MASK) << SPI_ISR_RX_FIFO_FULL_SHIFT)

#define SPI_ISR_RX_FIFO_PFULL_MASK      (0x1)
#define SPI_ISR_RX_FIFO_PFULL_SHIFT     (3)
#define SPI_ISR_RX_FIFO_PFULL_VAL(x)    (((x) & SPI_ISR_RX_FIFO_PFULL_MASK) << SPI_ISR_RX_FIFO_PFULL_SHIFT)

#define SPI_ISR_TX_OVERWRITE_MASK       (0x1)
#define SPI_ISR_TX_OVERWRITE_SHIFT      (4)
#define SPI_ISR_TX_OVERWRITE_VAL(x)     (((x) & SPI_ISR_TX_OVERWRITE_MASK) << SPI_ISR_TX_OVERWRITE_SHIFT)

#define SPI_ISR_TX_BYTE_DONE_MASK       (0x1)
#define SPI_ISR_TX_BYTE_DONE_SHIFT      (5)
#define SPI_ISR_TX_BYTE_DONE_VAL(x)     (((x) & SPI_ISR_TX_BYTE_DONE_MASK) << SPI_ISR_TX_BYTE_DONE_SHIFT)

#define SPI_ISR_SLV_RX_FRAME_ERR_MASK   (0x1)
#define SPI_ISR_SLV_RX_FRAME_ERR_SHIFT  (6)
#define SPI_ISR_SLV_RX_FRAME_ERR_VAL(x) (((x) & SPI_ISR_SLV_RX_FRAME_ERR_MASK) << SPI_ISR_SLV_RX_FRAME_ERR_SHIFT)

#define SPI_ISR_SLV_RX_DONE_MASK        (0x1)
#define SPI_ISR_SLV_RX_DONE_SHIFT       (7)
#define SPI_ISR_SLV_RX_DONE_VAL(x)      (((x) & SPI_ISR_SLV_RX_DONE_MASK) << SPI_ISR_SLV_RX_DONE_SHIFT)

#define SPI_ISR_RX_OVERWRITE_MASK       (0x1)
#define SPI_ISR_RX_OVERWRITE_SHIFT      (8)
#define SPI_ISR_RX_OVERWRITE_VAL(x)     (((x) & SPI_ISR_RX_OVERWRITE_MASK) << SPI_ISR_RX_OVERWRITE_SHIFT)

//------------------------------------------------------------------------------
//SPI_TSR 
//------------------------------------------------------------------------------
#define SPI_TSR_SPI_BUSY_MASK           (0x1)
#define SPI_TSR_SPI_BUSY_SHIFT          (0)
#define SPI_TSR_SPI_BUSY_VAL(x)         (((x) & SPI_TSR_SPI_BUSY_MASK) << SPI_TSR_SPI_BUSY_SHIFT)

#define SPI_TSR_TX_FIFO_CNT_MASK        (0xFF)
#define SPI_TSR_TX_FIFO_CNT_SHIFT       (8)
#define SPI_TSR_TX_FIFO_CNT_VAL(x)      (((x) & SPI_TSR_TX_FIFO_CNT_MASK) << SPI_TSR_TX_FIFO_CNT_SHIFT)

#define SPI_TSR_RX_FIFO_CNT_MASK        (0xFF)
#define SPI_TSR_RX_FIFO_CNT_SHIFT       (16)
#define SPI_TSR_RX_FIFO_CNT_VAL(x)      (((x) & SPI_TSR_RX_FIFO_CNT_MASK) << SPI_TSR_RX_FIFO_CNT_SHIFT)

#define SPI_TSR_TX_FIFO_EMPTY_MASK      (0x1)
#define SPI_TSR_TX_FIFO_EMPTY_SHIFT     (24)
#define SPI_TSR_TX_FIFO_EMPTY_VAL(x)    (((x) & SPI_TSR_TX_FIFO_EMPTY_MASK) << SPI_TSR_TX_FIFO_EMPTY_SHIFT)

#define SPI_TSR_TX_FIFO_PEMPTY_MASK     (0x1)
#define SPI_TSR_TX_FIFO_PEMPTY_SHIFT    (25)
#define SPI_TSR_TX_FIFO_PEMPTY_VAL(x)   (((x) & SPI_TSR_TX_FIFO_PEMPTY_MASK) << SPI_TSR_TX_FIFO_PEMPTY_SHIFT)

#define SPI_TSR_TX_FIFO_FULL_MASK       (0x1)
#define SPI_TSR_TX_FIFO_FULL_SHIFT      (26)
#define SPI_TSR_TX_FIFO_FULL_VAL(x)     (((x) & SPI_TSR_TX_FIFO_FULL_MASK) << SPI_TSR_TX_FIFO_FULL_SHIFT)

#define SPI_TSR_TX_FIFO_PFULL_MASK      (0x1)
#define SPI_TSR_TX_FIFO_PFULL_SHIFT     (27)
#define SPI_TSR_TX_FIFO_PFULL_VAL(x)    (((x) & SPI_TSR_TX_FIFO_PFULL_MASK) << SPI_TSR_TX_FIFO_PFULL_SHIFT)

#define SPI_TSR_RX_FIFO_EMPTY_MASK      (0x1)
#define SPI_TSR_RX_FIFO_EMPTY_SHIFT     (28)
#define SPI_TSR_RX_FIFO_EMPTY_VAL(x)    (((x) & SPI_TSR_RX_FIFO_EMPTY_MASK) << SPI_TSR_RX_FIFO_EMPTY_SHIFT)

#define SPI_TSR_RX_FIFO_PEMPTY_MASK     (0x1)
#define SPI_TSR_RX_FIFO_PEMPTY_SHIFT    (29)
#define SPI_TSR_RX_FIFO_PEMPTY_VAL(x)   (((x) & SPI_TSR_RX_FIFO_PEMPTY_MASK) << SPI_TSR_RX_FIFO_PEMPTY_SHIFT)

#define SPI_TSR_RX_FIFO_FULL_MASK       (0x1)
#define SPI_TSR_RX_FIFO_FULL_SHIFT      (30)
#define SPI_TSR_RX_FIFO_FULL_VAL(x)     (((x) & SPI_TSR_RX_FIFO_FULL_MASK) << SPI_TSR_RX_FIFO_FULL_SHIFT)

#define SPI_TSR_RX_FIFO_PFULL_MASK      (0x1)
#define SPI_TSR_RX_FIFO_PFULL_SHIFT     (31)
#define SPI_TSR_RX_FIFO_PFULL_VAL(x)    (((x) & SPI_TSR_RX_FIFO_PFULL_MASK) << SPI_TSR_RX_FIFO_PFULL_SHIFT)

//------------------------------------------------------------------------------
//SPI_IER
//------------------------------------------------------------------------------
#define SPI_IER_TX_FIFO_EMPTY_MASK      (0x1)
#define SPI_IER_TX_FIFO_EMPTY_SHIFT     (0)
#define SPI_IER_TX_FIFO_EMPTY_VAL(x)    (((x) & SPI_IER_TX_FIFO_EMPTY_MASK) << SPI_IER_TX_FIFO_EMPTY_SHIFT)

#define SPI_IER_TX_FIFO_PEMPTY_MASK     (0x1)
#define SPI_IER_TX_FIFO_PEMPTY_SHIFT    (1)
#define SPI_IER_TX_FIFO_PEMPTY_VAL(x)   (((x) & SPI_IER_TX_FIFO_PEMPTY_MASK) << SPI_IER_TX_FIFO_PEMPTY_SHIFT)

#define SPI_IER_RX_FIFO_FULL_MASK       (0x1)
#define SPI_IER_RX_FIFO_FULL_SHIFT      (2)
#define SPI_IER_RX_FIFO_FULL_VAL(x)     (((x) & SPI_IER_RX_FIFO_FULL_MASK) << SPI_IER_RX_FIFO_FULL_SHIFT)

#define SPI_IER_RX_FIFO_PFULL_MASK      (0x1)
#define SPI_IER_RX_FIFO_PFULL_SHIFT     (3)
#define SPI_IER_RX_FIFO_PFULL_VAL(x)    (((x) & SPI_IER_RX_FIFO_PFULL_MASK) << SPI_IER_RX_FIFO_PFULL_SHIFT)

#define SPI_IER_TX_OVERWRITE_MASK       (0x1)
#define SPI_IER_TX_OVERWRITE_SHIFT      (4)
#define SPI_IER_TX_OVERWRITE_VAL(x)     (((x) & SPI_IER_TX_OVERWRITE_MASK) << SPI_IER_TX_OVERWRITE_SHIFT)

#define SPI_IER_TX_BYTE_DONE_MASK       (0x1)
#define SPI_IER_TX_BYTE_DONE_SHIFT      (5)
#define SPI_IER_TX_BYTE_DONE_VAL(x)     (((x) & SPI_IER_TX_BYTE_DONE_MASK) << SPI_IER_TX_BYTE_DONE_SHIFT)

#define SPI_IER_SLV_RX_FRAME_ERR_MASK   (0x1)
#define SPI_IER_SLV_RX_FRAME_ERR_SHIFT  (6)
#define SPI_IER_SLV_RX_FRAME_ERR_VAL(x) (((x) & SPI_IER_SLV_RX_FRAME_ERR_MASK) << SPI_IER_SLV_RX_FRAME_ERR_SHIFT)

#define SPI_IER_SLV_RX_DONE_MASK        (0x1)
#define SPI_IER_SLV_RX_DONE_SHIFT       (7)
#define SPI_IER_SLV_RX_DONE_VAL(x)      (((x) & SPI_IER_SLV_RX_DONE_MASK) << SPI_IER_SLV_RX_DONE_SHIFT)

#define SPI_IER_RX_OVERWRITE_MASK       (0x1)
#define SPI_IER_RX_OVERWRITE_SHIFT      (8)
#define SPI_IER_RX_OVERWRITE_VAL(x)     (((x) & SPI_IER_RX_OVERWRITE_MASK) << SPI_IER_RX_OVERWRITE_SHIFT)

//------------------------------------------------------------------------------
//SPI_CTR
//------------------------------------------------------------------------------
#define SPI_CTR_SETUP_HOLD_MASK         (0x1FF)
#define SPI_CTR_SETUP_HOLD_SHIFT        (0)
#define SPI_CTR_SETUP_HOLD_VAL(x)       (((x) & SPI_CTR_SETUP_HOLD_MASK) << SPI_CTR_SETUP_HOLD_SHIFT)

#define SPI_CTR_BYTE_GAP_MASK           (0x1FF)
#define SPI_CTR_BYTE_GAP_SHIFT          (16)
#define SPI_CTR_BYTE_GAP_VAL(x)         (((x) & SPI_CTR_BYTE_GAP_MASK) << SPI_CTR_BYTE_GAP_SHIFT)

//------------------------------------------------------------------------------
//SPI_FTR
//------------------------------------------------------------------------------
#define SPI_FTR_TX_FIFO_PFULL_TH_MASK   (0x1F)
#define SPI_FTR_TX_FIFO_PFULL_TH_SHIFT  (0)
#define SPI_FTR_TX_FIFO_PFULL_TH_VAL(x) (((x) & SPI_FTR_TX_FIFO_PFULL_TH_MASK) << SPI_FTR_TX_FIFO_PFULL_TH_SHIFT)

#define SPI_FTR_TX_FIFO_PEMPTY_TH_MASK  (0x1F)
#define SPI_FTR_TX_FIFO_PEMPTY_TH_SHIFT (8)
#define SPI_FTR_TX_FIFO_PEMPTY_TH_VAL(x) (((x) & SPI_FTR_TX_FIFO_PEMPTY_TH_MASK) << SPI_FTR_TX_FIFO_PEMPTY_TH_SHIFT)

#define SPI_FTR_RX_FIFO_PFULL_TH_MASK   (0x1F)
#define SPI_FTR_RX_FIFO_PFULL_TH_SHIFT  (16)
#define SPI_FTR_RX_FIFO_PFULL_TH_VAL(x) (((x) & SPI_FTR_RX_FIFO_PFULL_TH_MASK) << SPI_FTR_RX_FIFO_PFULL_TH_SHIFT)

#define SPI_FTR_RX_FIFO_PEMPTY_TH_MASK  (0x1F)
#define SPI_FTR_RX_FIFO_PEMPTY_TH_SHIFT (24)
#define SPI_FTR_RX_FIFO_PEMPTY_TH_VAL(x) (((x) & SPI_FTR_RX_FIFO_PEMPTY_TH_MASK) << SPI_FTR_RX_FIFO_PEMPTY_TH_SHIFT)

#endif
