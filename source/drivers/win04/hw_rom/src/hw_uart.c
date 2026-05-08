/**
 * ****************************************************************************
 * @file hw_uart.c
 * @author SLC(SC) software team. 
 * @brief This driver is only applicable to UART with FIFO, lpuart is not changed.
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_uart.h"

#ifdef BOOT2_TEST

uint32_t SystemCoreClock = SYS_CLK_FREQ_50M;

#else
//Add in chip_name.c - uint32_t SystemCoreClock
extern uint32_t SystemCoreClock;
#endif

/**
 * @brief [20241227]-1 Hw Design - Need consider uart busy or not.
 * 
 * @param pstHandle 
 * @return true : Enable clock div reg.
 * @return false : Disable clock div reg.
 */
bool rom_hw_uart_get_clock_div_reg_status(stUartHandle_t *pstHandle)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    return (pstHandle->LINE_CTRL & UART_LINE_CTRL_DIV_REG_EN_MASK) >> UART_LINE_CTRL_DIV_REG_EN_SHIFT;
}


uint16_t rom_hw_uart_get_clock_div(uint32_t u32Baud)
{
    //Div = (Apb0) / (16 * Baud)  -- temp use sysclock
    uint16_t u16Div = (SystemCoreClock >> 4) / u32Baud;

    return u16Div;
}

//Must config fraction first.
uint8_t rom_hw_uart_get_fraction_clock_div(uint16_t u16Div, uint32_t u32Baud)
{
    float fDivAll = ((float)(SystemCoreClock >> 4) / u32Baud ) - u16Div;
    fDivAll = fDivAll * 16;
    uint8_t u32DivFrac = (uint8_t)fDivAll;
    
    return (u32DivFrac & 0xF);
}

EN_ERR_STA_T rom_hw_uart_set_clk_div_reg_enable(stUartHandle_t *pstHandle, bool bEnable)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    //Wait busy flag
    uint16_t u8Flag = 0;

    while(u8Flag & EN_UART_STA_UART_BUSY)
    {
        rom_hw_uart_get_uart_status(pstHandle, &u8Flag);
    }

    if(bEnable)
    {
        pstHandle->LINE_CTRL |= UART_LINE_CTRL_DIV_REG_EN_MASK << UART_LINE_CTRL_DIV_REG_EN_SHIFT;
    }
    else
    {
        pstHandle->LINE_CTRL &= ~(UART_LINE_CTRL_DIV_REG_EN_MASK << UART_LINE_CTRL_DIV_REG_EN_SHIFT);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_baudrate(stUartHandle_t *pstHandle, uint32_t u32BaudRate)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint16_t u16ClkDiv = rom_hw_uart_get_clock_div(u32BaudRate);
    uint8_t u8ClkDivFraction = rom_hw_uart_get_fraction_clock_div(u16ClkDiv, u32BaudRate);

    //[20241227]-1 Hw Design - Need consider uart busy or not.
    rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);

    //Must config fraction first.
    pstHandle->CLK_DIV_FRACTION = u8ClkDivFraction & UART_CLK_DIV_FRACTION_REG_MASK;
    pstHandle->INT_EN_AND_CLK_DIV_HI = (u16ClkDiv >> 8) & UART_CLK_DIV_HI_REG_MASK;
    pstHandle->DATA_FIFO_AND_CLK_DLV_LO = u16ClkDiv & UART_CLK_DIV_LO_REG_MASK;

    rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_fifo_mode(stUartHandle_t *pstHandle, stUartFifoCfg_t *pstFifo)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) | (NULL == pstFifo))
    {
        return EN_ERROR_STA_INVALID;
    }

    // Reuse reg, need make sure config correct value.
    pstHandle->FIFO_CTRL_AND_INT_FLAG =( ((pstFifo->bFifoEn & UART_FIFO_CTRL_FIFO_EN_MASK) << UART_FIFO_CTRL_FIFO_EN_SHIFT) |\
                                        ((pstFifo->bRxFifoClean & UART_FIFO_CTRL_RX_FIFO_RST_MASK) << UART_FIFO_CTRL_RX_FIFO_RST_SHIFT)|\
                                        ((pstFifo->bTxFifoClean & UART_FIFO_CTRL_TX_FIFO_RST_MASK) << UART_FIFO_CTRL_TX_FIFO_RST_SHIFT)|\
                                        ((pstFifo->enDmaMode & UART_FIFO_CTRL_DMA_MODE_MASK) << UART_FIFO_CTRL_DMA_MODE_SHIFT)|\
                                        ((pstFifo->enTxFifoThld & UART_FIFO_CTRL_TX_FIFO_THLD_MASK) << UART_FIFO_CTRL_TX_FIFO_THLD_SHIFT)|\
                                        ((pstFifo->enRxFifoThld) & UART_FIFO_CTRL_RX_FIFO_THLD_MASK) << UART_FIFO_CTRL_RX_FIFO_THLD_SHIFT ) & UART_FIFO_CTRL_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_enable_interrupt(stUartHandle_t *pstHandle, uint8_t u8Mask)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    bool bDivRegSta = rom_hw_uart_get_clock_div_reg_status(pstHandle);

    //[20241227]-1 Hw Design - Need consider uart busy or not.
    if(bDivRegSta)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);
    }

    pstHandle->INT_EN_AND_CLK_DIV_HI = u8Mask & UART_INT_EN_REG_MASK;

    if(bDivRegSta)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_disable_interrupt(stUartHandle_t *pstHandle, uint8_t u8Mask)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    bool bDivRegSta = rom_hw_uart_get_clock_div_reg_status(pstHandle);

    //[20241227]-1 Hw Design - Need consider uart busy or not.
    if(bDivRegSta)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);
    }

    pstHandle->INT_EN_AND_CLK_DIV_HI &= (~u8Mask) & UART_INT_EN_REG_MASK;

    if(bDivRegSta)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_get_interrupt_flag(stUartHandle_t *pstHandle, uint8_t *pu8Flag)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) | (NULL == pu8Flag))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = pstHandle->LINE_STA & UART_LINE_STA_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_mode(stUartHandle_t *pstHandle, unUartMode_t unMode)
{
    if(!IS_VALID_UART_HADNLE(pstHandle))
    {
        return EN_ERROR_STA_INVALID;
    }

    //If config div, must wait busy sig clean.
    pstHandle->LINE_CTRL = unMode.u32Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_get_uart_status(stUartHandle_t *pstHandle, uint16_t *pu8Sta)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) | (NULL == pu8Sta))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Sta = (pstHandle->LINE_STA & UART_LINE_STA_REG_MASK) | ((pstHandle->UART_STA & UART_STA_REG_MASK) << UART_LINE_STA_REG_LEN);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_rs485_mode(stUartHandle_t *pstHandle, unRs485Mode_t unMode)
{
    if(NULL == pstHandle)
    {
        return EN_ERROR_STA_INVALID;
    }
    pstHandle->RSTX_CTRL = unMode.u32Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_rs485_software_data_enable(stUartHandle_t *pstHandle, bool bEnable)
{
    if(NULL == pstHandle)
    {
        return EN_ERROR_STA_INVALID;
    }

    pstHandle->SW_DE_ENABLE = bEnable;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_set_rs485_data_enable_timing(stUartHandle_t *pstHandle, uint8_t u8AssertionTime, uint8_t u8DeAssertionTime)
{
    if(NULL == pstHandle)
    {
        return EN_ERROR_STA_INVALID;
    }

    pstHandle->DE_TIMING_CFG = ((u8AssertionTime & UART_DE_TIMING_CFG_TX_DELAY_TIME_MASK) << UART_DE_TIMING_CFG_TX_DELAY_TIME_SHIFT) |\
                                ((u8DeAssertionTime & UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_MASK) << UART_DE_TIMING_CFG_TX_DONE_DELAY_TIME_SHIFT);
    return EN_ERROR_STA_OK;
}

void rom_hw_uart_flow_control_enable(stUartHandle_t *pstHandle, bool enable)
{
    bool bDivFlag = rom_hw_uart_get_clock_div_reg_status(pstHandle);
    if(bDivFlag)
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);

    if (enable) {
        pstHandle->MODEM_CTRL |= ((UART_MODEM_CTRL_RTS_EN_MASK << UART_MODEM_CTRL_RTS_EN_SHIFT) |
                                  (UART_MODEM_CTRL_AUTO_RTS_CTS_EN_MASK << UART_MODEM_CTRL_AUTO_RTS_CTS_EN_SHIFT));
    } else {
        pstHandle->MODEM_CTRL &= ~((UART_MODEM_CTRL_RTS_EN_MASK << UART_MODEM_CTRL_RTS_EN_SHIFT) |
                                    (UART_MODEM_CTRL_AUTO_RTS_CTS_EN_MASK << UART_MODEM_CTRL_AUTO_RTS_CTS_EN_SHIFT));
    }

    if(bDivFlag)
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);
}

uint32_t rom_hw_uart_get_cts_sta(stUartHandle_t *pstHandle)
{
    uint32_t cts_sta = 0;
    cts_sta = (pstHandle->MODEM_STA >> UART_MODEM_STA_CST_LEVEL_SHIFT) & UART_MODEM_STA_CST_LEVEL_MASK;
    return cts_sta;
}

void rom_hw_uart_rs485_enable(stUartHandle_t *pstHandle, bool enable)
{
    if (enable) {
        pstHandle->RSTX_CTRL |= (UART_RSTX_CTRL_RS485_EN_MASK << UART_RSTX_CTRL_RS485_EN_SHIFT);
    } else {
        pstHandle->RSTX_CTRL &= ~(UART_RSTX_CTRL_RS485_EN_MASK << UART_RSTX_CTRL_RS485_EN_SHIFT);
    }
}

void rom_hw_uart_rs485_re_enable(stUartHandle_t *pstHandle, bool enable)
{
    if (enable) {
        pstHandle->RE_EN |= (UART_RE_EN_MASK << UART_RE_EN_SHIFT);
    } else {
        pstHandle->RE_EN &= ~(UART_RE_EN_MASK << UART_RE_EN_SHIFT);
    }
}

EN_ERR_STA_T rom_hw_uart_init(stUartHandle_t *pstHandle, stUartInit_t *pstInit)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) | (NULL == pstInit))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint8_t u8Ret = EN_ERROR_STA_OK;
    u8Ret |= rom_hw_uart_set_fifo_mode(pstHandle, &pstInit->stFifo);
    u8Ret |= rom_hw_uart_set_baudrate(pstHandle, pstInit->u32BaudRate);
    u8Ret |= rom_hw_uart_set_mode(pstHandle, pstInit->unMode);

    return u8Ret;
}

EN_ERR_STA_T rom_hw_uart_transmit_bytes(stUartHandle_t *pstHandle, uint8_t *pu8Data, uint16_t u16Len, uint32_t TimeOutUs)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) | (NULL == pu8Data))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint16_t u16Sta = 0;
    uint16_t i = 0;
    uint32_t u32TimeOutUs = 0;

    //[20241227]-1 Hw Design - Need consider uart busy or not.
    bool bDivFlag = rom_hw_uart_get_clock_div_reg_status(pstHandle);
    if(bDivFlag)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);
    }

    while(u16Len)
    {
        rom_hw_uart_get_uart_status(pstHandle, &u16Sta);
        while(EN_UART_STA_TX_FIFO_UNDER_THLD != (u16Sta & EN_UART_STA_TX_FIFO_UNDER_THLD))
        {
            rom_hw_uart_get_uart_status(pstHandle, &u16Sta);
            rom_utility_delay_us(5);
            u32TimeOutUs += 5;
            if(u32TimeOutUs >= TimeOutUs)
            {
                return EN_ERROR_STA_TIMEOUT;
            }
        }
        pstHandle->DATA_FIFO_AND_CLK_DLV_LO = pu8Data[i];
        i++;
        u16Len--;
        u32TimeOutUs = 0;
    }

    if(bDivFlag)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_uart_recieve_bytes(stUartHandle_t *pstHandle, uint8_t *pu8Buffer, uint16_t *pu16RxLen, uint32_t u32TimeOutUs)
{
    if((!IS_VALID_UART_HADNLE(pstHandle)) || (NULL == pu8Buffer) || (NULL == pu16RxLen) || (0 == *pu16RxLen))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32TimeCounter = 0;
    uint16_t u16Sta = 0;
    uint16_t max_len = *pu16RxLen;

    *pu16RxLen = 0;

    //[20241227]-1 Hw Design - Need consider uart busy or not.
    bool bDivFlag = rom_hw_uart_get_clock_div_reg_status(pstHandle);
    // TODO: 是否冗余
    if(bDivFlag)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, false);
    }

    while(max_len)
    {
        rom_hw_uart_get_uart_status(pstHandle, &u16Sta);
        while(EN_UART_STA_DATA_READY != (u16Sta & EN_UART_STA_DATA_READY))
        {
            rom_utility_delay_us(5);
            u32TimeCounter += 5;
            rom_hw_uart_get_uart_status(pstHandle, &u16Sta);
            if(u32TimeCounter >= u32TimeOutUs)
            {
                return EN_ERROR_STA_TIMEOUT;
            }
        }

        pu8Buffer[*pu16RxLen] = pstHandle->DATA_FIFO_AND_CLK_DLV_LO & UART_DATA_FIFO_REG_MASK;
        *pu16RxLen += 1;
        u32TimeCounter = 0;
        max_len--;
    }

    if(bDivFlag)
    {
        rom_hw_uart_set_clk_div_reg_enable(pstHandle, true);
    }

    return EN_ERROR_STA_OK;
}
