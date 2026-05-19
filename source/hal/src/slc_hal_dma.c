/**
 * ****************************************************************************
 * @file slc_hal_dma.c
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
#include "reg_Dev.h"
#include "error_def.h"
#include "slc_hal_dma.h"
#include "utility.h"
#include "hw_dma.h"

void slc_hal_dma_init(hal_dma_ch_e dma_ch, hal_dma_init_t *config)
{
    stDMAInit_t init;

    if (dma_ch >= HAL_DMA_CH_MAX)
        return;

    init.enDmaCh = (EN_DMA_CHANNEL_T)dma_ch;

    init.unChCfg.stDMAChCfg.SrcDataType = config->src_width;
    init.unChCfg.stDMAChCfg.DstDataType = config->dst_width;
    init.unChCfg.stDMAChCfg.SrcAddrRiseEn = config->src_addr_rise;
    init.unChCfg.stDMAChCfg.DstAddrRiseEn = config->dst_addr_rise;
    init.unChCfg.stDMAChCfg.FinishIntEn = (config->irq_enable & HAL_DMA_IRQ_ENABLE_TRANSFER_DONE) ? true : false;
    init.unChCfg.stDMAChCfg.ErrorIntEn = (config->irq_enable & HAL_DMA_IRQ_ENABLE_CFG_ERROR) ? true : false;
    init.unChCfg.stDMAChCfg.MutiTrigEn = (config->muli_trigger_num > 1) ? true : false;
    init.unChCfg.stDMAChCfg.TimeoutEn = (config->timeout == 0) ? false : true;
    init.unChCfg.stDMAChCfg.TimeoutVal = config->timeout;

    init.enReqType = (EN_DMA_REQ_T)config->req;
    init.u32SrcAddr = config->src_addr;
    init.u32DstAddr = config->dst_addr;
    init.u16PackByte  = config->single_pkg_size;
    init.u16TrigNum   = config->muli_trigger_num;
    init.bSoftReqEn  = config->soft_req;
    init.bHiPriority = config->high_priority;

    rom_hw_dma_channel_init(&init);
}

void slc_hal_dma_deinit(hal_dma_ch_e dma_ch)
{
    rom_hw_dma_enable_channel((EN_DMA_CHANNEL_T)dma_ch, false);
}

void slc_hal_dma_transfer_cfg(hal_dma_ch_e dma_ch, hal_dma_cfg_t *cfg)
{
    EN_DMA_CFG_T dma_cfg;

    if (dma_ch >= HAL_DMA_CH_MAX)
        return;

    dma_cfg.src_addr = cfg->src_addr;
    dma_cfg.dst_addr = cfg->dst_addr;
    dma_cfg.single_pkg_size = cfg->single_pkg_size;
    dma_cfg.muli_trigger_num = cfg->muli_trigger_num;
    dma_cfg.src_addr_rise = cfg->src_addr_rise;
    dma_cfg.dst_addr_rise = cfg->dst_addr_rise;

    rom_hw_dma_transfer_cfg((EN_DMA_CHANNEL_T)dma_ch, &dma_cfg);
}

void slc_hal_dma_start(hal_dma_ch_e dma_ch)
{
    if (dma_ch >= HAL_DMA_CH_MAX)
        return;

    rom_hw_dma_enable_channel((EN_DMA_CHANNEL_T)dma_ch, true);
}

void slc_hal_dma_abort(hal_dma_ch_e dma_ch)
{
    if (dma_ch >= HAL_DMA_CH_MAX)
        return;

    rom_hw_dma_enable_channel((EN_DMA_CHANNEL_T)dma_ch, false);
}

uint32_t slc_hal_dma_get_finish_size(hal_dma_ch_e dma_ch)
{
    uint32_t size = 0;

    if (dma_ch >= HAL_DMA_CH_MAX)
        return 0;

    rom_hw_dma_channel_get_finish_num((EN_DMA_CHANNEL_T)dma_ch, (uint16_t *)&size);
    return size;
}

uint32_t slc_hal_dma_get_interrupt_status(void)
{
    uint32_t sta = 0;

    rom_hw_dma_get_interrupt_flag(&sta);
    return sta;
}

void slc_hal_dma_clear_interrupt_status(uint32_t sta)
{
    rom_hw_dma_clear_interrupt_flag(sta);
}
