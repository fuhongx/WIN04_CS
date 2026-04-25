/**
 * ****************************************************************************
 * @file hw_dma.c
 * @author SLC(SC) software team.
 * @brief
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
#include "hw_dma.h"

/**
 * @brief Configure the source&destination address for DMA migration.
 *
 * @param pstHandle Must be DMA_CH1~DMA_CH7.
 * @param src Source Address.
 * @param dst Destination Address.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_set_addr(stDMAChannel_t *pstHandle, uint32_t src, uint32_t dst)
{
    if (NULL == pstHandle)
        return EN_ERROR_STA_INVALID;

    pstHandle->SRC_ADDR = src;
    pstHandle->DST_ADDR = dst;

    return EN_ERROR_STA_OK;
}

/**
 * @brief Set the number of transmit and multiple triggers. Use 1 for a single trigger.
 *
 * @param pstHandle Must be DMA_CH1~DMA_CH7.
 * @param u16packByte The length of single package. The unit is byte
 * @param u16TrigNum The num of package to be transferred.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_set_trans_num(stDMAChannel_t *pstHandle, uint16_t u16packByte, uint16_t u16TrigNum)
{
    if (NULL == pstHandle)
        return EN_ERROR_STA_INVALID;

    pstHandle->TRANS_NUM = ((u16TrigNum & DMA_TRANS_NUM_MASK) << DMA_TRANS_NUM_SHIFT) |
                            ((u16packByte & DMA_TRIG_NUM_MASK) << DMA_TRIG_NUM_SHIFT);

    return EN_ERROR_STA_OK;
}

/**
 * @brief Gets the number of transfers completed.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param ByteNum CHx transmitted data that has been completed. The unit is byte.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_get_finish_num(EN_DMA_CHANNEL_T u8Channel, uint16_t *ByteNum)
{
    if (NULL == ByteNum)
        return EN_ERROR_STA_INVALID;

    stDMAChannel_t *pstHandle = (stDMAChannel_t *)DMA_CHX_BASE_ADDR(u8Channel);

    *ByteNum = (uint16_t)(pstHandle->FINE_BYTE_NUM & DMA_FINE_NUM_REG_MASK);
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure DMA trigger mode, software/peripherals.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param bSoftEn true: soft. false: peripherals.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_request_mode_config(EN_DMA_CHANNEL_T u8Channel, bool bSoftEn)
{
    uint32_t u32Cfg = DMA->DMA_REQ_SET;
    if (bSoftEn)
        u32Cfg |= (1U << u8Channel);
    else
        u32Cfg &= ~(1U << u8Channel);

    DMA->DMA_REQ_SET = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure DMA priorities, only high and low.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param bHiPriority true: high. false: low.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_set_priority(EN_DMA_CHANNEL_T u8Channel, bool bHiPriority)
{
    uint32_t u32Cfg = DMA->DMA_PRIORITY_SET;
    if (bHiPriority)
        u32Cfg |= (1U << u8Channel);
    else
        u32Cfg &= ~(1U << u8Channel);

    DMA->DMA_PRIORITY_SET = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Enable DMA Channel.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param bEnable true: enable. false: disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_enable_channel(EN_DMA_CHANNEL_T u8Channel, bool bEnable)
{
    if (bEnable)
        *((volatile uint32_t *)(DMA_CHX_EN_REG(u8Channel))) |= 0x1U;
    else
        *((volatile uint32_t *)(DMA_CHX_EN_REG(u8Channel))) &= ~0x1U;

    if ((DMA->DMA_REQ_SET & (1U << u8Channel)) && bEnable)
        // if soft request mode, need to set a req after enable channel
        DMA->DMA_SOFT_REQ |= (1U << u8Channel);


    return EN_ERROR_STA_OK;
}

/**
 * @brief Gets the DMA interrupt flag.
 *
 * @param pu32Flag Pointer to Buffer. @ref EN_DMA_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_get_interrupt_flag(uint32_t *pu32Flag)
{
    *pu32Flag = DMA->DMA_INT_STATE;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Clear the DMA interrupt flag.
 *
 * @param u32Flag @ref EN_DMA_INT_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_clear_interrupt_flag(uint32_t u32Flag)
{
    if ((u32Flag & EN_DMA_INT_FALT) != 0) {
        // clear fault flag need to disable NVIC irq first.
        NVIC_DisableIRQ(DMA_IRQ);
    }

    DMA->DMA_INT_CLR = u32Flag & DMA_INT_CLR_REG_MASK;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure the DMA mapping mode.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @param enMode @ref EN_DMA_REQ_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_set_request_map(EN_DMA_CHANNEL_T u8Channel, EN_DMA_REQ_T enMode)
{
    uint8_t i = 0;
    uint32_t u32Cfg = 0;

    if (u8Channel >= DMA_MAX_CHANNEL_NUM)
        return EN_ERROR_STA_INVALID;

    // CH6的map低2bit在DMA_REQ_MAP的bit[30:31]，高3bit在DMA_REQ2的bit[0:2]
    if (u8Channel == EN_DMA_CH6) {
        u32Cfg = DMA->DMA_REQ_MAP;
        u32Cfg &= ~(DMA_REQ_MAP0_LAST_BIT_MASK << (32 - DMA_REQ_MAP0_LAST_BIT_NUM));
        u32Cfg |= ((DMA_REQ_MAP0_LAST_BIT_MASK & enMode) << (32 - DMA_REQ_MAP0_LAST_BIT_NUM));
        DMA->DMA_REQ_MAP = u32Cfg;

        u32Cfg = DMA->DMA_REQ2;
        u32Cfg &= ~(DMA_REQ_MAP_BIT_MASK >> (DMA_REQ_MAP0_LAST_BIT_NUM));
        u32Cfg |= ((DMA_REQ_MAP_BIT_MASK & enMode) >> (DMA_REQ_MAP0_LAST_BIT_NUM));
        DMA->DMA_REQ2 = u32Cfg;

        return EN_ERROR_STA_OK;
    }

    // CH7的map在DMA_REQ2的bit[3:7]
    if (u8Channel == EN_DMA_CH7) {
        u32Cfg = DMA->DMA_REQ2;
        u32Cfg &= ~(DMA_REQ_MAP_BIT_MASK << (DMA_REQ_MAP_BIT_NUM - DMA_REQ_MAP0_LAST_BIT_NUM));
        u32Cfg |= ((DMA_REQ_MAP_BIT_MASK & enMode) << (DMA_REQ_MAP_BIT_NUM - DMA_REQ_MAP0_LAST_BIT_NUM));

        DMA->DMA_REQ2 = u32Cfg;
        return EN_ERROR_STA_OK;
    }

    // CH0~CH5的map在DMA_REQ_MAP的bit[0:4],bit[5:9],bit[10:14],bit[15:19],bit[20:24],bit[25:29]
    u32Cfg = DMA->DMA_REQ_MAP;
    u32Cfg &= ~(DMA_REQ_MAP_BIT_MASK << (DMA_REQ_MAP_BIT_NUM * u8Channel));
    u32Cfg |= (enMode << (DMA_REQ_MAP_BIT_NUM * u8Channel));
    DMA->DMA_REQ_MAP = u32Cfg;

    return EN_ERROR_STA_OK;
}

/**
 * @brief Reset dma by software.
 *
 * @param u8Channel @ref EN_DMA_CHANNEL_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_chanel_soft_reset(EN_DMA_CHANNEL_T u8Channel)
{
    DMA->CHL_RST_SW_CFG |= (1U << u8Channel);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_dma_transfer_cfg(EN_DMA_CHANNEL_T ch, EN_DMA_CFG_T *cfg)
{
    stDMAChannel_t *pstChHandle = (stDMAChannel_t *)DMA_CHX_BASE_ADDR(ch);
    unDMAChCfg_t unChCfg;
    uint8_t enable, sta;
    uint32_t timeout = 0;

    enable = read32(DMA_CHX_EN_REG(ch)) & DMA_CH_EN_REG_MASK;
    if (enable) {
        return EN_ERROR_STA_ERROR;
    }
    do {
        sta = (DMA->DMA_DEBUG & DMA_CH_STA_MASK(ch)) >> (ch*4);
        if (sta & DMA_CH_STA_IDLE)
            break;

        rom_utility_delay_us(1);
        timeout++;
    } while (timeout < DMA_ABORT_TIMEOUT_US);
    if (timeout >= DMA_ABORT_TIMEOUT_US) {
        return EN_ERROR_STA_TIMEOUT;
    }

    rom_hw_dma_channel_set_addr(pstChHandle, cfg->src_addr, cfg->dst_addr);

    rom_hw_dma_channel_set_trans_num(pstChHandle, cfg->single_pkg_size, cfg->muli_trigger_num);

    unChCfg.u32Cfg = pstChHandle->CTRL_CFG;
    unChCfg.stDMAChCfg.SrcAddrRiseEn = cfg->src_addr_rise;
    unChCfg.stDMAChCfg.DstAddrRiseEn = cfg->dst_addr_rise;
    pstChHandle->CTRL_CFG = unChCfg.u32Cfg & DMA_CTRL_CFG_REG_MASK;

    return EN_ERROR_STA_OK;
}

/**
 * @brief Initialize the DMA channel configuration. note: this function will not enable the channel.
 *
 * @param pstInit @ref stDMAInit_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_dma_channel_init(stDMAInit_t *pstInit)
{
    if (NULL == pstInit)
        return EN_ERROR_STA_INVALID;

    stDMAChannel_t *pstChHandle = (stDMAChannel_t *)DMA_CHX_BASE_ADDR(pstInit->enDmaCh);

    uint8_t u8Ret = EN_ERROR_STA_OK;
    uint8_t enable, sta;
    uint32_t timeout = 0;

    enable = read32(DMA_CHX_EN_REG(pstInit->enDmaCh)) & DMA_CH_EN_REG_MASK;
    if (enable) {
        return EN_ERROR_STA_ERROR;
    }

    do {
        sta = (DMA->DMA_DEBUG & DMA_CH_STA_MASK(pstInit->enDmaCh)) >> (pstInit->enDmaCh*4);
        if (sta & DMA_CH_STA_IDLE)
            break;

        rom_utility_delay_us(1);
        timeout++;
    } while (timeout < DMA_ABORT_TIMEOUT_US);
    if (timeout >= DMA_ABORT_TIMEOUT_US) {
        return EN_ERROR_STA_TIMEOUT;
    }

    // 0. reset channel when occur error or re-init
    rom_hw_dma_chanel_soft_reset(pstInit->enDmaCh);

    // 1. set src&dst Addr
    u8Ret = rom_hw_dma_channel_set_addr(pstChHandle, pstInit->u32SrcAddr, pstInit->u32DstAddr);
    if (u8Ret != EN_ERROR_STA_OK)
        return u8Ret;

    // 2. set package byte and trigle num
    u8Ret = rom_hw_dma_channel_set_trans_num(pstChHandle, pstInit->u16PackByte, pstInit->u16TrigNum);
    if (u8Ret != EN_ERROR_STA_OK)
        return u8Ret;

    // 3. set map
    u8Ret = rom_hw_dma_set_request_map(pstInit->enDmaCh, pstInit->enReqType);
    if (u8Ret != EN_ERROR_STA_OK)
        return u8Ret;

    // 4. set priority
    rom_hw_dma_set_priority(pstInit->enDmaCh, pstInit->bHiPriority);

    // 5. set req mode
    rom_hw_dma_request_mode_config(pstInit->enDmaCh, pstInit->bSoftReqEn);

    // 6. set CTRL CFG, such as ErrorIntEn, FinishIntEn, SrcDataType, DstDataType
    if ((pstInit->unChCfg.stDMAChCfg.SrcDataType != pstInit->unChCfg.stDMAChCfg.DstDataType) ||
        (pstInit->unChCfg.stDMAChCfg.SrcDataType > EN_DMA_DATA_TYPE_WORD))
        return EN_ERROR_STA_INVALID;

    pstChHandle->CTRL_CFG = pstInit->unChCfg.u32Cfg & DMA_CTRL_CFG_REG_MASK;

    return u8Ret;
}
