/**
 * ****************************************************************************
 * @file hw_lpuart.c
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

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_lpuart.h"
#include "hw_sysctrl.h"

#define LPUART_SEND_TIMEOUT_CNT     (0x8000)
#define LPUART_RECV_TIMEOUT_CNT     (0x1000)

EN_ERR_STA_T rom_hw_lpuart_set_baudrate(uint32_t u32BaudRate)
{
    if (u32BaudRate > LPUART_MAX_BAUDRATE)
        return EN_ERROR_STA_INVALID;

    uint32_t u32Div = (SYS_CLK_FREQ_32K << 16) / u32BaudRate;

    LPUART->LPUART_BRR = u32Div;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_init(stLpUartInit_t *pstInit, uint32_t u32Baud)
{
    EN_ERR_STA_T enRet = EN_ERROR_STA_OK;

    if (pstInit->u8RxThld > LPUART_RX_THLD_MAX)
        return EN_ERROR_STA_INVALID;

    // [BUG#287] 初始FIFO阈值为0，跨时钟域配置时导致产生了1次req，启动DMA会搬1字节空数据
    LPUART->LPUART_CR = 0x100000;

    enRet = rom_hw_lpuart_set_baudrate(u32Baud);
    if (EN_ERROR_STA_OK != enRet)
    {
        return enRet;
    }

    rom_utility_delay_us(LP_RW_DELAY);
    uint32_t u32Reg = LPUART->LPUART_CR;
    // DataLen
    u32Reg &= ~(LPUART_CR_DATA_LEN_MASK << LPUART_CR_DATA_LEN_SHIFT);
    u32Reg |= ((pstInit->enDataLen & LPUART_CR_DATA_LEN_MASK) << LPUART_CR_DATA_LEN_SHIFT);

    // Stop len
    u32Reg &= ~(LPUART_CR_STOP_LEN_MASK << LPUART_CR_STOP_LEN_SHIFT);
    u32Reg |= (pstInit->enStopLen << LPUART_CR_STOP_LEN_SHIFT);

    // Parity
    u32Reg &= ~((LPUART_CR_PARITY_EN_MASK << LPUART_CR_PARITY_EN_SHIFT) | (LPUART_CR_PARITY_EVEN_MASK << LPUART_CR_PARITY_EVEN_SHIFT));
    u32Reg |= (pstInit->bParityEn << LPUART_CR_PARITY_EN_SHIFT) | (pstInit->enParity << LPUART_CR_PARITY_EVEN_SHIFT);

    // RxThld
    u32Reg &= ~(LPUART_CR_RX_THLD_MASK << LPUART_CR_RX_THLD_SHIFT);
    u32Reg |= ((pstInit->u8RxThld & LPUART_CR_RX_THLD_MASK) << LPUART_CR_RX_THLD_SHIFT);

    // Dma
    u32Reg &= ~((LPUART_CR_DMA_TX_EN_MASK << LPUART_CR_DMA_TX_EN_SHIFT) | (LPUART_CR_DMA_RX_EN_MASK << LPUART_CR_DMA_RX_EN_SHIFT));
    u32Reg |= (pstInit->bDmaTxEn << LPUART_CR_DMA_TX_EN_SHIFT) | (pstInit->bDmaRxEn << LPUART_CR_DMA_RX_EN_SHIFT);

    // Enable
    u32Reg &= ~((LPUART_CR_TX_EN_MASK << LPUART_CR_TX_EN_SHIFT) | (LPUART_CR_RX_EN_MASK << LPUART_CR_RX_EN_SHIFT));
    u32Reg |= (pstInit->bTxEn & LPUART_CR_TX_EN_MASK) << LPUART_CR_TX_EN_SHIFT;
    u32Reg |= (pstInit->bRxEn & LPUART_CR_RX_EN_MASK) << LPUART_CR_RX_EN_SHIFT;

    LPUART->LPUART_ISR = LPUART_ISR_REG_MASK; // 清除中断标志
    LPUART->LPUART_CR = u32Reg;

    rom_utility_delay_us(LP_RW_DELAY);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_put_char(uint8_t val)
{
    uint32_t cnt = 0;

    LPUART->LPUART_TBR = val;

    do {
        // 读取寄存器需要至少2个32K时钟周期，跨时钟域的规避
        rom_utility_delay_us(LP_RW_DELAY);

        if ((LPUART->LPUART_SR & EN_LPUART_STA_TX_BUSY) == 0)
            break;
    } while (cnt++ < LPUART_SEND_TIMEOUT_CNT);

    if (cnt >= LPUART_SEND_TIMEOUT_CNT)
        return EN_ERROR_STA_TIMEOUT;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_get_char(uint8_t *val)
{
    uint32_t cnt = 0;

    do {
        // 至少等待2个32K时钟周期才可以读取寄存器，跨时钟域的规避
        rom_utility_delay_us(LP_RW_DELAY);

        if ((LPUART->LPUART_SR & EN_LPUART_RX_EMPTY) == 0)
            break;
    } while (cnt++ < LPUART_RECV_TIMEOUT_CNT);

    // 使用1200波特率时，1个bit约834us时间，一个字节大概需要9.2ms，超时时间为20ms相对合理
    if (cnt >= LPUART_RECV_TIMEOUT_CNT)
        return EN_ERROR_STA_TIMEOUT;

    *val = (uint8_t)(LPUART->LPUART_RBR);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_send_data(uint8_t *pu8Buffer, uint16_t u16Len)
{
    // Hw issue, don't have tx fifo info for using.
    EN_ERR_STA_T ret = EN_ERROR_STA_OK;
    uint16_t idx = 0;

    for (idx = 0; (idx < u16Len) && !ret; idx++)
        ret = rom_hw_lpuart_put_char(pu8Buffer[idx]);

    return ret;
}

EN_ERR_STA_T rom_hw_lpuart_recv_data(uint8_t *pu8Buffer, uint16_t *len)
{
    if ((NULL == pu8Buffer) | (NULL == len))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint16_t idx = 0;
    uint16_t max_len = *len;
    EN_ERR_STA_T ret = EN_ERROR_STA_OK;
    *len = 0;

    if (max_len == 0)
    {
        return EN_ERROR_STA_INVALID;
    }

    for (idx = 0; (idx <= max_len) && !ret; idx++)
        ret = rom_hw_lpuart_get_char(pu8Buffer++);

    *len = (idx - 1);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_get_status(uint16_t *pu16Sta)
{
    if (NULL == pu16Sta)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu16Sta = LPUART->LPUART_SR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_enable_interrupt(uint8_t u8Mask)
{
    LPUART->LPUART_IER = u8Mask & LPUART_ISR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_get_interrupt_flag(uint8_t *pu8Flag)
{
    if (NULL == pu8Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = LPUART->LPUART_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_lpuart_clear_interrup(uint8_t u8Flag)
{
    LPUART->LPUART_ISR = u8Flag;
    rom_utility_delay_us(LP_RW_DELAY);

    return EN_ERROR_STA_OK;
}

void rom_hw_lpuart_disable_irq(uint8_t irq_mask)
{
    uint32_t mask = LPUART->LPUART_IER;
    mask &= ~(irq_mask & LPUART_ISR_REG_MASK);
    LPUART->LPUART_IER = (mask & LPUART_ISR_REG_MASK);
}

void rom_hw_lpuart_enable_dma(uint8_t trx_dir)
{
    uint32_t u32Reg = LPUART->LPUART_CR;
    // Dma
    if (trx_dir) {
        u32Reg |= (1U << LPUART_CR_DMA_TX_EN_SHIFT);
    } else {
        u32Reg |= (1U << LPUART_CR_DMA_RX_EN_SHIFT);
    }

    LPUART->LPUART_CR = u32Reg;
    rom_utility_delay_us(LP_RW_DELAY);
}
