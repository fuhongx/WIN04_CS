/**
 * ****************************************************************************
 * @file hw_spi.c
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
#include "hw_spi.h"

#define SPI_TRANSMIT_TIMEOUT_US         (3000)

#define SPI_HANDLE_INVALID_RETURN(pstHandle)    \
    if(NULL == pstHandle)   \
    {                       \
        return EN_ERROR_STA_INVALID;    \
    }                       \

EN_ERR_STA_T rom_hw_spi_init(stSpiHandle_t *pstHandle, unSpiInit_t unInit)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_TCR = unInit.u32Cfg & SPI_TCR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_get_status(stSpiHandle_t *pstHandle, uint8_t *pu8Flag)
{
    if((NULL == pstHandle) | (NULL == pu8Flag))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = pstHandle->SPI_TSR & SPI_TSR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_clear_status(stSpiHandle_t *pstHandle, uint8_t u8Flag)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    uint8_t u8Cfg = pstHandle->SPI_TSR & SPI_TSR_REG_MASK;
    pstHandle->SPI_TSR = u8Cfg & u8Flag;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_cs_pin_level(stSpiHandle_t *pstHandle, EN_SPI_CS_LEVEL_T enLevel)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->MCS_CTRL = enLevel & SPI_MCS_CTRL_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_master_transmit_receive(stSpiHandle_t *pstHandle, void *tx_buf, void *rx_buf, uint32_t len, uint32_t timeout_us)
{
    if((pstHandle == NULL) || (len == 0)) {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t cost_us = 0;
    uint32_t i = 0;
    uint32_t data_len = (pstHandle->SPI_TCR >> SPI_TCR_SPI_DATA_LEN_SHIFT) & SPI_TCR_SPI_DATA_LEN_MASK;
    uint32_t cnt = (len + (1 << data_len) - 1) / (1 << data_len);   // 不足位宽的向上取整

    cost_us = pstHandle->SPI_RBR;   // 清除初始的RX_NEW_DATA状态，避免第一次读空
    cost_us = 0;

    for (i = 0; i < cnt; i++) {
        while(!(pstHandle->SPI_TSR & EN_SPI_STA_TX_BUFFER_EMPTY)) {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        }

        switch (data_len) {
            case EN_SPI_DATA_LEN_8BIT:
                pstHandle->SPI_THR = (tx_buf != NULL) ? ((uint8_t *)tx_buf)[i] : 0;
                break;

            case EN_SPI_DATA_LEN_16BIT:
                pstHandle->SPI_THR = (tx_buf != NULL) ? ((uint16_t *)tx_buf)[i] : 0;
                break;

            case EN_SPI_DATA_LEN_32BIT:
                pstHandle->SPI_THR = (tx_buf != NULL) ? ((uint32_t *)tx_buf)[i] : 0;
                break;

            default:
                break;
        }

        cost_us = 0;

        if (rx_buf != NULL) {
            while(!(pstHandle->SPI_TSR & EN_SPI_STA_RX_NEW_DATA)) {
                rom_utility_delay_us(1);
                cost_us += 1;
                if(cost_us >= timeout_us)
                    return EN_ERROR_STA_TIMEOUT;
            }

            switch (data_len) {
                case EN_SPI_DATA_LEN_8BIT:
                    ((uint8_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                    break;

                case EN_SPI_DATA_LEN_16BIT:
                    ((uint16_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                    break;

                case EN_SPI_DATA_LEN_32BIT:
                    ((uint32_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                    break;

                default:
                    break;
            }
            cost_us = 0;
        }
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_slave_transmit(stSpiHandle_t *pstHandle, void *tx_buf, uint32_t len, uint32_t timeout_us)
{
    if((pstHandle == NULL) || (tx_buf == NULL) || (len == 0)) {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t cost_us = 0;
    uint32_t temp = 0;
    uint32_t i = 0;
    uint32_t data_len = (pstHandle->SPI_TCR >> SPI_TCR_SPI_DATA_LEN_SHIFT) & SPI_TCR_SPI_DATA_LEN_MASK;
    uint32_t cnt = (len + (1 << data_len) - 1) / (1 << data_len);   // 不足位宽的向上取整

    for (i = 0; i < cnt; i++) {
        while(!(pstHandle->SPI_TSR & EN_SPI_STA_TX_BUFFER_EMPTY)) {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        }

        switch (data_len) {
            case EN_SPI_DATA_LEN_8BIT:
                pstHandle->SPI_THR = ((uint8_t *)tx_buf)[i];
                break;

            case EN_SPI_DATA_LEN_16BIT:
                pstHandle->SPI_THR = ((uint16_t *)tx_buf)[i];
                break;

            case EN_SPI_DATA_LEN_32BIT:
                pstHandle->SPI_THR = ((uint32_t *)tx_buf)[i];
                break;

            default:
                break;
        }

        cost_us = 0;

        while(!(pstHandle->SPI_TSR & EN_SPI_STA_RX_NEW_DATA)) {
            // 此处不用us接口，us太久，可能导致时序错乱，误清状态
            rom_utility_delay_cycles(10);
            cost_us += 1;
            if(cost_us >= timeout_us)
                break;
        }

        temp = pstHandle->SPI_RBR;   // 读取RBR寄存器以清除RX_NEW_DATA状态，避免下一轮循环读空

        cost_us = 0;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_slave_receive(stSpiHandle_t *pstHandle, void *rx_buf, uint32_t *len, uint32_t timeout_us)
{
    if((pstHandle == NULL) || (rx_buf == NULL) || (*len == 0)) {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t max_len = *len;
    uint32_t cost_us = 0;
    uint32_t i = 0;
    uint32_t data_len = (pstHandle->SPI_TCR >> SPI_TCR_SPI_DATA_LEN_SHIFT) & SPI_TCR_SPI_DATA_LEN_MASK;
    uint32_t cnt = (max_len + (1 << data_len) - 1) / (1 << data_len);   // 不足位宽的向上取整

    *len = 0;

    for (i = 0; i < cnt; i++) {
        while(!(pstHandle->SPI_TSR & EN_SPI_STA_RX_NEW_DATA)) {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        }

        switch (data_len) {
            case EN_SPI_DATA_LEN_8BIT:
                ((uint8_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                *len += 1;
                break;

            case EN_SPI_DATA_LEN_16BIT:
                ((uint16_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                *len += 2;
                break;

            case EN_SPI_DATA_LEN_32BIT:
                ((uint32_t *)rx_buf)[i] = pstHandle->SPI_RBR;
                *len += 4;
                break;

            default:
                break;
        }

        cost_us = 0;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_interrupt_mask(stSpiHandle_t *pstHandle, uint8_t u8Mask)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_IMR = u8Mask & SPI_IMR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_enable_interrupt(stSpiHandle_t *pstHandle, uint8_t u8Mask, bool bEnable)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    if(bEnable)
    {
        pstHandle->SPI_IER = u8Mask & SPI_IER_REG_MASK;
    }
    else
    {
        pstHandle->SPI_IER = (~u8Mask) & SPI_IER_REG_MASK;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_get_interrupt_flag(stSpiHandle_t *pstHandle, uint8_t *pu8Mask)
{
    if((NULL == pstHandle) | (NULL == pu8Mask))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Mask = pstHandle->SPI_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_clear_interrupt_flag(stSpiHandle_t *pstHandle, uint8_t u8Mask)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    uint8_t u8Cfg = pstHandle->SPI_ISR;
    pstHandle->SPI_ISR = u8Mask & u8Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_enable_anti_noise(stSpiHandle_t *pstHandle, bool bEnable)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    if(bEnable)
    {
        pstHandle->SPI_NSEN |= SPI_NESN_REG_MASK;
    }
    else
    {
        pstHandle->SPI_NSEN &= (~SPI_NESN_REG_MASK) & SPI_NESN_REG_MASK;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_dma_timeout(stSpiHandle_t *pstHandle, uint32_t u32Cnt)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->DMA_TIMEOUT = u32Cnt;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_cs_min_holding_time(stSpiHandle_t *pstHandle, uint32_t u32Time)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_CSHT = u32Time;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_clock_polarity_phase(stSpiHandle_t *pstHandle, EN_SPI_CLK_POLARITY_PHASE_T enMode)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);
    if(enMode > EN_SPI_CLK_CPOL1_CPHA1)
    {
        return EN_ERROR_STA_INVALID;
    }

    pstHandle->SPI_CLKMOD = enMode & SPI_CLKMOD_REG_MASK;

    return EN_ERROR_STA_OK;
}

void rom_hw_spi_dma_mode(stSpiHandle_t *pstHandle, EN_SPI_DMA_TX_TIMING_T mode)
{
    pstHandle->SPI_TCR &= ~(SPI_TCR_DMA_TX_TIMING_MASK << SPI_TCR_DMA_TX_TIMING_SHIFT);
    pstHandle->SPI_TCR |= (mode << SPI_TCR_DMA_TX_TIMING_SHIFT);
}
