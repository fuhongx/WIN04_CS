/**
 * ****************************************************************************
 * @file hw_iic.c
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
#include "hw_iic.h"
#include "hw_sysctrl.h"

#define IIC_DATA_CMD_STOP   (1 << 9)
#define IIC_DATA_CMD_WRITE  (0 << 8)
#define IIC_DATA_CMD_READ   (1 << 8)

#define IIC_100K_HZ         (100000)
#define IIC_400K_HZ         (400000)
#define IIC_1M_HZ           (1000000)

#define IIC_HANDLE_INVALID_RETURN(pstHandle) \
    if (NULL == pstHandle)                   \
    {                                        \
        return EN_ERROR_STA_INVALID;         \
    }

EN_ERR_STA_T rom_hw_iic_wait_idle(stIicHandle_t *pstHandle, uint32_t u32Timeout, uint32_t u32Mask)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    uint32_t u32Time = 0;

    do {
        if ((pstHandle->IIC_STATUS & u32Mask) == 0)
            return EN_ERROR_STA_OK;

        u32Time++;
        rom_utility_delay_us(1);
    } while (u32Time < u32Timeout);

    if (u32Time >= u32Timeout)
        return EN_ERROR_STA_TIMEOUT;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_wait_enable_sta(stIicHandle_t *pstHandle, uint32_t u32Timeout, uint32_t u32Mask, bool bEnable)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    uint32_t u32Time = 0;

    do {
        if ((pstHandle->IIC_ENABLE_STATUS & u32Mask) == bEnable)
            return EN_ERROR_STA_OK;

        u32Time++;
        rom_utility_delay_us(1);
    } while (u32Time < u32Timeout);

    if (u32Time >= u32Timeout)
        return EN_ERROR_STA_TIMEOUT;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_mode_config(stIicHandle_t *pstHandle, EN_IIC_MODE_T enMode, EN_IIC_SPEED_T enSpeed)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);
    uint32_t u32Cfg = pstHandle->IIC_CON;
    u32Cfg &= ~((IIC_CON_MASTER_MODE_EN_MASK << IIC_CON_MASTER_MODE_EN_SHIFT) |
                (IIC_CON_FAST_MODE_EN_MASK << IIC_CON_FAST_MODE_EN_SHIFT) |
                (IIC_CON_SLAVE_DISABLE_MASK << IIC_CON_SLAVE_DISABLE_SHIFT) |
                (IIC_CON_RESTART_EN_MASK << IIC_CON_RESTART_EN_SHIFT));

    switch (enMode)
    {
        case EN_IIC_MODE_SLAVE:
        {
            u32Cfg &= ~(IIC_CON_SLAVE_DISABLE_MASK << IIC_CON_SLAVE_DISABLE_SHIFT);
            break;
        }

        case EN_IIC_MODE_MASTER:
        {
            u32Cfg |= (IIC_CON_MASTER_MODE_EN_MASK << IIC_CON_MASTER_MODE_EN_SHIFT);
            break;
        }

        default:
            return EN_ERROR_STA_ERROR;
    }

    if (enSpeed == EN_IIC_SPEED_FAST_PLUS_1M)
        enSpeed = EN_IIC_SPEED_FAST_400K;
    u32Cfg |= ((enSpeed & IIC_CON_FAST_MODE_EN_MASK) << IIC_CON_FAST_MODE_EN_SHIFT);

    pstHandle->IIC_CON = (u32Cfg | (IIC_CON_RESTART_EN_MASK << IIC_CON_RESTART_EN_SHIFT));
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_set_slave_addr(stIicHandle_t *pstHandle, uint16_t u16Addr, EN_IIC_ADDR_LEN_T enAddrLen)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    uint32_t u32Cfg = pstHandle->IIC_CON;
    u32Cfg &= ~(IIC_CON_SLAVE_ADDR_LEN_MASK << IIC_CON_SLAVE_ADDR_LEN_SHIFT);
    u32Cfg |= (enAddrLen << IIC_CON_SLAVE_ADDR_LEN_SHIFT);
    pstHandle->IIC_CON = u32Cfg;

    pstHandle->IIC_SAR = u16Addr & IIC_SAR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_set_target_addr(stIicHandle_t *pstHandle, uint16_t u16Addr, EN_IIC_ADDR_LEN_T enAddrLen)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    uint32_t u32Cfg = pstHandle->IIC_CON;
    u32Cfg &= ~(IIC_CON_MASTER_ADDR_LEN_MASK << IIC_CON_MASTER_ADDR_LEN_SHIFT);
    u32Cfg |= (enAddrLen << IIC_CON_MASTER_ADDR_LEN_SHIFT);
    pstHandle->IIC_CON = u32Cfg;

    uint16_t u16Cfg = pstHandle->IIC_TAR;
    u16Cfg &= ~(IIC_TAR_TARGET_SLAVE_ADDR_MASK << IIC_TAR_TARGET_SLAVE_ADDR_SHIFT);
    u16Cfg |= u16Addr & IIC_TAR_TARGET_SLAVE_ADDR_MASK;

    pstHandle->IIC_TAR = u16Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_enable_interrupt(stIicHandle_t *pstHandle, uint16_t u16Mask)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->IIC_INTR_MASK = u16Mask & IIC_INTR_MASK_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_disable_interrupt(stIicHandle_t *pstHandle, uint16_t u16Mask)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->IIC_INTR_MASK &= (~u16Mask & IIC_INTR_MASK_REG_MASK);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_get_interrupt_flag(stIicHandle_t *pstHandle, uint16_t *pu16Flag)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);
    if (NULL == pu16Flag)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu16Flag = pstHandle->IIC_RAW_INTR_STAT & IIC_RAW_INTR_STAT_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_clear_interrupt_flag(stIicHandle_t *pstHandle, uint16_t u16Flag)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    u16Flag = pstHandle->IIC_CLR_INTR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_set_fifo_threshold(stIicHandle_t *pstHandle, uint8_t u8TxThld, uint8_t u8RxThld)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->IIC_TX_TL = u8TxThld & IIC_TX_TL_REG_MASK;
    pstHandle->IIC_RX_TL = u8RxThld & IIC_RX_TL_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_enable(stIicHandle_t *pstHandle, bool bEnable)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    uint8_t u8Cfg = pstHandle->IIC_ENABLE;
    u8Cfg &= ~(IIC_ENABLE_EN_MASK << IIC_ENABLE_EN_SHIFT);
    if (bEnable)
        u8Cfg |= (IIC_ENABLE_EN_MASK << IIC_ENABLE_EN_SHIFT);

    pstHandle->IIC_ENABLE = u8Cfg;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_get_status(stIicHandle_t *pstHandle, uint8_t *pu8Sta)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    if (NULL == pu8Sta)
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Sta = pstHandle->IIC_STATUS;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_enable_dma_transmit(stIicHandle_t *pstHandle, EN_IIC_DMA_TRX_T enMode)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->IIC_DMA_CR = enMode & IIC_DMA_CR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_set_dma_trx_threshold(stIicHandle_t *pstHandle, uint8_t u8TxThld, uint8_t u8RxThld)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    pstHandle->IIC_DMA_TDLR = u8TxThld & IIC_DMA_TDLR_REG_MASK;

    pstHandle->IIC_DMA_RDLR = u8RxThld & IIC_DMA_RDLR_REG_MASK;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_set_scl_cnt(stIicHandle_t *pstHandle, EN_IIC_SPEED_T enSpeed)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    switch (enSpeed)
    {
        case EN_IIC_SPEED_STD_100K:
            pstHandle->IIC_SS_SCL_HCNT = (SystemCoreClock / IIC_100K_HZ / 2) - 7;
            pstHandle->IIC_SS_SCL_LCNT = (SystemCoreClock / IIC_100K_HZ / 2) - 1;
            break;

        case EN_IIC_SPEED_FAST_400K:
            pstHandle->IIC_FS_SPKLEN = 1;
            pstHandle->IIC_FS_SCL_HCNT = (SystemCoreClock / IIC_400K_HZ / 2) - 7 - pstHandle->IIC_FS_SPKLEN;
            pstHandle->IIC_FS_SCL_LCNT = (SystemCoreClock / IIC_400K_HZ / 2) - 1;
            break;

        case EN_IIC_SPEED_FAST_PLUS_1M:
            pstHandle->IIC_FS_SPKLEN = 1;
            pstHandle->IIC_FS_SCL_HCNT = (SystemCoreClock / IIC_1M_HZ / 2) - 7 - pstHandle->IIC_FS_SPKLEN;
            pstHandle->IIC_FS_SCL_LCNT = (SystemCoreClock / IIC_1M_HZ / 2) - 1;
            break;

        default:
            break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_master_tx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t u16Len, uint32_t u32Timeout)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    int ret = EN_ERROR_STA_OK;
    uint16_t i = 0;
    uint16_t tx_once = 0;
    uint16_t index = 0;
    uint32_t loop_cycle = 0;

    if (NULL == u8Buf)
        return EN_ERROR_STA_INVALID;

    // Wait IIC idle
    ret = rom_hw_iic_wait_idle(pstHandle, u32Timeout, IIC_STATUS_MASTER_BUSY_MASK << IIC_STATUS_MASTER_BUSY_SHIFT);
    if (ret != EN_ERROR_STA_OK)
        return ret;

    do {
        tx_once = IIC_MAX_FIFO_DEPTH - pstHandle->IIC_TXFLR;
        tx_once = (tx_once > (u16Len - index)) ? (u16Len - index) : tx_once;
        for (i = 0; i < tx_once; i++) {
            if (index == (u16Len - 1)) {
                pstHandle->IIC_DATA_CMD = u8Buf[index++] | IIC_DATA_CMD_WRITE | IIC_DATA_CMD_STOP;
            } else {
                pstHandle->IIC_DATA_CMD = u8Buf[index++] | IIC_DATA_CMD_WRITE;
            }
        }

        if (tx_once == 0) {
            loop_cycle++;
            if (loop_cycle > u32Timeout)
                return EN_ERROR_STA_TIMEOUT;
        }
    } while (index < u16Len);

    // Wait TX FIFO empty
    loop_cycle = 0;
    do {
        if ((pstHandle->IIC_STATUS & (IIC_STATUS_TX_FIFO_EMPTY_MASK << IIC_STATUS_TX_FIFO_EMPTY_SHIFT)) != 0)
            return EN_ERROR_STA_OK;

        loop_cycle++;
        rom_utility_delay_us(1);
    } while (loop_cycle < u32Timeout);

    if (loop_cycle >= u32Timeout)
        return EN_ERROR_STA_TIMEOUT;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_iic_master_rx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t *u16Len, uint32_t u32Timeout)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    int ret = EN_ERROR_STA_OK;
    uint16_t rd_once = 0;
    uint16_t rd_left = *u16Len;
    uint16_t rd_req = *u16Len;
    uint16_t i = 0;
    uint16_t index = 0;
    uint32_t loop_cycle = 0;

    if (NULL == u8Buf || *u16Len == 0 || NULL == u16Len)
        return EN_ERROR_STA_INVALID;

    *u16Len = 0;

    // Wait IIC idle
    ret = rom_hw_iic_wait_idle(pstHandle, u32Timeout, IIC_STATUS_MASTER_BUSY_MASK << IIC_STATUS_MASTER_BUSY_SHIFT);
    if (ret != EN_ERROR_STA_OK)
        return ret;

    do {
        if (rd_req > 0) {
            rd_once = IIC_MAX_FIFO_DEPTH - pstHandle->IIC_TXFLR;
            rd_once = (rd_once > rd_req) ? rd_req : rd_once;
            // send read reqest
            for (i = 0; i < rd_once; i++) {
                if ((rd_req - i) == 1)
                    pstHandle->IIC_DATA_CMD = IIC_DATA_CMD_READ | IIC_DATA_CMD_STOP;
                else
                    pstHandle->IIC_DATA_CMD = IIC_DATA_CMD_READ;
            }

            rd_req -= rd_once;
        }

        if (rd_left > 0) {
            rd_once = pstHandle->IIC_RXFLR;
            for (i = 0; i < rd_once; i++)
                u8Buf[index++] = pstHandle->IIC_DATA_CMD & IIC_DATA_CMD_DATA_MASK;

            rd_left -= rd_once;
            *u16Len += rd_once;
        }

        if (rd_once == 0) {
            loop_cycle++;
            if (loop_cycle > u32Timeout)
                return EN_ERROR_STA_TIMEOUT;
        }
    } while ((rd_req > 0) || (rd_left > 0));

    return ret;
}

EN_ERR_STA_T rom_hw_iic_slave_tx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t u16Len, uint32_t u32Timeout)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    int ret = EN_ERROR_STA_OK;
    uint16_t i = 0;
    uint32_t rd_req = 0;
    uint16_t tx_once = 0;
    uint16_t index = 0;
    uint32_t loop_cycle = 0;

    if (NULL == u8Buf)
        return EN_ERROR_STA_INVALID;

    do {
        rd_req = pstHandle->IIC_CLR_RD_REQ;

        tx_once = IIC_MAX_FIFO_DEPTH - pstHandle->IIC_TXFLR;
        tx_once = (tx_once > (u16Len - index)) ? (u16Len - index) : tx_once;

        for (i = 0; i < tx_once; i++)
            pstHandle->IIC_DATA_CMD = u8Buf[index++] | IIC_DATA_CMD_WRITE;

        if (tx_once == 0) {
            loop_cycle++;
            if (loop_cycle > u32Timeout)
                return EN_ERROR_STA_TIMEOUT;
        }
    } while (index < u16Len);

    return ret;
}

EN_ERR_STA_T rom_hw_iic_slave_rx(stIicHandle_t *pstHandle, uint8_t *u8Buf, uint16_t *u16Len, uint32_t u32Timeout)
{
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    int ret = EN_ERROR_STA_OK;
    uint16_t rd_once = 0;
    uint16_t i = 0;
    uint16_t index = 0;
    uint32_t loop_cycle = 0;

    if (NULL == u8Buf || *u16Len == 0 || NULL == u16Len)
        return EN_ERROR_STA_INVALID;

    uint16_t rd_left = *u16Len;
    *u16Len = 0;

    do {
        rd_once = pstHandle->IIC_RXFLR;
        rd_once = (rd_once > rd_left) ? rd_left : rd_once;
        for (i = 0; i < rd_once; i++)
            u8Buf[index++] = pstHandle->IIC_DATA_CMD & IIC_DATA_CMD_DATA_MASK;

        rd_left -= rd_once;
        *u16Len = index;

        if (rd_once == 0) {
            loop_cycle++;
            if (loop_cycle > u32Timeout)
                return EN_ERROR_STA_TIMEOUT;
        }
    } while (rd_left > 0);

    return ret;
}

EN_ERR_STA_T rom_hw_iic_init(stIicHandle_t *pstHandle, stIicInt_t *pstInit)
{
    uint32_t u32Cfg;
    IIC_HANDLE_INVALID_RETURN(pstHandle);

    if (NULL == pstInit)
    {
        return EN_ERROR_STA_INVALID;
    }

    rom_hw_iic_wait_idle(pstHandle, IIC_WAIT_TIMEOUT, IIC_STATUS_ACTIVITY_MASK << IIC_STATUS_ACTIVITY_SHIFT);

    rom_hw_iic_enable(pstHandle, false);

    rom_hw_iic_enable_interrupt(pstHandle, 0x0);

    rom_hw_iic_wait_enable_sta(pstHandle, IIC_WAIT_TIMEOUT, IIC_ENABLE_STATUS_IIC_EN_MASK << IIC_ENABLE_STATUS_IIC_EN_SHIFT, false);

    rom_hw_iic_mode_config(pstHandle, pstInit->enMode, pstInit->enSpeed);

    rom_hw_iic_set_target_addr(pstHandle, pstInit->u16TargetAddr, pstInit->enTargetAddrLen);

    rom_hw_iic_set_slave_addr(pstHandle, pstInit->u16OwnSlaveAddr, pstInit->enOwnSlaveAddrLen);

    rom_hw_iic_set_fifo_threshold(pstHandle, pstInit->u8TxTL, pstInit->u8RxTL);

    rom_hw_iic_set_scl_cnt(pstHandle, pstInit->enSpeed);

    rom_hw_iic_enable(pstHandle, true);

    rom_hw_iic_wait_enable_sta(pstHandle, IIC_WAIT_TIMEOUT, IIC_ENABLE_STATUS_IIC_EN_MASK << IIC_ENABLE_STATUS_IIC_EN_SHIFT, true);

    return EN_ERROR_STA_OK;
}
