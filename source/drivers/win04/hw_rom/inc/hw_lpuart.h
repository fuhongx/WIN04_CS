/**
 * ****************************************************************************
 * @file hw_lpuart.h
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

#ifndef __HW_LPUART_H__
#define __HW_LPUART_H__

#define LPUART_MAX_BAUDRATE (9600U)  // Max baudrate for LPUART
#define LPUART_RX_THLD_MAX  (4)      // Max RX FIFO reach threshold for LPUART

typedef enum
{
    EN_LPUART_DATALEN_5BIT = 0x0,
    EN_LPUART_DATALEN_6BIT = 0x1,
    EN_LPUART_DATALEN_7BIT = 0x2,
    EN_LPUART_DATALEN_8BIT = 0x3,
} EN_LPUART_DATA_LEN_T;

typedef enum
{
    EN_LPUART_STOP_LEN_1BIT = 0x0,
    EN_LPUART_STOP_LEN_2BIT = 0x1,
    EN_LPUART_STOP_LEN_3BIT = 0x2,
    EN_LPUART_STOP_LEN_4BIT = 0x3,
} EN_LPUART_STOP_LEN_T;

typedef enum
{
    EN_LPUART_PARITY_ODD =  0x0,
    EN_LPUART_PARITY_EVEN = 0x1,
} EN_LPUART_PARITY_T;

typedef enum
{
    EN_LPUART_INT_TX_DONE = LPUART_ISR_TX_DONE_MASK << LPUART_ISR_TX_DONE_SHIFT,
    EN_LPUART_INT_RX_DONE = LPUART_ISR_RX_DONE_MASK << LPUART_ISR_RX_DONE_SHITF,
    EN_LPUART_INT_RX_START_ERROR = LPUART_ISR_RX_START_ERR_MASK << LPUART_ISR_RX_START_ERR_SHIFT,
    EN_LPUART_INT_RX_PARITY_ERROR = LPUART_ISR_RX_PARITY_ERR_MASK << LPUART_ISR_RX_PARITY_ERR_SHIFT,
    EN_LPUART_INT_RX_STOP_ERROR = LPUART_ISR_RX_STOP_ERR_MASK << LPUART_ISR_RX_STOP_ERR_SHIFT,
    EN_LPUART_INT_RX_FIFO_OVERFLOW = LPUART_ISR_RX_FIFO_OVERFLOW_MASK << LPUART_ISR_RX_FIFO_OVERFLOW_SHIFT,
    EN_LPUART_INT_RX_FIFO_RICH_THLD = LPUART_ISR_RX_FIFO_REACH_THLD_MASK << LPUART_ISR_RX_FIFO_REACH_THLD_SHIFT,
} EN_LPUART_INT_T;

typedef enum
{
    EN_LPUART_STA_TX_EN = LPUART_SR_TX_EN_MASK << LPUART_SR_TX_EN_SHIFT,
    EN_LPUART_STA_TX_BUSY = LPUART_SR_TX_BUSY_MASK << LPUART_SR_TX_BUSY_SHIFT,
    EN_LPUART_STA_RX_EN = LPUART_SR_RX_EN_MASK << LPUART_SR_RX_EN_SHIFT,
    EN_LPUART_STA_RX_START_OK = LPUART_SR_RX_START_OK_MASK << LPUART_SR_RX_START_OK_SHIFT,
    EN_LPUART_STA_RX_START_FAULT = LPUART_SR_RX_START_ERR_MASK << LPUART_SR_RX_START_ERR_SHIFT,
    EN_LPUART_PARITY_ERROR = LPUART_SR_RX_PARITY_ERR_MASK << LPUART_SR_RX_PARITY_ERR_SHIFT,
    EN_LPUART_RX_STOP_ERROR = LPUART_SR_RX_STOP_ERR_MASK << LPUART_SR_RX_STOP_ERR_SHIFT,
    EN_LPUART_RX_EMPTY = LPUART_SR_RX_FIFO_EMPTY_MASK << LPUART_SR_RX_FIFO_EMPTY_SHIFT,
} EN_LPUART_STA_T;

typedef struct
{
    bool bTxEn;
    bool bRxEn;
    EN_LPUART_DATA_LEN_T enDataLen;
    EN_LPUART_STOP_LEN_T enStopLen;
    bool bParityEn;
    EN_LPUART_PARITY_T enParity;
    uint8_t u8RxThld; // 接收fifo阈值，范围1~4
    bool bDmaTxEn;    // 发送模块DMA使能
    bool bDmaRxEn;    // 接收模块DMA使能
} stLpUartInit_t;

EN_ERR_STA_T rom_hw_lpuart_set_baudrate(uint32_t u32BaudRate);
EN_ERR_STA_T rom_hw_lpuart_init(stLpUartInit_t *pstInit, uint32_t u32Baud);
EN_ERR_STA_T rom_hw_lpuart_send_data(uint8_t *pu8Buffer, uint16_t u16Len);
EN_ERR_STA_T rom_hw_lpuart_recv_data(uint8_t *pu8Buffer, uint16_t *len);
EN_ERR_STA_T rom_hw_lpuart_get_status(uint16_t *pu16Sta);
EN_ERR_STA_T rom_hw_lpuart_enable_interrupt(uint8_t u8Mask);
EN_ERR_STA_T rom_hw_lpuart_get_interrupt_flag(uint8_t *pu8Flag);
EN_ERR_STA_T rom_hw_lpuart_clear_interrup(uint8_t u8Flag);
void rom_hw_lpuart_disable_irq(uint8_t irq_mask);
void rom_hw_lpuart_enable_dma(uint8_t trx_dir);

#endif
