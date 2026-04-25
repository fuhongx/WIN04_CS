/**
 * ****************************************************************************
 * @file qmx_hal_lpuart.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "qmx_hal_sysctrl.h"
#include "qmx_hal_lpuart.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_lpuart.h"
#include "compiler.h"

#if HAL_LPUART_DMA_SUPPORT
static uint8_t g_lpuart_dma_txch[HAL_LPUART_MAX] __RETENTION_DATA = {0};
static uint8_t g_lpuart_dma_rxch[HAL_LPUART_MAX] __RETENTION_DATA = {0};
#endif  // #if HAL_LPUART_DMA_SUPPORT

int qmx_hal_lpuart_init(hal_lpuart_id_e lpuart, hal_lpuart_init_t *config)
{
    if (lpuart >= HAL_LPUART_MAX || config == NULL) {
        return -1;
    }

    stLpUartInit_t hw_config = {0};

    hw_config.bTxEn = true;
    hw_config.bRxEn = true;
    hw_config.enDataLen = (EN_LPUART_DATA_LEN_T)config->datalen;
    hw_config.enStopLen = (EN_LPUART_STOP_LEN_T)config->stopbit;
    hw_config.bParityEn = (config->parity == HAL_LPUART_PARITY_NONE) ? false : true;
    hw_config.enParity = (config->parity == HAL_LPUART_PARITY_EVEN) ? EN_LPUART_PARITY_EVEN : EN_LPUART_PARITY_ODD;
    hw_config.u8RxThld = config->rx_fifo_thld;
    hw_config.bDmaRxEn = false;
    hw_config.bDmaTxEn = false;

    return rom_hw_lpuart_init(&hw_config, config->baudrate);
}

void qmx_hal_lpuart_deinit(hal_lpuart_id_e lpuart)
{
    if (lpuart >= HAL_LPUART_MAX) {
        return;
    }

    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_LPUART);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_LPUART, false);
}

int qmx_hal_lpuart_send_data(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len)
{
    if (lpuart >= HAL_LPUART_MAX || data == NULL) {
        return -1;
    }

    return rom_hw_lpuart_send_data(data, len);
}

int qmx_hal_lpuart_receive_data(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t *len)
{
    if (lpuart >= HAL_LPUART_MAX || data == NULL) {
        return -1;
    }

    return rom_hw_lpuart_recv_data(data, (uint16_t *)len);
}

void qmx_hal_lpuart_enable_irq(hal_lpuart_id_e lpuart, uint32_t irq_mask)
{
    if (lpuart >= HAL_LPUART_MAX) {
        return;
    }

    rom_hw_lpuart_enable_interrupt((uint8_t)irq_mask);
}

void qmx_hal_lpuart_disable_irq(hal_lpuart_id_e lpuart, uint32_t irq_mask)
{
    if (lpuart >= HAL_LPUART_MAX) {
        return;
    }

    rom_hw_lpuart_disable_irq((uint8_t)irq_mask);
}

uint32_t qmx_hal_lpuart_get_irq_status(hal_lpuart_id_e lpuart)
{
    if (lpuart >= HAL_LPUART_MAX) {
        return 0;
    }

    uint32_t status = 0;

    rom_hw_lpuart_get_interrupt_flag((uint8_t *)&status);

    return status;
}

void qmx_hal_lpuart_clear_irq_status(hal_lpuart_id_e lpuart, uint32_t irq_sta)
{
    if (lpuart >= HAL_LPUART_MAX) {
        return;
    }

    rom_hw_lpuart_clear_interrup((uint8_t)irq_sta);
}

#if HAL_LPUART_DMA_SUPPORT
void qmx_hal_lpuart_dma_config(hal_lpuart_id_e lpuart, hal_lpuart_dma_cfg_t *config)
{
    if (lpuart >= HAL_LPUART_MAX || config == NULL) {
        return;
    }

    hal_dma_init_t dma_config;

    rom_hw_lpuart_enable_dma(config->trx_dir);

    if (config->trx_dir == 0) {
        g_lpuart_dma_txch[lpuart] = config->dma_ch;
        dma_config.req = HAL_DMA_REQ_LPUART_TX;
        dma_config.src_addr_rise = true;
        dma_config.dst_addr_rise = false;
    } else {
        g_lpuart_dma_rxch[lpuart] = config->dma_ch;
        dma_config.req = HAL_DMA_REQ_LPUART_RX;
        dma_config.src_addr_rise = false;
        dma_config.dst_addr_rise = true;
    }

    dma_config.src_width = HAL_DMA_WIDTH_BYTE;
    dma_config.dst_width = HAL_DMA_WIDTH_BYTE;
    dma_config.src_addr = 0;
    dma_config.dst_addr = 0;
    dma_config.single_pkg_size = 1;
    dma_config.muli_trigger_num = 2;
    dma_config.irq_enable = config->dma_irq_enable;
    dma_config.timeout = (uint16_t)config->timeout;
    dma_config.soft_req = false;
    dma_config.high_priority = config->hign_pri;

    qmx_hal_dma_init(config->dma_ch, &dma_config);
}

int qmx_hal_lpuart_dma_send(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len)
{
    if (lpuart >= HAL_LPUART_MAX || data == NULL) {
        return -1;
    }

    hal_dma_cfg_t dma_cfg;

    dma_cfg.src_addr = (uint32_t)data;
    dma_cfg.dst_addr = (uint32_t)&(LPUART->LPUART_TBR);
    dma_cfg.single_pkg_size = 1;    // 需要和FIFO阈值一致
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = true;
    dma_cfg.dst_addr_rise = false;

    qmx_hal_dma_transfer_cfg((hal_dma_ch_e)g_lpuart_dma_txch[lpuart], &dma_cfg);
    qmx_hal_dma_start((hal_dma_ch_e)g_lpuart_dma_txch[lpuart]);

    return 0;
}

int qmx_hal_lpuart_dma_receive(hal_lpuart_id_e lpuart, uint8_t *data, uint32_t len)
{
    if (lpuart >= HAL_LPUART_MAX || data == NULL) {
        return -1;
    }

    hal_dma_cfg_t dma_cfg;

    dma_cfg.src_addr = (uint32_t)&(LPUART->LPUART_RBR);
    dma_cfg.dst_addr = (uint32_t)data;
    dma_cfg.single_pkg_size = 1;    // 需要和FIFO阈值一致
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = false;
    dma_cfg.dst_addr_rise = true;

    qmx_hal_dma_transfer_cfg((hal_dma_ch_e)g_lpuart_dma_rxch[lpuart], &dma_cfg);
    qmx_hal_dma_start((hal_dma_ch_e)g_lpuart_dma_rxch[lpuart]);

    return 0;
}

#endif  // #if HAL_LPUART_DMA_SUPPORT
