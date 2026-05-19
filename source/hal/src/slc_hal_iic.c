/**
 * ****************************************************************************
 * @file slc_hal_uart.c
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "slc_hal_sysctrl.h"
#include "slc_hal_iic.h"
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "hw_iic.h"
#include "compiler.h"

stIicHandle_t *slc_hal_iic_get_handle(hal_iic_id_e iic_id)
{
    switch (iic_id) {
        case HAL_IIC0:
            return IIC0;
        case HAL_IIC1:
            return IIC1;
        default:
            return NULL;
    }
}

void slc_hal_iic_init(hal_iic_id_e iic_id, hal_iic_init_t *iic_init)
{
    stIicInt_t iic_cfg;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);

    if (iic == NULL) {
        return;
    }

    iic_cfg.enMode = (EN_IIC_MODE_T)iic_init->mode;
    iic_cfg.enSpeed = (EN_IIC_SPEED_T)(iic_init->speed+1);
    if (iic_init->mode == HAL_IIC_MASTER) {
        iic_cfg.enTargetAddrLen = (EN_IIC_ADDR_LEN_T)iic_init->addr_bits;
        iic_cfg.u16TargetAddr = iic_init->addr;
        iic_cfg.enOwnSlaveAddrLen = (EN_IIC_ADDR_LEN_T)iic_init->addr_bits;
        iic_cfg.u16OwnSlaveAddr = 0;
    } else {
        iic_cfg.enOwnSlaveAddrLen = (EN_IIC_ADDR_LEN_T)iic_init->addr_bits;
        iic_cfg.u16OwnSlaveAddr = iic_init->addr;
        iic_cfg.enTargetAddrLen = (EN_IIC_ADDR_LEN_T)iic_init->addr_bits;
        iic_cfg.u16TargetAddr = 0;
    }

    iic_cfg.u8TxTL = iic_init->tx_thld;
    iic_cfg.u8RxTL = iic_init->rx_thld;

    rom_hw_iic_init(iic, &iic_cfg);

    rom_hw_iic_set_dma_trx_threshold(iic, 6, 0);
}

void slc_hal_iic_deinit(hal_iic_id_e iic_id)
{
    hal_peripheral_clk_e iic_clk = HAL_CLK_I2C0;
    if (iic_id == HAL_IIC0) {
        iic_clk = HAL_CLK_I2C0;
    } else if (iic_id == HAL_IIC1) {
        iic_clk = HAL_CLK_I2C1;
    } else {
        return;
    }

    slc_hal_sysctrl_peripheral_mod_reset(iic_clk);
    slc_hal_sysctrl_peripheral_clk_enable(iic_clk, false);
}

int slc_hal_iic_master_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len, uint32_t timeout)
{
    int ret = 0;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    ret = rom_hw_iic_master_tx(iic, data, len, timeout);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

int slc_hal_iic_master_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t *len, uint32_t timeout)
{
    int ret = 0;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    ret = rom_hw_iic_master_rx(iic, data, (uint16_t *)len, timeout);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

int slc_hal_iic_slave_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len, uint32_t timeout)
{
    int ret = 0;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    ret = rom_hw_iic_slave_tx(iic, data, len, timeout);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

int slc_hal_iic_slave_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t *len, uint32_t timeout)
{
    int ret = 0;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    ret = rom_hw_iic_slave_rx(iic, data, (uint16_t *)len, timeout);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

void slc_hal_iic_irq_enable(hal_iic_id_e iic_id, uint32_t irq_mask)
{
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return;
    }

    rom_hw_iic_enable_interrupt(iic, irq_mask);
}

void slc_hal_iic_irq_disable(hal_iic_id_e iic_id, uint32_t irq_mask)
{
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return;
    }

    rom_hw_iic_disable_interrupt(iic, irq_mask);
}

uint32_t slc_hal_iic_get_irq_status(hal_iic_id_e iic_id)
{
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return 0;
    }

    uint16_t sta = 0;

    rom_hw_iic_get_interrupt_flag(iic, &sta);
    return sta;
}

void slc_hal_iic_clear_irq_status(hal_iic_id_e iic_id, uint32_t irq_sta)
{
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return;
    }

    rom_hw_iic_clear_interrupt_flag(iic, irq_sta);
}

uint32_t slc_hal_iic_get_status(hal_iic_id_e iic_id)
{
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return 0;
    }

    uint8_t sta = 0;

    rom_hw_iic_get_status(iic, &sta);
    return sta;
}

#if HAL_IIC_DMA_SUPPORT

static uint8_t g_iic_dma_txch[HAL_IIC_MAX] __RETENTION_DATA = {0};
static uint8_t g_iic_dma_rxch[HAL_IIC_MAX] __RETENTION_DATA = {0};

void slc_hal_iic_dma_config(hal_iic_id_e iic_id, hal_iic_dma_cfg_t *iic_dma_init)
{
    hal_dma_init_t dma_config;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if ((iic == NULL) || (iic_dma_init->dma_ch >= HAL_DMA_CH_MAX)) {
        return;
    }

    if (iic_dma_init->trx_dir == 0) {
        g_iic_dma_txch[iic_id] = iic_dma_init->dma_ch;
        dma_config.req = (iic_id == HAL_IIC0) ? HAL_DMA_REQ_I2C0_TX : HAL_DMA_REQ_I2C1_TX;
        dma_config.src_addr_rise = true;
        dma_config.dst_addr_rise = false;
        if (iic_dma_init->mode == HAL_IIC_MASTER) {
            dma_config.src_width = HAL_DMA_WIDTH_HALFWORD;
            dma_config.dst_width = HAL_DMA_WIDTH_HALFWORD;
        } else {
            dma_config.src_width = HAL_DMA_WIDTH_BYTE;
            dma_config.dst_width = HAL_DMA_WIDTH_BYTE;
        }
    } else {
        g_iic_dma_rxch[iic_id] = iic_dma_init->dma_ch;
        dma_config.req = (iic_id == HAL_IIC0) ? HAL_DMA_REQ_I2C0_RX : HAL_DMA_REQ_I2C1_RX;
        dma_config.src_addr_rise = false;
        dma_config.dst_addr_rise = true;
        dma_config.src_width = HAL_DMA_WIDTH_BYTE;
        dma_config.dst_width = HAL_DMA_WIDTH_BYTE;
    }

    dma_config.src_addr = 0;
    dma_config.dst_addr = 0;
    dma_config.single_pkg_size = 1;
    dma_config.muli_trigger_num = 2;
    dma_config.irq_enable = iic_dma_init->dma_irq_enable;
    dma_config.timeout = (uint16_t)iic_dma_init->timeout;
    dma_config.soft_req = false;
    dma_config.high_priority = iic_dma_init->hign_pri;

    slc_hal_dma_init(iic_dma_init->dma_ch, &dma_config);
}

int slc_hal_iic_master_dma_send(hal_iic_id_e iic_id, uint16_t *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    dma_cfg.src_addr = (uint32_t)data;
    dma_cfg.dst_addr = (uint32_t)&(iic->IIC_DATA_CMD);
    dma_cfg.single_pkg_size = 2;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = true;
    dma_cfg.dst_addr_rise = false;
    data[len - 1] |= 0x200; // last byte set stop bit

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_iic_dma_txch[iic_id], &dma_cfg);

    slc_hal_dma_start((hal_dma_ch_e)g_iic_dma_txch[iic_id]);
    rom_hw_iic_enable_dma_transmit(iic, EN_IIC_DMA_TX_ENABLE);
    return 0;
}

int slc_hal_iic_master_dma_recv(hal_iic_id_e iic_id, uint16_t *cmd_buf, uint8_t *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    for (uint32_t i = 0; i < len; i++) {
        cmd_buf[i] = 0x100;
    }

    dma_cfg.src_addr = (uint32_t)&(iic->IIC_DATA_CMD);
    dma_cfg.dst_addr = (uint32_t)data;
    dma_cfg.single_pkg_size = 1;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = false;
    dma_cfg.dst_addr_rise = true;

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_iic_dma_rxch[iic_id], &dma_cfg);
    slc_hal_dma_start((hal_dma_ch_e)g_iic_dma_rxch[iic_id]);

    slc_hal_iic_master_dma_send(iic_id, cmd_buf, len);
    rom_hw_iic_enable_dma_transmit(iic, EN_IIC_DMA_TRX_ENABLE);
    return 0;
}

int slc_hal_iic_slave_dma_send(hal_iic_id_e iic_id, uint8_t *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    dma_cfg.src_addr = (uint32_t)data;
    dma_cfg.dst_addr = (uint32_t)&(iic->IIC_DATA_CMD);
    dma_cfg.single_pkg_size = 1;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = true;
    dma_cfg.dst_addr_rise = false;

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_iic_dma_txch[iic_id], &dma_cfg);
    slc_hal_dma_start((hal_dma_ch_e)g_iic_dma_txch[iic_id]);
    rom_hw_iic_enable_dma_transmit(iic, EN_IIC_DMA_TX_ENABLE);
    return 0;
}

int slc_hal_iic_slave_dma_recv(hal_iic_id_e iic_id, uint8_t *data, uint32_t len)
{
    hal_dma_cfg_t dma_cfg;
    stIicHandle_t *iic = slc_hal_iic_get_handle(iic_id);
    if (iic == NULL) {
        return -1;
    }

    dma_cfg.src_addr = (uint32_t)&(iic->IIC_DATA_CMD);
    dma_cfg.dst_addr = (uint32_t)data;
    dma_cfg.single_pkg_size = 1;
    dma_cfg.muli_trigger_num = len;
    dma_cfg.src_addr_rise = false;
    dma_cfg.dst_addr_rise = true;

    slc_hal_dma_transfer_cfg((hal_dma_ch_e)g_iic_dma_rxch[iic_id], &dma_cfg);
    slc_hal_dma_start((hal_dma_ch_e)g_iic_dma_rxch[iic_id]);
    rom_hw_iic_enable_dma_transmit(iic, EN_IIC_DMA_RX_ENABLE);
    return 0;
}

#endif  // #if HAL_IIC_DMA_SUPPORT
