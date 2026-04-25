/**
 * ****************************************************************************
 * @file reg_lpuart.h
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

#ifndef __REG_LPUART_H__
#define __REG_LPUART_H__

/**
 * @brief LpUart reg list define.
 *
 */
typedef struct
{
    __IOM uint32_t LPUART_CR;  // 0x00, 控制寄存器
    __IOM uint32_t LPUART_BRR; // 0x04, 波特率寄存器
    __IOM uint32_t LPUART_SR;  // 0x08, 状态寄存器
    __IOM uint32_t LPUART_IER; // 0x0C, 中断使能寄存器
    __IOM uint32_t LPUART_ISR; // 0x10, 中断状态寄存器
    __IOM uint32_t LPUART_TBR; // 0x14, 发送buffer寄存器
    __IOM uint32_t LPUART_RBR; // 0x18, 接收buffer寄存器
} stLpUartHandle_t, *pstLpUartHandle_t;

//------------------------------------------------------------------------------
//Start LPUART Define
//0x00 LPUART_CR
//------------------------------------------------------------------------------
//BIT[0] TX EN
#define LPUART_CR_TX_EN_MASK                       (0x1)
#define LPUART_CR_TX_EN_SHIFT                      (0)

//BIT[1] RX EN
#define LPUART_CR_RX_EN_MASK                       (0x1)
#define LPUART_CR_RX_EN_SHIFT                      (1)

//BIT[4:5] data len
#define LPUART_CR_DATA_LEN_MASK                    (0x3)
#define LPUART_CR_DATA_LEN_SHIFT                   (4)

//BIT[8:9] Stop len
#define LPUART_CR_STOP_LEN_MASK                    (0x3)
#define LPUART_CR_STOP_LEN_SHIFT                   (8)

//BIT[12] PARITY EN
#define LPUART_CR_PARITY_EN_MASK                   (0x1)
#define LPUART_CR_PARITY_EN_SHIFT                  (12)

//BIT[13] PARITY EVEN
#define LPUART_CR_PARITY_EVEN_MASK                 (0x1)
#define LPUART_CR_PARITY_EVEN_SHIFT                (13)

//BIT[16] DMA TX EN
#define LPUART_CR_DMA_TX_EN_MASK                   (0x1)
#define LPUART_CR_DMA_TX_EN_SHIFT                  (16)

//BIT[17] DMA RX EN
#define LPUART_CR_DMA_RX_EN_MASK                   (0x1)
#define LPUART_CR_DMA_RX_EN_SHIFT                  (17)

//BIT[20:22]
#define LPUART_CR_RX_THLD_MASK                     (0x7)
#define LPUART_CR_RX_THLD_SHIFT                    (20)



#define LPUART_CR_REG_MASK             ((LPUART_CR_TX_EN_MASK << LPUART_CR_TX_EN_SHIFT)|\
                                        (LPUART_CR_RX_EN_MASK << LPUART_CR_RX_EN_SHIFT)|\
                                        (LPUART_CR_DATA_LEN_MASK << LPUART_CR_DATA_LEN_SHIFT)|\
                                        (LPUART_CR_STOP_LEN_MASK << LPUART_CR_STOP_LEN_SHIFT)|\
                                        (LPUART_CR_PARITY_EN_MASK << LPUART_CR_PARITY_EN_SHIFT)|\
                                        (LPUART_CR_PARITY_EVEN_MASK << LPUART_CR_PARITY_EVEN_SHIFT)|\
                                        (LPUART_CR_DMA_TX_EN_MASK << LPUART_CR_DMA_TX_EN_SHIFT)|\
                                        (LPUART_CR_DMA_RX_EN_MASK << LPUART_CR_DMA_RX_EN_SHIFT)|\
                                        (LPUART_CR_RX_THLD_MASK << LPUART_CR_RX_THLD_SHIFT))


//------------------------------------------------------------------------------
//0x04 LPUART_BRR
//------------------------------------------------------------------------------
#define LPUART_BRR_MASK             (0xFFFFFFFF)
#define LPUART_BRR_SHITF            (0)

#define LPUART_BRR_REG_MASK         (LPUART_BRR_MASK << LPUART_BRR_SHITF)

//------------------------------------------------------------------------------
//0x08 LPUART_SR
//------------------------------------------------------------------------------
//BIT[0] TX EN
#define LPUART_SR_TX_EN_MASK                       (0x1)
#define LPUART_SR_TX_EN_SHIFT                      (0)

//BIT[1] TX BUSY
#define LPUART_SR_TX_BUSY_MASK                     (0x1)
#define LPUART_SR_TX_BUSY_SHIFT                    (1)

//BIT[2] RX EN
#define LPUART_SR_RX_EN_MASK                       (0x1)
#define LPUART_SR_RX_EN_SHIFT                      (2)

//BIT[3] RX START OK
#define LPUART_SR_RX_START_OK_MASK                 (0x1)
#define LPUART_SR_RX_START_OK_SHIFT                (3)

//BIT[4] RX START ERROR
#define LPUART_SR_RX_START_ERR_MASK                (0x1)
#define LPUART_SR_RX_START_ERR_SHIFT               (4)

//BIT[5] RX PARITY ERROR
#define LPUART_SR_RX_PARITY_ERR_MASK               (0x1)
#define LPUART_SR_RX_PARITY_ERR_SHIFT              (5)

//BIT[6] RX STOP ERROR
#define LPUART_SR_RX_STOP_ERR_MASK                 (0x1)
#define LPUART_SR_RX_STOP_ERR_SHIFT                (6)

//BIT[7] RX FIFO EMPTY
#define LPUART_SR_RX_FIFO_EMPTY_MASK               (0x1)
#define LPUART_SR_RX_FIFO_EMPTY_SHIFT              (7)

//BIT[8:10] RX FIFO CNT
#define LPUART_SR_RX_FIFO_CNT_MASK                 (0x7)
#define LPUART_SR_RX_FIFO_CNT_SHIFT                (8)

#define LPUART_SR_REG_MASK             ((LPUART_SR_TX_EN_MASK << LPUART_SR_TX_EN_SHIFT)|\
                                        (LPUART_SR_TX_BUSY_MASK << LPUART_SR_TX_BUSY_SHIFT)|\
                                        (LPUART_SR_RX_EN_MASK << LPUART_SR_RX_EN_SHIFT)|\
                                        (LPUART_SR_RX_START_OK_MASK << LPUART_SR_RX_START_OK_SHIFT)|\
                                        (LPUART_SR_RX_START_ERR_MASK << LPUART_SR_RX_START_ERR_SHIFT)|\
                                        (LPUART_SR_RX_PARITY_ERR_MASK << LPUART_SR_RX_PARITY_ERR_SHIFT)|\
                                        (LPUART_SR_RX_STOP_ERR_MASK << LPUART_SR_RX_STOP_ERR_SHIFT)|\
                                        (LPUART_SR_RX_FIFO_EMPTY_MASK << LPUART_SR_RX_FIFO_EMPTY_SHIFT)|\
                                        (LPUART_SR_RX_FIFO_CNT_MASK << LPUART_SR_RX_FIFO_CNT_SHIFT))

//------------------------------------------------------------------------------
//0x0C LPUART_ISR
//------------------------------------------------------------------------------
//BIT[0]
#define LPUART_ISR_TX_DONE_MASK                    (0x1)
#define LPUART_ISR_TX_DONE_SHIFT                   (0)

//BIT[1]
#define LPUART_ISR_RX_DONE_MASK                    (0x1)
#define LPUART_ISR_RX_DONE_SHITF                   (1)

//BIT[2]
#define LPUART_ISR_RX_START_ERR_MASK               (0x1)
#define LPUART_ISR_RX_START_ERR_SHIFT              (2)

//BIT[3]
#define LPUART_ISR_RX_PARITY_ERR_MASK              (0x1)
#define LPUART_ISR_RX_PARITY_ERR_SHIFT             (3)

//BIT[4]
#define LPUART_ISR_RX_STOP_ERR_MASK                (0x1)
#define LPUART_ISR_RX_STOP_ERR_SHIFT               (4)

//BIT[5]
#define LPUART_ISR_RX_FIFO_OVERFLOW_MASK           (0x1)
#define LPUART_ISR_RX_FIFO_OVERFLOW_SHIFT          (5)

//BIT[6]
#define LPUART_ISR_RX_FIFO_REACH_THLD_MASK         (0x1)
#define LPUART_ISR_RX_FIFO_REACH_THLD_SHIFT        (6)


#define LPUART_ISR_REG_MASK          ((LPUART_ISR_TX_DONE_MASK << LPUART_ISR_TX_DONE_SHIFT)|\
                                        (LPUART_ISR_RX_DONE_MASK << LPUART_ISR_RX_DONE_SHITF)|\
                                        (LPUART_ISR_RX_START_ERR_MASK << LPUART_ISR_RX_START_ERR_SHIFT)|\
                                        (LPUART_ISR_RX_PARITY_ERR_MASK << LPUART_ISR_RX_PARITY_ERR_SHIFT)|\
                                        (LPUART_ISR_RX_STOP_ERR_MASK << LPUART_ISR_RX_STOP_ERR_SHIFT)|\
                                        (LPUART_ISR_RX_FIFO_OVERFLOW_MASK << LPUART_ISR_RX_FIFO_OVERFLOW_SHIFT)|\
                                        (LPUART_ISR_RX_FIFO_REACH_THLD_MASK << LPUART_ISR_RX_FIFO_REACH_THLD_SHIFT))

//------------------------------------------------------------------------------
//0x10 LPUART_IER
//------------------------------------------------------------------------------
//The same with int en
#define LPUART_INT_FLAH_REG_MASK        (LPUART_ISR_REG_MASK)

//------------------------------------------------------------------------------
//0x14 TX FIFO
//------------------------------------------------------------------------------
//it will changed by data len config
#define LPUART_TBR_REG_MASK             (0xFF)

//------------------------------------------------------------------------------
//0x18 RX FIFO
//------------------------------------------------------------------------------
//it will changed by data len config
#define LPUART_RBR_REG_MASK             (0xFF)

#endif
