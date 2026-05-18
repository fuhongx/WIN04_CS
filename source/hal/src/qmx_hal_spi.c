/**
 * ****************************************************************************
 * @file qmx_hal_rtc.c
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
#include "qmx_hal_spi.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_spi.h"

stSpiHandle_t *qmx_hal_spi_get_handle(hal_spi_id_e spi_id)
{
    switch (spi_id) {
        case HAL_SPI0:
            return SPI0;
        case HAL_SPI1:
            return SPI1;
        default:
            return NULL;
    }
}

static void qmx_hal_spi_set_clock_polarity_phase(stSpiHandle_t *spi, hal_spi_polarity_phase_e polarity_phase)
{
    switch (polarity_phase) {
        case HAL_SPI_CPOL0_CPHA0:
            rom_hw_spi_set_clock_polarity_phase(spi, false, false);
            break;
        case HAL_SPI_CPOL0_CPHA1:
            rom_hw_spi_set_clock_polarity_phase(spi, false, true);
            break;
        case HAL_SPI_CPOL1_CPHA0:
            rom_hw_spi_set_clock_polarity_phase(spi, true, false);
            break;
        case HAL_SPI_CPOL1_CPHA1:
            rom_hw_spi_set_clock_polarity_phase(spi, true, true);
            break;
        default:
            rom_hw_spi_set_clock_polarity_phase(spi, false, false);
            break;
    }
}

void qmx_hal_spi_init(hal_spi_id_e id, hal_spi_cfg_t *spi_cfg)
{
    unSpiInit_t spi_init = {0};
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);

    if ((spi == NULL) || (spi_cfg == NULL) || (spi_cfg->clk_div >= HAL_SPI_DIV_MAX) || (spi_cfg->mode > HAL_SPI_MASTER) ||
        (spi_cfg->data_mode > HAL_SPI_DATA_LSB) || (spi_cfg->data_len > HAL_SPI_DATA_LEN_MAX) ||
        (spi_cfg->anti_noise_level > 1))
    {
        return;
    }
    if ((spi_cfg->tx_fifo_pfull_th >= TX_FIFO_DEPTH) || (spi_cfg->tx_fifo_pempty_th >= TX_FIFO_DEPTH) ||
        (spi_cfg->rx_fifo_pfull_th >= RX_FIFO_DEPTH) || (spi_cfg->rx_fifo_pempty_th >= RX_FIFO_DEPTH) ||
        (spi_cfg->tx_fifo_pempty_th == 0) || (spi_cfg->rx_fifo_pempty_th == 0) ||
        (spi_cfg->tx_fifo_pfull_th == 0) || (spi_cfg->rx_fifo_pfull_th == 0)) {
        return;
    }
    if ((spi_cfg->cs_holding_time < 0x4) || (spi_cfg->cs_gap_time < 0x4)) {
        return;
    }

    spi_init.stSpiInit.ClkDiv = spi_cfg->clk_div;
    spi_init.stSpiInit.MasterMode = spi_cfg->mode;
    spi_init.stSpiInit.ClkAdjustEn = spi_cfg->clk_adjust_en;
    spi_init.stSpiInit.DataMode = spi_cfg->data_mode;
    spi_init.stSpiInit.TxDmaEn = true;
    spi_init.stSpiInit.RxDmaEn = true;
    spi_init.stSpiInit.SwCsEn = spi_cfg->sw_cs_en;
    spi_init.stSpiInit.snoise_mode = spi_cfg->anti_noise_level;
    spi_init.stSpiInit.snoise_en = true;
    spi_init.stSpiInit.cpol = false;
    spi_init.stSpiInit.cpha = false;

    rom_hw_spi_init(spi, spi_init);
    qmx_hal_spi_set_clock_polarity_phase(spi, spi_cfg->polarity_phase);
    rom_hw_spi_set_hold_gap_time(spi, spi_cfg->cs_holding_time, spi_cfg->cs_gap_time);
    rom_hw_spi_set_fifo_threshold(spi, spi_cfg->tx_fifo_pempty_th, spi_cfg->rx_fifo_pempty_th,
        spi_cfg->tx_fifo_pfull_th, spi_cfg->rx_fifo_pfull_th);
}

void qmx_hal_spi_deinit(hal_spi_id_e id)
{
    hal_peripheral_clk_e spi_clk = HAL_CLK_SPI0;
    if (id == HAL_SPI0) {
        spi_clk = HAL_CLK_SPI0;
    } else if (id == HAL_SPI1) {
        spi_clk = HAL_CLK_SPI1;
    } else {
        return;
    }

    qmx_hal_sysctrl_peripheral_mod_reset(spi_clk);
    qmx_hal_sysctrl_peripheral_clk_enable(spi_clk, false);
}

void qmx_hal_spi_set_sw_cs(hal_spi_id_e id, hal_spi_cs_level_e cs_level)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    rom_hw_spi_set_cs_pin_level(spi, (EN_SPI_CS_LEVEL_T)cs_level);
}

int qmx_hal_spi_master_trx(hal_spi_id_e id, void *tx_buf, void *rx_buf, uint32_t len, uint32_t timeout)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return -1;
    }

    return rom_hw_spi_master_transmit_receive(spi, tx_buf, rx_buf, len, timeout);
}

int qmx_hal_spi_master_transmit(hal_spi_id_e id, void *tx_buf, uint32_t len, uint32_t timeout)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if ((spi == NULL) || (tx_buf == NULL) || (len == 0)) {
        return -1;
    }

    return rom_hw_spi_master_transmit_receive(spi, tx_buf, NULL, len, timeout);
}

int qmx_hal_spi_master_receive(hal_spi_id_e id, void *rx_buf, uint32_t len, uint32_t timeout)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if ((spi == NULL) || (rx_buf == NULL) || (len == 0)) {
        return -1;
    }

    return rom_hw_spi_master_transmit_receive(spi, NULL, rx_buf, len, timeout);
}

int qmx_hal_spi_slave_transmit(hal_spi_id_e id, void *tx_buf, uint32_t len, uint32_t timeout)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if ((spi == NULL) || (tx_buf == NULL) || (len == 0)) {
        return -1;
    }

    return rom_hw_spi_slave_transmit(spi, tx_buf, len, timeout);
}

int qmx_hal_spi_slave_receive(hal_spi_id_e id, void *rx_buf, uint32_t *len, uint32_t timeout)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if ((spi == NULL) || (rx_buf == NULL) || (*len == 0)) {
        return -1;
    }

    return rom_hw_spi_slave_receive(spi, rx_buf, len, timeout);
}

uint32_t qmx_hal_spi_get_cur_sta(hal_spi_id_e id)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    uint32_t status = 0;
    if (spi == NULL) {
        return 0;
    }

    rom_hw_spi_get_status(spi, &status);
    return status;
}

void qmx_hal_spi_set_irq_mask(hal_spi_id_e id, uint32_t irq_mask)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    rom_hw_spi_set_interrupt_mask(spi, irq_mask);
}

void qmx_hal_spi_irq_enable(hal_spi_id_e id, uint32_t irq_mask)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    rom_hw_spi_enable_interrupt(spi, irq_mask, true);
}

void qmx_hal_spi_irq_disable(hal_spi_id_e id, uint32_t irq_mask)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    rom_hw_spi_enable_interrupt(spi, irq_mask, false);
    rom_hw_spi_set_interrupt_mask(spi, irq_mask);
}

uint32_t qmx_hal_spi_get_irq_sta(hal_spi_id_e id)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    uint32_t status = 0;
    if (spi == NULL) {
        return 0;
    }

    rom_hw_spi_get_interrupt_flag(spi, &status);
    return status;
}

void qmx_hal_spi_clear_irq_sta(hal_spi_id_e id, uint32_t irq_sta)
{
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    rom_hw_spi_clear_interrupt_flag(spi, irq_sta);
}

#if HAL_SPI_DMA_SUPPORT

static uint8_t g_spi_dma_txch[HAL_SPI_MAX] __RETENTION_DATA = {0};
static uint8_t g_spi_dma_rxch[HAL_SPI_MAX] __RETENTION_DATA = {0};

void qmx_hal_spi_dma_config(hal_spi_id_e id, hal_spi_dma_cfg_t *spi_dma_init)
{
    hal_dma_init_t dma_config;
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return;
    }

    if (spi_dma_init->trx_dir == 0) {
        g_spi_dma_txch[id] = spi_dma_init->dma_ch;
        dma_config.req = (id == HAL_SPI0) ? HAL_DMA_REQ_SPI0_TX : HAL_DMA_REQ_SPI1_TX;
        dma_config.src_addr_rise = true;
        dma_config.dst_addr_rise = false;
    } else {
        g_spi_dma_rxch[id] = spi_dma_init->dma_ch;
        dma_config.req = (id == HAL_SPI0) ? HAL_DMA_REQ_SPI0_RX : HAL_DMA_REQ_SPI1_RX;
        dma_config.src_addr_rise = false;
        dma_config.dst_addr_rise = true;
    }

    dma_config.src_width = HAL_DMA_WIDTH_BYTE;
    dma_config.dst_width = HAL_DMA_WIDTH_BYTE;

    dma_config.src_addr = 0;
    dma_config.dst_addr = 0;
    dma_config.single_pkg_size = 1;
    dma_config.muli_trigger_num = 2;
    dma_config.irq_enable = spi_dma_init->dma_irq_enable;
    dma_config.timeout = (uint16_t)spi_dma_init->dma_timeout;
    dma_config.soft_req = false;
    dma_config.high_priority = spi_dma_init->hign_pri;

    qmx_hal_dma_init(spi_dma_init->dma_ch, &dma_config);
}

int qmx_hal_spi_dma_send(hal_spi_id_e id, void *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return -1;
    }

    dma_cfg.src_addr = (uint32_t)data;
    dma_cfg.dst_addr = (uint32_t)&(spi->SPI_THR);
    dma_cfg.single_pkg_size = 1;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = true;
    dma_cfg.dst_addr_rise = false;

    qmx_hal_dma_transfer_cfg((hal_dma_ch_e)g_spi_dma_txch[id], &dma_cfg);
    qmx_hal_dma_start((hal_dma_ch_e)g_spi_dma_txch[id]);

    return 0;
}

int qmx_hal_spi_dma_recv(hal_spi_id_e id, void *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stSpiHandle_t *spi = qmx_hal_spi_get_handle(id);
    if (spi == NULL) {
        return -1;
    }

    dma_cfg.src_addr = (uint32_t)&(spi->SPI_RBR);
    dma_cfg.dst_addr = (uint32_t)data;
    dma_cfg.single_pkg_size = 1;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = false;
    dma_cfg.dst_addr_rise = true;

    qmx_hal_dma_transfer_cfg((hal_dma_ch_e)g_spi_dma_rxch[id], &dma_cfg);
    qmx_hal_dma_start((hal_dma_ch_e)g_spi_dma_rxch[id]);

    return 0;
}

#endif /* HAL_SPI_DMA_SUPPORT */
