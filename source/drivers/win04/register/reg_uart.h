/**
 * ****************************************************************************
 * @file reg_uart.h
 * @author SLC(SC) software team. 
 * @brief This driver is only applicable to UART with FIFO, lpuart is not changed.
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */


#ifndef __REG_UART_H__
#define __REG_UART_H__

/**
 * @brief Uart reg list define.
 * 
 */
typedef struct
{
    __IOM uint32_t  DATA_FIFO_AND_CLK_DLV_LO;        // 0x00
    __IOM uint32_t  INT_EN_AND_CLK_DIV_HI;           // 0x04
    __IOM uint32_t  FIFO_CTRL_AND_INT_FLAG;      // 0x08
    __IOM uint32_t  LINE_CTRL;                   // 0x0C
    __IOM uint32_t  MODEM_CTRL;                  // 0x10
    __IOM uint32_t  LINE_STA;                    // 0x14
    __IOM uint32_t  MODEM_STA;                    // 0x18
    __IOM uint32_t  DEBUG_SCR;                   // 0x1C
    __IOM uint32_t  RES1[23];                    // 0x20 ~ 0x78
    __IOM uint32_t  UART_STA;                     // 0x7C
    __IOM uint32_t  TX_FIFO_CNT;                // 0x80
    __IOM uint32_t  RX_FIFO_CNT;                  // 0x84
    __IOM uint32_t  RES2[8];                    // 0x88 ~ 0xA4
    __IOM uint32_t  DMA_SW_ACK;                 // 0xA8
    __IOM uint32_t  RSTX_CTRL;                    // 0xAC
    __IOM uint32_t  SW_DE_ENABLE;               // 0xB0
    __IOM uint32_t  RES3[1];                    // 0xB4
    __IOM uint32_t  DE_TIMING_CFG;              // 0xB8
    __IOM uint32_t  RES4[1];                    // 0xBC
    __IOM uint32_t  CLK_DIV_FRACTION;           // 0xC0
}stUartHandle_t, *pstUartHandle;

#define IS_VALID_UART_HADNLE(pstHandle)     ((pstHandle == UART0) | (pstHandle == UART1))

#define IS_VALID_IR_HANDLE(pstHandle)       ((pstHandle == UART1))

#define IS_VALID_LPUART_HANDLE(pstHandle)   ((pstHandle == LPUART))

//------------------------------------------------------------------------------
//Start UART Define
//0x00 DATA_FIFO_AND_CLK_DLV_LO
//------------------------------------------------------------------------------
#define UART_DATA_FIFO_REG_MASK                   (0xFF)

#define UART_CLK_DIV_LO_REG_MASK                (0xFF)

//------------------------------------------------------------------------------
//0x04 INT_EN_AND_CLK_DIV_HI
//------------------------------------------------------------------------------

//BIT[0]
#define UART_INT_EN_RX_START_MASK                      (0x1)
#define UART_INT_EN_RX_START_SHIFT                     (0)

//BIT[1]
#define UART_INT_EN_TX_FIFO_EMPTY_MASK                 (0x1)
#define UART_INT_EN_TX_FIFO_EMPTY_SHIFT                (1)

//BIT[2]
#define UART_INT_EN_LINE_STA_MASK                      (0x1)
#define UART_INT_EN_LINE_STA_SHIFT                     (2)

//BIT[3]
#define UART_INT_EN_CTS_MASK                           (0x1)
#define UART_INT_EN_CTS_SHIFT                          (3)

//BIT[4]
#define UART_INT_EN_READ_CLEAN_MODE_MASK                   (0x1)
#define UART_INT_EN_READ_CLEAN_MODE_SHIFT                  (4)

//BIT[7]
#define UART_INT_EN_TX_FIFO_UNDER_THLD_MASK                (0x1)
#define UART_INT_EN_TX_FIFO_UNDER_THLD_SHIFT               (7)

#define UART_INT_EN_REG_MASK                ((UART_INT_EN_RX_START_MASK << UART_INT_EN_RX_START_SHIFT)|\
                                            (UART_INT_EN_TX_FIFO_EMPTY_MASK << UART_INT_EN_TX_FIFO_EMPTY_SHIFT)|\
                                            (UART_INT_EN_LINE_STA_MASK << UART_INT_EN_LINE_STA_SHIFT)|\
                                            (UART_INT_EN_CTS_MASK << UART_INT_EN_CTS_SHIFT)|\
                                            (UART_INT_EN_READ_CLEAN_MODE_MASK << UART_INT_EN_READ_CLEAN_MODE_SHIFT)|\
                                            (UART_INT_EN_TX_FIFO_UNDER_THLD_MASK << UART_INT_EN_TX_FIFO_UNDER_THLD_SHIFT))


#define UART_CLK_DIV_HI_REG_MASK                    (0xFF)

//------------------------------------------------------------------------------
//0x08 FIFO_CTRL_AND_INT_FLAG
//------------------------------------------------------------------------------
//BIT[0]
#define UART_FIFO_CTRL_FIFO_EN_MASK                 (0x1)
#define UART_FIFO_CTRL_FIFO_EN_SHIFT                (0)

//BIT[1]
#define UART_FIFO_CTRL_RX_FIFO_RST_MASK             (0x1)
#define UART_FIFO_CTRL_RX_FIFO_RST_SHIFT            (1)

//BIT[2]
#define UART_FIFO_CTRL_TX_FIFO_RST_MASK             (0x1)
#define UART_FIFO_CTRL_TX_FIFO_RST_SHIFT            (2)

//BIT[3]
#define UART_FIFO_CTRL_DMA_MODE_MASK                (0x1)
#define UART_FIFO_CTRL_DMA_MODE_SHIFT               (3)

//BIT[4:5]
#define UART_FIFO_CTRL_TX_FIFO_THLD_MASK            (0x3)
#define UART_FIFO_CTRL_TX_FIFO_THLD_SHIFT           (4)

//BIT[6:7]
#define UART_FIFO_CTRL_RX_FIFO_THLD_MASK            (0x3)
#define UART_FIFO_CTRL_RX_FIFO_THLD_SHIFT           (6)


#define UART_FIFO_CTRL_REG_MASK                 ((UART_FIFO_CTRL_FIFO_EN_MASK << UART_FIFO_CTRL_FIFO_EN_SHIFT)|\
                                                (UART_FIFO_CTRL_RX_FIFO_RST_MASK << UART_FIFO_CTRL_RX_FIFO_RST_SHIFT)|\
                                                (UART_FIFO_CTRL_TX_FIFO_RST_MASK << UART_FIFO_CTRL_TX_FIFO_RST_SHIFT)|\
                                                (UART_FIFO_CTRL_DMA_MODE_MASK << UART_FIFO_CTRL_DMA_MODE_SHIFT)|\
                                                (UART_FIFO_CTRL_TX_FIFO_THLD_MASK << UART_FIFO_CTRL_TX_FIFO_THLD_SHIFT)|\
                                                (UART_FIFO_CTRL_RX_FIFO_THLD_MASK << UART_FIFO_CTRL_RX_FIFO_THLD_SHIFT))

//BIT[0:3]
#define UART_INT_FLAG_SRC_MASK                      (0xF)
#define UART_INT_FLAG_SRC_SHIFT                     (0)

//BIT[6:7]
#define UART_INT_FLAG_FIFO_STA_MASK                 (0x3)
#define UART_INT_FLAG_FIFO_STA_SHIFT                (6)

#define UART_INT_FLAG_REG_MASK                      ((UART_INT_FLAG_SRC_MASK << UART_INT_FLAG_SRC_SHIFT)|\
                                                    (UART_INT_FLAG_FIFO_STA_MASK << UART_INT_FLAG_FIFO_STA_SHIFT))


#define UART_TX_BUFFER_MASK                     (0xFF)
#define UART_TX_BUFFER_SHIFT                    (0)
#define UART_TX_BUFFER_REG_MASK             (UART_TX_BUFFER_MASK << UART_TX_BUFFER_SHIFT)

//------------------------------------------------------------------------------
//0x0C LINE_CTRL
//------------------------------------------------------------------------------
//BIT[0:1]
#define UART_LINE_CTRL_DATA_LEN_MASK            (0x3)
#define UART_LINE_CTRL_DATA_LEN_SHIFT           (0)

//BIT[2]
#define UART_LINE_CTRL_STOP_LEN_MASK            (0x1)
#define UART_LINE_CTRL_STOP_LEN_SHIFT           (2)

//BIT[3]
#define UART_LINE_CTRL_PARITY_EN_MASK           (0x1)
#define UART_LINE_CTRL_PARITY_EN_SHIFT          (3)

//BIT[4]
#define UART_LINE_CTRL_EVEN_PARITY_EN_MASK      (0x1)
#define UART_LINE_CTRL_EVEN_PARITY_EN_SHIFT     (4)

//BIT[5]
#define UART_LINE_CTRL_FORCE_PARITY_MASK        (0x1)
#define UART_LINE_CTRL_FORCE_PARITY_SHIFT       (5)

//BIT[6]
#define UART_LINE_CTRL_BRAEK_CTRL_EN_MASK       (0x1)
#define UART_LINE_CTRL_BRAEK_CTRL_EN_SHIFT      (6)

//BIT[7]
#define UART_LINE_CTRL_DIV_REG_EN_MASK          (0x1)
#define UART_LINE_CTRL_DIV_REG_EN_SHIFT         (7)

#define UART_LINE_CTRL_REG_MASK                 ((UART_LINE_CTRL_DATA_LEN_MASK << UART_LINE_CTRL_DATA_LEN_SHIFT)|\
                                                (UART_LINE_CTRL_STOP_LEN_MASK << UART_LINE_CTRL_STOP_LEN_SHIFT)|\
                                                (UART_LINE_CTRL_PARITY_EN_MASK << UART_LINE_CTRL_PARITY_EN_SHIFT)|\
                                                (UART_LINE_CTRL_EVEN_PARITY_EN_MASK << UART_LINE_CTRL_EVEN_PARITY_EN_SHIFT)|\
                                                (UART_LINE_CTRL_FORCE_PARITY_MASK << UART_LINE_CTRL_FORCE_PARITY_SHIFT)|\
                                                (UART_LINE_CTRL_BRAEK_CTRL_EN_MASK << UART_LINE_CTRL_BRAEK_CTRL_EN_SHIFT)|\
                                                (UART_LINE_CTRL_DIV_REG_EN_MASK << UART_LINE_CTRL_DIV_REG_EN_SHIFT))

//------------------------------------------------------------------------------
//0x10 MODEM_CTRL
//------------------------------------------------------------------------------
//BIT[1]
#define UART_MODEM_CTRL_RTS_EN_MASK                 (0x1)
#define UART_MODEM_CTRL_RTS_EN_SHIFT                (1)

//BIT[4]
#define UART_MODEM_CTRL_LOOP_EN_MASK                (0x1)
#define UART_MODEM_CTRL_LOOP_EN_SHIFT               (4)

//BIT[5]
#define UART_MODEM_CTRL_AUTO_RTS_CTS_EN_MASK        (0x1)
#define UART_MODEM_CTRL_AUTO_RTS_CTS_EN_SHIFT       (5)

#define UART_MODEM_CTRL_REG_MASK            ((UART_MODEM_CTRL_RTS_EN_MASK << UART_MODEM_CTRL_RTS_EN_SHIFT)|\
                                            (UART_MODEM_CTRL_LOOP_EN_MASK << UART_MODEM_CTRL_LOOP_EN_SHIFT)|\
                                            (UART_MODEM_CTRL_AUTO_RTS_CTS_EN_MASK << UART_MODEM_CTRL_AUTO_RTS_CTS_EN_SHIFT))

//------------------------------------------------------------------------------
//0x14 LINE_STA
//------------------------------------------------------------------------------
#define UART_LINE_STA_REG_LEN                       (8)
//BIT[0]
#define UART_LINE_STA_DATA_READY_MASK               (0x1)
#define UART_LINE_STA_DATA_READY_SHIFT              (0)

//BIT[1]
#define UART_LINE_STA_RX_FIFO_OVERFLOW_MASK            (0x1)
#define UART_LINE_STA_RX_FIFO_OVERFLOW_SHIFT           (1)

//BIT[2]
#define UART_LINE_STA_PARITY_ERROR_MASK             (0x1)
#define UART_LINE_STA_PARITY_ERROR_SHIFT            (2)

//BIT[3]
#define UART_LINE_STA_STOP_BIT_ERROR_MASK           (0x1)
#define UART_LINE_STA_STOP_BIT_ERROR_SHIFT          (3)

//BIT[4]
#define UART_LINE_STA_LINE_BREAK_MASK               (0x1)
#define UART_LINE_STA_LINE_BREAK_SHIFT              (4)

//BIT[5]
#define UART_LINE_STA_TX_FIFO_UNDER_THLD_MASK       (0x1)
#define UART_LINE_STA_TX_FIFO_UNDER_THLD_SHIFT      (5)

//BIT[6]
#define UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_MASK   (0x1)
#define UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_SHIFT  (6)

//BIT[7]
#define UART_LINE_STA_RX_FIFO_PARITY_ERROR_MASK     (0x1)
#define UART_LINE_STA_RX_FIFO_PARITY_ERROR_SHIFT    (7)


#define UART_LINE_STA_REG_MASK          ((UART_LINE_STA_DATA_READY_MASK << UART_LINE_STA_DATA_READY_SHIFT)|\
                                        (UART_LINE_STA_RX_FIFO_OVERFLOW_MASK << UART_LINE_STA_RX_FIFO_OVERFLOW_SHIFT)|\
                                        (UART_LINE_STA_PARITY_ERROR_MASK << UART_LINE_STA_PARITY_ERROR_SHIFT)|\
                                        (UART_LINE_STA_STOP_BIT_ERROR_MASK << UART_LINE_STA_STOP_BIT_ERROR_SHIFT)|\
                                        (UART_LINE_STA_LINE_BREAK_MASK << UART_LINE_STA_LINE_BREAK_SHIFT)|\
                                        (UART_LINE_STA_TX_FIFO_UNDER_THLD_MASK << UART_LINE_STA_TX_FIFO_UNDER_THLD_SHIFT)|\
                                        (UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_MASK << UART_LINE_STA_TX_DONE_AND_FIFO_EMPTY_SHIFT)|\
                                        (UART_LINE_STA_RX_FIFO_PARITY_ERROR_MASK << UART_LINE_STA_RX_FIFO_PARITY_ERROR_SHIFT))
//------------------------------------------------------------------------------
//0x18 MODEM_STA
//------------------------------------------------------------------------------
//BIT[0]
#define UART_MODEM_STA_CTS_CHANGE_MASK                   (0x1)
#define UART_MODEM_STA_CTS_CHANGE_SHIFT                  (0)

//BIT[4]
#define UART_MODEM_STA_CST_LEVEL_MASK                    (0x1)
#define UART_MODEM_STA_CST_LEVEL_SHIFT                   (4)

#define UART_MODEM_STA_REG_MASK                 ((UART_MODEM_STA_CTS_CHANGE_MASK << UART_MODEM_STA_CTS_CHANGE_SHIFT)|\
                                                (UART_MODEM_STA_CST_LEVEL_MASK << UART_MODEM_STA_CST_LEVEL_SHIFT))

//------------------------------------------------------------------------------
//0x7C UART_STA
//------------------------------------------------------------------------------
//BIT[0]
#define UART_STA_BUSY_MASK                              (0x1)
#define UART_STA_BUSY_SHIFT                             (0)

//BIT[1]
#define UART_STA_TX_FIFO_NOT_FULL_MASK                 (0x1)
#define UART_STA_TX_FIFO_NOT_FULL_SHIFT                (1)

//BIT[2]
#define UART_STA_TX_FIFO_EMPTY_MASK                     (0x1)
#define UART_STA_TX_FIFO_EMPTY_SHIFT                    (2)

//BIT[3]
#define UART_STA_RX_FIFO_NOT_EMPTY_MASK                 (0x1)
#define UART_STA_RX_FIFO_NOT_EMPTY_SHIFT                (3)

//BIT[4]
#define UART_STA_RX_FIFO_FULL_MASK                      (0x1)
#define UART_STA_RX_FIFO_FULL_SHIFT                     (4)


#define UART_STA_REG_MASK                       ((UART_STA_BUSY_MASK << UART_STA_BUSY_SHIFT)|\
                                                (UART_STA_TX_FIFO_NOT_FULL_MASK << UART_STA_TX_FIFO_NOT_FULL_SHIFT)|\
                                                (UART_STA_TX_FIFO_EMPTY_MASK << UART_STA_TX_FIFO_EMPTY_SHIFT)|\
                                                (UART_STA_RX_FIFO_NOT_EMPTY_MASK << UART_STA_RX_FIFO_NOT_EMPTY_SHIFT)|\
                                                (UART_STA_RX_FIFO_FULL_MASK << UART_STA_RX_FIFO_FULL_SHIFT))

//------------------------------------------------------------------------------
//0x80 TX_FIFO_CNT
//------------------------------------------------------------------------------
#define UART_TX_FIFO_CNT_REG_MASK           (0xFF)

//------------------------------------------------------------------------------
//0x84 RX_FIFO_CNT
//------------------------------------------------------------------------------
#define UART_RX_FIFO_CNT_REG_MASK           (0xFF)

//------------------------------------------------------------------------------
//0xA8 DMA_SW_ACK
//------------------------------------------------------------------------------
#define UART_DMA_SW_ACK_REG_MASK            (0x1)

//------------------------------------------------------------------------------
//0xAC RSTX_CTRL
//------------------------------------------------------------------------------
//BIT[0]
#define UART_RSTX_CTRL_RS485_EN_MASK          (0x1)
#define UART_RSTX_CTRL_RS485_EN_SHIFT         (0)

//BIT[2]
#define UART_RSTX_CTRL_DE_OUT_LEVEL_MASK      (0x1)
#define UART_RSTX_CTRL_DE_OUT_LEVEL_SHIFT     (2)

//BIT[3:4]
#define UART_RSTX_CTRL_TX_MODE_MASK           (0x3)
#define UART_RSTX_CTRL_TX_MODE_SHIFT          (3)

#define UART_RSTX_CTRL_REG_MASK           ((UART_RSTX_CTRL_RS485_EN_MASK << UART_RSTX_CTRL_RS485_EN_SHIFT)|\
                                        (UART_RSTX_CTRL_DE_OUT_LEVEL_MASK << UART_RSTX_CTRL_DE_OUT_LEVEL_SHIFT)|\
                                        (UART_RSTX_CTRL_TX_MODE_MASK << UART_RSTX_CTRL_TX_MODE_SHIFT))

//------------------------------------------------------------------------------
//0xB0 SW_DE_ENABLE
//------------------------------------------------------------------------------
#define UART_SW_DE_ENABLE_REG_MASK          (0x1)

//------------------------------------------------------------------------------
//0xB8 DE_TIMING_CFG
//------------------------------------------------------------------------------
//BIT[0:7]
#define UART_DE_TIMING_CFG_TX_DELAY_TIME_MASK      (0xFF)
#define UART_DE_TIMING_CFG_TX_DELAY_TIME_SHIFT     (0)

//BIT[16:23]
#define UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_MASK      (0xFF)
#define UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_SHIFT     (16)


#define UART_DE_TIMING_CFG_REG_MASK         ((UART_DE_TIMING_CFG_TX_DELAY_TIME_MASK << UART_DE_TIMING_CFG_TX_DELAY_TIME_SHIFT)|\
                                            (UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_MASK << UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_SHIFT))

//------------------------------------------------------------------------------
//0xC0 CLK_DIV_FRACTION
//------------------------------------------------------------------------------
#define UART_CLK_DIV_FRACTION_REG_MASK      (0xF)

#endif
