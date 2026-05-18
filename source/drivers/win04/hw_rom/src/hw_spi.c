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

#define SPI_HANDLE_INVALID_RETURN(pstHandle)    \
    if(NULL == pstHandle)   \
    {                       \
        return EN_ERROR_STA_INVALID;    \
    }                       \

#define SPI_GET_TX_FIFO_CNT(status)     (((status) >> SPI_TSR_TX_FIFO_CNT_SHIFT) & SPI_TSR_TX_FIFO_CNT_MASK)
#define SPI_GET_RX_FIFO_CNT(status)     (((status) >> SPI_TSR_RX_FIFO_CNT_SHIFT) & SPI_TSR_RX_FIFO_CNT_MASK)

static void rom_hw_spi_flush_rx_fifo(stSpiHandle_t *pstHandle)
{
    uint32_t i = 0;
    uint32_t rx_cnt = SPI_GET_RX_FIFO_CNT(pstHandle->SPI_TSR);

    if(rx_cnt > RX_FIFO_DEPTH) {
        rx_cnt = RX_FIFO_DEPTH;
    }

    for(i = 0; i < rx_cnt; i++) {
        (void)pstHandle->SPI_RBR;
    }
}

EN_ERR_STA_T rom_hw_spi_init(stSpiHandle_t *pstHandle, unSpiInit_t unInit)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_TCR = unInit.u32Cfg & SPI_TCR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_get_status(stSpiHandle_t *pstHandle, uint32_t *pu32Flag)
{
    if((NULL == pstHandle) | (NULL == pu32Flag))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu32Flag = pstHandle->SPI_TSR;

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
    uint32_t tx_index = 0;
    uint32_t rx_index = 0;
    uint32_t req = len;
    uint32_t rx_pending = 0;
    uint32_t tx_once = 0;
    uint32_t rx_once = 0;
    uint32_t tx_free = 0;
    uint32_t rx_free = 0;
    uint32_t status = 0;
    bool progressed = false;

    rom_hw_spi_flush_rx_fifo(pstHandle);

    do {
        progressed = false;

        if (rx_pending > 0) {
            rx_once = SPI_GET_RX_FIFO_CNT(pstHandle->SPI_TSR);
            if(rx_once > RX_FIFO_DEPTH) {
                rx_once = RX_FIFO_DEPTH;
            }
            rx_once = (rx_once > rx_pending) ? rx_pending : rx_once;
            for (i = 0; i < rx_once; i++) {
                uint32_t rx_data = pstHandle->SPI_RBR & SPI_RBR_REG_MASK;
                if(rx_buf != NULL) {
                    ((uint8_t *)rx_buf)[rx_index++] = (uint8_t)rx_data;
                }
            }
            rx_pending -= rx_once;
            progressed = (rx_once > 0);
        }

        if (req > 0) {
            status = pstHandle->SPI_TSR;
            tx_free = SPI_GET_TX_FIFO_CNT(status);
            tx_free = (tx_free < TX_FIFO_DEPTH) ? (TX_FIFO_DEPTH - tx_free) : 0;
            rx_free = SPI_GET_RX_FIFO_CNT(status);
            rx_free = (rx_free < RX_FIFO_DEPTH) ? (RX_FIFO_DEPTH - rx_free) : 0;

            tx_once = (tx_free > rx_free) ? rx_free : tx_free;
            tx_once = (tx_once > req) ? req : tx_once;

            for (i = 0; i < tx_once; i++) {
                pstHandle->SPI_THR = (tx_buf != NULL) ? ((uint8_t *)tx_buf)[tx_index++] : 0;
            }

            req -= tx_once;
            rx_pending += tx_once;
            progressed = progressed || (tx_once > 0);
        }

        if (!progressed) {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        } else {
            cost_us = 0;
        }
    } while ((req > 0) || (rx_pending > 0));

    cost_us = 0;
    do {
        status = pstHandle->SPI_TSR;
        if((status & (SPI_TSR_TX_FIFO_EMPTY_MASK << SPI_TSR_TX_FIFO_EMPTY_SHIFT)) &&
           ((status & (SPI_TSR_SPI_BUSY_MASK << SPI_TSR_SPI_BUSY_SHIFT)) == 0)) {
            break;
        }
        rom_utility_delay_us(1);
        cost_us += 1;
        if(cost_us >= timeout_us)
            return EN_ERROR_STA_TIMEOUT;
    } while (1);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_slave_transmit(stSpiHandle_t *pstHandle, void *tx_buf, uint32_t len, uint32_t timeout_us)
{
    if((pstHandle == NULL) || (tx_buf == NULL) || (len == 0)) {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t cost_us = 0;
    uint32_t i = 0;
    uint32_t index = 0;
    uint32_t left = len;
    uint32_t req = len;
    uint32_t tx_once = 0;
    uint32_t rx_once = 0;
    uint32_t tx_free = 0;
    uint32_t rx_free = 0;
    uint32_t status = 0;
    bool progressed = false;

    rom_hw_spi_flush_rx_fifo(pstHandle);

    do {
        progressed = false;

        if (left > 0) {
            rx_once = SPI_GET_RX_FIFO_CNT(pstHandle->SPI_TSR);
            if(rx_once > RX_FIFO_DEPTH) {
                rx_once = RX_FIFO_DEPTH;
            }
            rx_once = (rx_once > left) ? left : rx_once;
            for (i = 0; i < rx_once; i++) {
                (void)pstHandle->SPI_RBR;
            }
            left -= rx_once;
            progressed = (rx_once > 0);
        }

        if (req > 0) {
            status = pstHandle->SPI_TSR;
            tx_free = SPI_GET_TX_FIFO_CNT(status);
            tx_free = (tx_free < TX_FIFO_DEPTH) ? (TX_FIFO_DEPTH - tx_free) : 0;
            rx_free = SPI_GET_RX_FIFO_CNT(status);
            rx_free = (rx_free < RX_FIFO_DEPTH) ? (RX_FIFO_DEPTH - rx_free) : 0;

            tx_once = (tx_free > rx_free) ? rx_free : tx_free;
            tx_once = (tx_once > req) ? req : tx_once;

            for (i = 0; i < tx_once; i++) {
                pstHandle->SPI_THR = ((uint8_t *)tx_buf)[index++];
            }

            req -= tx_once;
            progressed = progressed || (tx_once > 0);
        }

        if (!progressed) {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        } else {
            cost_us = 0;
        }
    } while ((req > 0) || (left > 0));

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_slave_receive(stSpiHandle_t *pstHandle, void *rx_buf, uint32_t *len, uint32_t timeout_us)
{
    if((pstHandle == NULL) || (rx_buf == NULL) || (len == NULL) || (*len == 0)) {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t max_len = *len;
    uint32_t cost_us = 0;
    uint32_t i = 0;
    uint32_t rx_index = 0;
    uint32_t rx_once = 0;
    uint32_t left = max_len;

    *len = 0;

    while (left > 0) {
        rx_once = SPI_GET_RX_FIFO_CNT(pstHandle->SPI_TSR);
        if(rx_once > RX_FIFO_DEPTH) {
            rx_once = RX_FIFO_DEPTH;
        }
        rx_once = (rx_once > left) ? left : rx_once;

        if (rx_once > 0) {
            for (i = 0; i < rx_once; i++) {
                ((uint8_t *)rx_buf)[rx_index++] = pstHandle->SPI_RBR & SPI_RBR_REG_MASK;
            }
            left -= rx_once;
            *len += rx_once;
            cost_us = 0;
        } else {
            rom_utility_delay_us(1);
            cost_us += 1;
            if(cost_us >= timeout_us)
                return EN_ERROR_STA_TIMEOUT;
        }
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_interrupt_mask(stSpiHandle_t *pstHandle, uint32_t u32Mask)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_IMR = u32Mask;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_enable_interrupt(stSpiHandle_t *pstHandle, uint32_t u32Mask, bool bEnable)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    if(bEnable)
    {
        pstHandle->SPI_IER |= u32Mask;
    }
    else
    {
        pstHandle->SPI_IER &= (~u32Mask);
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_get_interrupt_flag(stSpiHandle_t *pstHandle, uint32_t *u32Mask)
{
    if((NULL == pstHandle) | (NULL == u32Mask))
    {
        return EN_ERROR_STA_INVALID;
    }

    *u32Mask = pstHandle->SPI_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_clear_interrupt_flag(stSpiHandle_t *pstHandle, uint32_t u32Mask)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    uint32_t u32Cfg = pstHandle->SPI_ISR;
    pstHandle->SPI_ISR = u32Mask & u32Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_hold_gap_time(stSpiHandle_t *pstHandle, uint32_t hold_cycles, uint32_t gap_cycles)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_CTR = SPI_CTR_SETUP_HOLD_VAL(hold_cycles) | SPI_CTR_BYTE_GAP_VAL(gap_cycles);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_spi_set_clock_polarity_phase(stSpiHandle_t *pstHandle, bool bCPOL, bool bCPHA)
{
    SPI_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->SPI_TCR &= ~((SPI_CLKMOD_CPOL_REG_MASK << SPI_CLKMOD_CPOL_REG_SHIFT) |
                           (SPI_CLKMODE_CPHA_REG_MASK << SPI_CLKMODE_CPHA_REG_SHIFT));
    pstHandle->SPI_TCR |= (SPI_CLKMOD_CPOL_REG_VAL(bCPOL) | SPI_CLKMODE_CPHA_REG_VAL(bCPHA));

    return EN_ERROR_STA_OK;
}

void rom_hw_spi_set_fifo_threshold(stSpiHandle_t *pstHandle,
                                    uint32_t tx_empty_th, uint32_t rx_empty_th,
                                    uint32_t tx_th, uint32_t rx_th)
{
    pstHandle->SPI_FTR = (SPI_FTR_TX_FIFO_PFULL_TH_VAL(tx_th) | SPI_FTR_RX_FIFO_PFULL_TH_VAL(rx_th) |
                         SPI_FTR_TX_FIFO_PEMPTY_TH_VAL(tx_empty_th) | SPI_FTR_RX_FIFO_PEMPTY_TH_VAL(rx_empty_th));
}
