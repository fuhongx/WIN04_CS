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
    __IOM uint32_t SPI_NSEN;        // 0x20
    __IOM uint32_t DMA_TIMEOUT;     // 0x24
    __IOM uint32_t SPI_CSHT;        // 0x28
    __IOM uint32_t SPI_CLKMOD;      // 0x2C
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

//BIT[7:8]
#define SPI_TCR_SPI_DATA_LEN_MASK       (0x3)
#define SPI_TCR_SPI_DATA_LEN_SHIFT      (7)

//BIT[9] RES

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
#define SPI_TCR_SPI_ANTI_NOISE_EN_MASK     (0x1)
#define SPI_TCR_SPI_ANTI_NOISE_EN_SHIFT    (13)

//BIT[14]
#define SPI_TCR_SPI_TX_EMPTY_ERR_MASK       (0x1)
#define SPI_TCR_SPI_TX_EMPTY_ERR_SHIFT      (14)

//BIT[15]
#define SPI_TCR_DMA_TX_TIMING_MASK          (0x1)
#define SPI_TCR_DMA_TX_TIMING_SHIFT         (15)

#define SPI_TCR_REG_MASK            ((SPI_TCR_SPI_CLK_DIV_MASK << SPI_TCR_SPI_CLK_DIV_SHIFT)|\
                                    (SPI_TCR_SPI_MODE_FLAG_MASK << SPI_TCR_SPI_MODE_FLAG_SHIFT)|\
                                    (SPI_TCR_SPI_CLK_ADJUST_MASK << SPI_TCR_SPI_CLK_ADJUST_SHIFT)|\
                                    (SPI_TCR_SPI_DATA_MODE_MASK << SPI_TCR_SPI_DATA_MODE_SHIFT)|\
                                    (SPI_TCR_SPI_DATA_LEN_MASK << SPI_TCR_SPI_DATA_LEN_SHIFT)|\
                                    (SPI_TCR_DMA_TX_EN_MASK << SPI_TCR_DMA_TX_EN_SHIFT)|\
                                    (SPI_TCR_DMA_RX_EN_MASK << SPI_TCR_DMA_RX_EN_SHIFT)|\
                                    (SPI_TCR_CS_MODE_MASK << SPI_TCR_CS_MODE_SHIFT)|\
                                    (SPI_TCR_SPI_ANTI_NOISE_EN_MASK << SPI_TCR_SPI_ANTI_NOISE_EN_SHIFT)|\
                                    (SPI_TCR_SPI_TX_EMPTY_ERR_MASK << SPI_TCR_SPI_TX_EMPTY_ERR_SHIFT)|\
                                    (SPI_TCR_DMA_TX_TIMING_MASK << SPI_TCR_DMA_TX_TIMING_SHIFT))


//------------------------------------------------------------------------------
//SPI_THR 
//------------------------------------------------------------------------------
#define SPI_THR_REG_MASK            (0xFFFFFFFF)

//------------------------------------------------------------------------------
//SPI_RBR 
//------------------------------------------------------------------------------
#define SPI_RBR_REG_MASK            (0xFFFFFFFF)

//------------------------------------------------------------------------------
//MCS_CTRL 
//------------------------------------------------------------------------------
#define SPI_MCS_CTRL_REG_MASK       (0x1)

//------------------------------------------------------------------------------
//SPI_IMR 
//------------------------------------------------------------------------------
//BIT[0]
#define SPI_IMR_INT_TX_BUFFER_OVERFLOW_MASK         (0x1)
#define SPI_IMR_INT_TX_BUFFER_OVERFLOW_SHIFT        (0)

//BIT[1]
#define SPI_IMR_INT_TX_BUFFER_EMPTY_MASK            (0x1)
#define SPI_IMR_INT_TX_BUFFER_EMPTY_SHIFT           (1)

//BIT[2]
#define SPI_IMR_INT_TX_FINISH_MASK           (0x1)
#define SPI_IMR_INT_TX_FINISH_SHIFT          (2)

//BIT[3]
#define SPI_IMR_INT_RX_ERROR_MASK                   (0x1)
#define SPI_IMR_INT_RX_ERROR_SHIFT                  (3)

//BIT[4]
#define SPI_IMR_INT_RX_BUFFER_OVERFLOW_MASK         (0x1)
#define SPI_IMR_INT_RX_BUFFER_OVERFLOW_SHIFT        (4)

//BIT[5]
#define SPI_IMR_INT_RX_FINISH_MASK                  (0x1)
#define SPI_IMR_INT_RX_FINISH_SHIFT                 (5)

//BIT[6]
#define SPI_IMR_INT_SPI_TRANS_END_MASK              (0x1)
#define SPI_IMR_INT_SPI_TRANS_END_SHIFT             (6)

//BIT[7]
#define SPI_IMR_INT_TX_BUFFER_EMPTY_ERR_MASK        (0x1)
#define SPI_IMR_INT_TX_BUFFER_EMPTY_ERR_SHIFT       (7)

#define SPI_IMR_REG_MASK            ((SPI_IMR_INT_TX_BUFFER_OVERFLOW_MASK << SPI_IMR_INT_TX_BUFFER_OVERFLOW_SHIFT)|\
                                    (SPI_IMR_INT_TX_BUFFER_EMPTY_MASK << SPI_IMR_INT_TX_BUFFER_EMPTY_SHIFT)|\
                                    (SPI_IMR_INT_TX_FINISH_MASK << SPI_IMR_INT_TX_FINISH_SHIFT)|\
                                    (SPI_IMR_INT_RX_ERROR_MASK << SPI_IMR_INT_RX_ERROR_SHIFT)|\
                                    (SPI_IMR_INT_RX_BUFFER_OVERFLOW_MASK << SPI_IMR_INT_RX_BUFFER_OVERFLOW_SHIFT)|\
                                    (SPI_IMR_INT_RX_FINISH_MASK << SPI_IMR_INT_RX_FINISH_SHIFT)|\
                                    (SPI_IMR_INT_SPI_TRANS_END_MASK << SPI_IMR_INT_SPI_TRANS_END_SHIFT)|\
                                    (SPI_IMR_INT_TX_BUFFER_EMPTY_ERR_MASK << SPI_IMR_INT_TX_BUFFER_EMPTY_ERR_SHIFT))

//------------------------------------------------------------------------------
//SPI_ISR 
//------------------------------------------------------------------------------
#define SPI_ISR_REG_MASK            (SPI_IMR_REG_MASK)

//------------------------------------------------------------------------------
//SPI_TSR 
//------------------------------------------------------------------------------
//BIT[0]
#define SPI_TSR_RX_NEW_DATA_MASK              (0x1)
#define SPI_TSR_RX_NEW_DATA_SHIFT             (0)

//BIT[1]
#define SPI_TSR_RX_DATA_OVERFLOW_MASK           (0x1)
#define SPI_TSR_RX_DATA_OVERFLOW_SHIFT          (1)

//BIT[2]
#define SPI_TSR_RX_DATA_ERROR_MASK                (0x1)
#define SPI_TSR_RX_DATA_ERROR_SHIFT                (2)

//BIT[3]
#define SPI_TSR_TX_BUFFER_EMPTY_MASK            (0x1)
#define SPI_TSR_TX_BUFFER_EMPTY_SHIFT           (3)

//BIT[4]
#define SPI_TSR_SPI_BUSY_MASK                   (0x1)
#define SPI_TSR_SPI_BUSY_SHIFT                   (4)



#define SPI_TSR_REG_MASK            ((SPI_TSR_RX_NEW_DATA_MASK << SPI_TSR_RX_NEW_DATA_SHIFT)|\
                                    (SPI_TSR_RX_DATA_OVERFLOW_MASK << SPI_TSR_RX_DATA_OVERFLOW_SHIFT)|\
                                    (SPI_TSR_RX_DATA_ERROR_MASK << SPI_TSR_RX_DATA_ERROR_SHIFT)|\
                                    (SPI_TSR_TX_BUFFER_EMPTY_MASK << SPI_TSR_TX_BUFFER_EMPTY_SHIFT)|\
                                    (SPI_TSR_SPI_BUSY_MASK << SPI_TSR_SPI_BUSY_SHIFT))

//------------------------------------------------------------------------------
//SPI_IER, Interrupt enable. 
//------------------------------------------------------------------------------
#define SPI_IER_REG_MASK            (SPI_IMR_REG_MASK)

//------------------------------------------------------------------------------
//SPI_NSEN, Anti noise enable.
//------------------------------------------------------------------------------
#define SPI_NESN_REG_MASK           (0x1)

//------------------------------------------------------------------------------
//DMA_TIMEOUT, unit spi clk.
//------------------------------------------------------------------------------
#define SPI_DMA_TIMEOUT_REG_MASK        (0xFFFFFFFF)

//------------------------------------------------------------------------------
//SPI_CSHT ,Define min cs hold time. unit 20.8ns.
//------------------------------------------------------------------------------
#define SPI_CSHT_REG_MASK               (0x3FFFF)

//------------------------------------------------------------------------------
//SPI_CLKMOD
//------------------------------------------------------------------------------
//BIT[0]
#define SPI_CLKMOD_CPOL_REG_MASK            (0x1)
#define SPI_CLKMOD_CPOL_REG_SHIFT           (0)

//BIT[1]
#define SPI_CLKMODE_CPHA_REG_MASK           (0x1)
#define SPI_CLKMODE_CPHA_REG_SHIFT          (1)

#define SPI_CLKMOD_REG_MASK             ((SPI_CLKMOD_CPOL_REG_MASK << SPI_CLKMOD_CPOL_REG_SHIFT)|\
                                        (SPI_CLKMODE_CPHA_REG_MASK << SPI_CLKMODE_CPHA_REG_SHIFT))

#define SPI_CLKMOD_CPOL0_CPHA0     ( 0 )

#define SPI_CLKMOD_CPOL1_CPHA0     (((SPI_CLKMOD_CPOL_REG_MASK) << SPI_CLKMOD_CPOL_REG_SHIFT) & SPI_CLKMOD_REG_MASK)

#define SPI_CLKMOD_CPOL0_CPHA1     (((SPI_CLKMODE_CPHA_REG_MASK) << SPI_CLKMODE_CPHA_REG_SHIFT) & SPI_CLKMOD_REG_MASK)

#define SPI_CLKMOD_CPOL1_CPHA1     ( (((SPI_CLKMOD_CPOL_REG_MASK) << SPI_CLKMOD_CPOL_REG_SHIFT) |\
                                     ((SPI_CLKMODE_CPHA_REG_MASK) << SPI_CLKMODE_CPHA_REG_SHIFT)) & SPI_CLKMOD_REG_MASK)

#endif
