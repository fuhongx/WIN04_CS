/**
 * ****************************************************************************
 * @file hw_flash.c
 * @author SLC(SC) software team.
 * @brief This file is the Flash_Ctrl module driver of the WIN series chip.
 *        The drive is based on the GT25Q20D.
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
#include "hw_flash.h"
#include "app_cfg.h"
#include "gt25q20d.h"
#include "p25q21l.h"
#include "compiler.h"

volatile uint8_t gu8FlashType __ROM_RAM_SHARED = 0;

#define FLASH_TIMEOUT_US (2000)

//------------------------------------------------------------------------------
// Flash ctrl apis
//------------------------------------------------------------------------------

void rom_hw_flash_set_type(EN_FLASH_TYPE_T type)
{
    gu8FlashType = type;
}

uint8_t rom_hw_flash_get_type(void)
{
    return gu8FlashType;
}

/**
 * @brief Get flash ctrl is idle or others.
 *
 * @return true: Qspi Idle.
 * @return false Qspi Busy.
 */
bool rom_hw_flash_ctrl_get_qspi_status(void)
{
    return (FLASH_CTRL->QSPI_CFG & (FLASH_CTRL_QSPI_CFG_STA_MASK << FLASH_CTRL_QSPI_CFG_STA_SHIFT)) >> FLASH_CTRL_QSPI_CFG_STA_SHIFT;
}

/**
 * @brief Query flash busy status reg.
 *
 * @return true: flash not busy.
 * @return false: flash busy.
 */
bool rom_hw_flash_ctrl_query_qspi_busy(void)
{
    uint16_t u16TimeOut = 0;
    while (!rom_hw_flash_ctrl_get_qspi_status())
    {
        rom_utility_delay_us(10);
        u16TimeOut += 10;
        if (u16TimeOut >= FLASH_TIMEOUT_US)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief get flash cmd finish flah.
 *
 * @return true: last cmd not finish.
 * @return false: last cmd finish.
 */
bool rom_hw_flash_ctrl_get_flash_cmd_flag(void)
{
    return (FLASH_CTRL->FLASH_CMD_CRTL & (FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK << FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT)) >> FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT;
}

bool rom_hw_flash_ctrl_wait_flash_cmd_done(void)
{
    uint16_t u16TimeOut = 0;
    while (rom_hw_flash_ctrl_get_flash_cmd_flag())
    {
        rom_utility_delay_us(10);
        u16TimeOut += 10;
        if (u16TimeOut >= FLASH_TIMEOUT_US)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Select the QSPI mode.
 *
 * @param enMode @ref EN_FLASH_CTRL_SPI_MODE_T.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_qspi_mode(EN_FLASH_CTRL_SPI_MODE_T enMode)
{
    if (EN_FLASH_QSPI_MODE_MAX <= enMode)
    {
        return EN_ERROR_STA_ERROR;
    }

    uint32_t u32Cfg = FLASH_CTRL->QSPI_CFG;

    // Clear bit
    u32Cfg &= (~((FLASH_CTRL_QSPI_CFG_CLK_POL_MASK << FLASH_CTRL_QSPI_CFG_CLK_POL_SHIFT) |
                 (FLASH_CTRL_QSPI_CFG_CLK_PHA_MASK << FLASH_CTRL_QSPI_CFG_CLK_PHA_SHIFT)));
    u32Cfg |= (enMode << FLASH_CTRL_QSPI_CFG_CLK_POL_SHIFT);

    if (!rom_hw_flash_ctrl_query_qspi_busy())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    FLASH_CTRL->QSPI_CFG = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Set qspi clock divider.
 *
 * @param enDiv @ref EN_FLASH_CLK_DIV_T
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_clk_div(EN_FLASH_CLK_DIV_T enDiv)
{
    if (FLASH_CTRL_QSPI_CFG_CLK_DIV_MASK < enDiv)
    {
        return EN_ERROR_STA_ERROR;
    }

    uint32_t u32Cfg = FLASH_CTRL->QSPI_CFG;
    // Clear bit
    u32Cfg &= (~((FLASH_CTRL_QSPI_CFG_CLK_DIV_MASK << FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT)));
    u32Cfg |= (enDiv << FLASH_CTRL_QSPI_CFG_CLK_DIV_SHIFT);

    if (!rom_hw_flash_ctrl_query_qspi_busy())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    FLASH_CTRL->QSPI_CFG = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Init flash ctrl, given flash_ctrl setting.
 *
 * @param unCfg @ref unFlashCrtlInit_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_init(unFlashCrtlInit_t unCfg)
{
    if (!rom_hw_flash_ctrl_query_qspi_busy())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    FLASH_CTRL->QSPI_CFG = (unCfg.u32Cfg & FLASH_CTRL_QSPI_CFG_REG_MASK);
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure the data read command structure.
 *
 * @param unInst @ref unFlashReadInst_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst_t unInst)
{
    FLASH_CTRL->READ_INSTRUCT = (unInst.u32Cfg & FLASH_CTRL_READ_INST_REG_MASK);
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure the command structure for writing data.
 *
 * @param unInst @ref unFlashWriteInst_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_write_instruction(unFlashWriteInst_t unInst)
{
    FLASH_CTRL->WRITE_INSTRUCT = (unInst.u32Cfg & FLASH_CTRL_WRITE_INST_REG_MASK);
    return EN_ERROR_STA_OK;
}

/**
 * @brief For details about how to configure the waiting time during Flash operations,
 *        see the Flash data manual (GT25Q20D).
 *
 * @param unDelay @ref unFlashQspiDelay_t.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_qspi_delay(unFlashQspiDelay_t unDelay)
{
    FLASH_CTRL->QSPI_DELAY = (unDelay.u32Cfg & FLASH_CTRL_QSPI_DELAY_REG_MASK);
    return EN_ERROR_STA_OK;
}

/**
 * @brief Configure the delay and edge when reading QSPI.
 *
 * @param u8SampDelay The delay is the number of reference clocks.
 * @param u8Edge 0: Use ref_clk to sample flash output along the falling edge.
 *               1: use ref_clk to sample flash output along the rising edge
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_read_data_capture(uint8_t u8SampDelay, uint8_t u8Edge)
{
    if ((FLASH_CTRL_READ_CAP_SAMPLING_EDGE_MASK < u8Edge) | (FLASH_CTRL_READ_CAP_SAMPLING_DELAY_MASK < u8SampDelay))
    {
        return EN_ERROR_STA_INVALID;
    }
    uint32_t u32Cfg = FLASH_CTRL->READ_DATA_CAP;
    u32Cfg &= ~(FLASH_CTRL_READ_CAP_SAMPLING_DELAY_MASK << FLASH_CTRL_READ_CAP_SAMPLING_DELAY_SHIFT);
    u32Cfg &= ~(FLASH_CTRL_READ_CAP_SAMPLING_EDGE_MASK << FLASH_CTRL_READ_CAP_SAMPLING_EDGE_SHIFT);

    u32Cfg |= (u8SampDelay << FLASH_CTRL_READ_CAP_SAMPLING_DELAY_SHIFT);
    u32Cfg |= (u8Edge << FLASH_CTRL_READ_CAP_SAMPLING_EDGE_SHIFT);

    FLASH_CTRL->READ_DATA_CAP = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief The Mode bit is configured. After the mode bit is enabled, it is sent
 *        to the flash immediately following the address byte (1byte).
 *
 * @param u8ModeBits Mode byte wait to send.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_mode_bits(uint8_t u8ModeBits)
{
    // No need recheck
    FLASH_CTRL->MODE_BIT_CFG = u8ModeBits;
    return EN_ERROR_STA_OK;
}

/**
 * @brief If write protection is required, set the current Flash parameters.
 *
 * @param u8BlockSize The number of bytes in each block must be a power of two.
 *                    0 = 1Byte
 *                    1 = 2Bytes
 *                    2 = 4Bytes
 *                    16 = 65535Bytes
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_protect_block_size(uint8_t u8BlockSize)
{
    uint32_t u32Cfg = FLASH_CTRL->DEV_SIZE_CFG;
    // No need recheck
    u32Cfg &= ~(FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_MASK << FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_SHIFT);
    u32Cfg |= (u8BlockSize << FLASH_CTRL_DEV_SIZE_CFG_BLOCK_SIZE_SHIFT);
    FLASH_CTRL->DEV_SIZE_CFG = u32Cfg;

    return EN_ERROR_STA_OK;
}

/**
 * @brief Defines the block number of the address range to be protected.
 *
 * @param u32LowIdx Low Area block number.
 * @param u32HiIdx High Area block number
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_write_protection_block_index(uint32_t u32LowIdx, uint32_t u32HiIdx)
{
    FLASH_CTRL->LOW_WRITE_PROTECT = u32LowIdx;
    FLASH_CTRL->UPPER_WRITE_PROTECT = u32HiIdx;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Enable write protection.
 *
 * @param bEnable True: enable write protection.
 *                False: disable write protection.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_enable_write_protecion(bool bEnable)
{
    uint32_t u32Cfg = FLASH_CTRL->WRITE_PROTECT;
    u32Cfg &= ~(FLASH_CTRL_WRITE_PROTECT_EN_MASK << FLASH_CTRL_WRITE_PROTECT_EN_SHIFT);
    u32Cfg |= (bEnable << FLASH_CTRL_WRITE_PROTECT_EN_SHIFT);
    FLASH_CTRL->WRITE_PROTECT = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Enable write protection reverse.
 *
 * @param bReverse
 *        true: When set to "1", the protected region defined in the
 *              "high region" and "low region" write protect registers is inverted,
 *              meaning that it is the region to which the system is allowed to write.
 *        false: When set to "0", the protected areas defined in the "high zone" and "Low zone"
 *               write protection registers are the areas that the system does not allow to write.
 *               This bit is modified only if the write protection function (by the first bit of the write protection register) is 0.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_enbale_write_protection_reverse(bool bReverse)
{
    uint32_t u32Cfg = FLASH_CTRL->WRITE_PROTECT;
    u32Cfg &= ~(FLASH_CTRL_WRITE_PROTECT_REVERSE_MASK << FLASH_CTRL_WRITE_PROTECT_REVERSE_SHIFT);
    u32Cfg |= (bReverse << FLASH_CTRL_WRITE_PROTECT_REVERSE_SHIFT);
    FLASH_CTRL->WRITE_PROTECT = u32Cfg;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Send commands to Flash through QSPI.
 *
 * @param u32Cmd Wait for the command to be sent.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_send_cmd(uint32_t u32Cmd)
{
    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    FLASH_CTRL->FLASH_CMD_CRTL = u32Cmd;
    return EN_ERROR_STA_OK;
}

/** Query flash ctrl status.*/
bool rom_hw_flash_ctrl_cmd_done(void)
{
    return (FLASH_CTRL->FLASH_CMD_CRTL >> FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_SHIFT) & FLASH_CTRL_FLASH_CMD_CTRL_EXECUTE_STA_MASK;
}

/**
 * @brief Configure the address for sending commands.
 *
 * @param u32Addr The addr wait to send.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_cmd_addr(uint32_t u32Addr)
{
    FLASH_CTRL->FLASH_CMD_ADDR = u32Addr;
    return EN_ERROR_STA_OK;
}

/**
 * @brief Obtain the data after reading the Flash by running commands.
 *
 * @param pu8Buffer Points to the stored Buffer.
 * @param u8Len The read length cannot exceed 8 bytes.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_get_read_data(uint8_t *pu8Buffer, uint8_t u8Len)
{
    if ((FLASH_CTRL_FLASH_READ_DATA_MAX_SIZE_ONCE < u8Len) | (0 == u8Len))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32ReadData = FLASH_CTRL->FLASH_READ_DATA_LOW;
    uint32_t u32ReadDataHi = FLASH_CTRL->FLASH_READ_DATA_HI;
    for (uint8_t i = 0; i < u8Len; i++)
    {
        if (i < 4)
        {
            pu8Buffer[i] = (u32ReadData >> ((i) * 8));
        }
        else
        {
            pu8Buffer[i] = (u32ReadDataHi >> ((i - 4) * 8));
        }
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief Data written to the Flash by running commands.
 *
 * @param pu8Buffer A pointer to the Buffer where data is written.
 * @param u8Len The write length cannot exceed 8 bytes.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_ctrl_set_write_data(uint8_t *pu8Buffer, uint8_t u8Len)
{
    if ((FLASH_CTRL_FLASH_WRITE_DATA_MAX_SIZE_ONCE < u8Len) | (0 == u8Len))
    {
        return EN_ERROR_STA_INVALID;
    }
    uint32_t u32DataLo = 0;
    uint32_t u32DataHi = 0;
    uint8_t u8Pos = 0;
    for (uint8_t i = 0; i < u8Len; i++)
    {
        if (i < 4)
        {
            u32DataLo |= (pu8Buffer[i] << (i * 8));
        }
        else
        {
            u32DataHi |= (pu8Buffer[i] << (u8Pos * 8));
            u8Pos++;
        }
    }

    FLASH_CTRL->FLASH_WRITE_DATA_LOW = u32DataLo;
    if (u8Len > 4)
    {
        FLASH_CTRL->FLASH_WRITE_DATA_HI = u32DataHi;
    }

    return EN_ERROR_STA_OK;
}

//------------------------------------------------------------------------------
// Flash apis
//------------------------------------------------------------------------------
// Query cmd
bool rom_hw_flash_wait_cmd_done(void)
{
    uint16_t u16TimeOut = 0;
    while (!rom_hw_flash_ctrl_cmd_done())
    {
        rom_utility_delay_us(10);
        u16TimeOut += 10;
        if (u16TimeOut == FLASH_TIMEOUT_US)
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Send the write enable command to the Flash.
 *
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_write_enable_cmd(void)
{
    uint32_t u32Cmd = FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_WRITE_ENABLE);

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (false == rom_hw_flash_wait_cmd_done())
    {
        return EN_ERROR_STA_ERROR;
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief Erase Erases the contents held by the specified Flash address.
 *
 * @param enErase @ref EN_FLASH_ERASE_T.
 * @param u32Addr Absolute Flash address to be erased.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_erase(EN_FLASH_ERASE_T enErase, uint32_t u32Addr)
{
    uint32_t u32Cmd = 0;
    uint32_t delay = PY_FLASH_TIMING_SE_US;

    switch (enErase)
    {
        case EN_FLASH_ERASE_SECTOR:
        {
            u32Cmd = FLASH_CTRL_CMD_ADDR_NODATA_INIT(GT_FLASH_CMD_SECTOR_ERASE);
            delay = (gu8FlashType == EN_FLASH_TYPE_PY) ? PY_FLASH_TIMING_SE_US : GT_FLASH_TIMING_SECTOR_ERASE_US;
            break;
        }

        case EN_FLASH_ERASE_32K:
        {
            u32Cmd = FLASH_CTRL_CMD_ADDR_NODATA_INIT(GT_FLASH_CMD_32K_ERASE);
            delay = (gu8FlashType == EN_FLASH_TYPE_PY) ? PY_FLASH_TIMING_BE32_US : GT_FLASH_TIMING_32K_ERASE_US;
            break;
        }

        case EN_FLASH_ERASE_64K:
        {
            u32Cmd = FLASH_CTRL_CMD_ADDR_NODATA_INIT(GT_FLASH_CMD_64K_ERASE);
            delay = (gu8FlashType == EN_FLASH_TYPE_PY) ? PY_FLASH_TIMING_BE64_US : GT_FLASH_TIMING_64K_ERASE_US;
            break;
        }

        case EN_FLASH_ERASE_CHIP:
        {
            // PY & GT support chip erase cmd 0xC7/0x60
            if (gu8FlashType == EN_FLASH_TYPE_PY) {
                u32Cmd = FLASH_CTRL_CMD_NOADDR_NODATA_INIT(PY_FLASH_CMD_CE);
                delay = PY_FLASH_TIMING_CE_US;
            } else {
                u32Cmd = FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_CHIP_ERASE);
                delay = GT_FLASH_TIMING_CHIP_ERASE_US;
            }
            break;
        }

        case EN_FLASH_ERASE_PAGE:
        {
            if (gu8FlashType == EN_FLASH_TYPE_PY) {
                u32Cmd = FLASH_CTRL_CMD_ADDR_NODATA_INIT(PY_FLASH_CMD_PE);
                delay = PY_FLASH_TIMING_PE_US;
            } else {
                u32Cmd = FLASH_CTRL_CMD_ADDR_NODATA_INIT(GT_FLASH_CMD_PAGE_WRITE);
                delay = GT_FLASH_TIMING_PAGE_PROGRAM_US;
            }
            break;
        }

        default:
            return EN_ERROR_STA_ERROR;
    }

    rom_hw_flash_ctrl_set_cmd_addr(u32Addr);

    rom_hw_flash_write_enable_cmd();

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    rom_utility_delay_us(delay);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_erase_by_length(uint32_t u32Addr, uint32_t u32Len)
{
    uint32_t u32AdjAddr = u32Addr;
    uint32_t u32MinEraseSize = GT_FLASH_SECTOR_SIZE_IN_BYTE;
    uint8_t u8Ret = EN_ERROR_STA_OK;

    if (u32AdjAddr & FLASH_BASE_ADDR)
    {
        u32AdjAddr &= ~(FLASH_BASE_ADDR);
    }

    // Addr illigal check
    if ((u32Addr & (u32MinEraseSize - 1)))
    {
        // Flash erase must be 4k-byte alignment(GT) or 256-byte alignment(PY).
        return EN_ERROR_STA_INVALID;
    }

    // Len illigal check
    if (u32Len % u32MinEraseSize)
    {
        return EN_ERROR_STA_INVALID;
    }

    while (u32Len)
    {
        // From big to little, Whole 64K block
        if ((u32Len >= GT_FLASH_64KBLOCK_SIZE_IN_BYTE) & (0 == (u32AdjAddr % GT_FLASH_64KBLOCK_SIZE_IN_BYTE)))
        {
            PRINTF("Erase 64k, start addr = 0x%x\n", u32AdjAddr);
            u8Ret |= rom_hw_flash_erase(EN_FLASH_ERASE_64K, u32AdjAddr);
            u32AdjAddr += GT_FLASH_64KBLOCK_SIZE_IN_BYTE;
            u32Len -= GT_FLASH_64KBLOCK_SIZE_IN_BYTE;
            continue;
        }
        else if ((u32Len >= GT_FLASH_32KBLOCK_SIZE_IN_BYTE) & (0 == (u32AdjAddr % GT_FLASH_32KBLOCK_SIZE_IN_BYTE))) // Whole 32K block
        {
            PRINTF("Erase 32k, start addr = 0x%x\n", u32AdjAddr);
            u8Ret |= rom_hw_flash_erase(EN_FLASH_ERASE_32K, u32AdjAddr);
            u32AdjAddr += GT_FLASH_32KBLOCK_SIZE_IN_BYTE;
            u32Len -= GT_FLASH_32KBLOCK_SIZE_IN_BYTE;
            continue;
        }
        else if ((u32Len >= GT_FLASH_SECTOR_SIZE_IN_BYTE) & (0 == (u32AdjAddr % GT_FLASH_SECTOR_SIZE_IN_BYTE))) // Whole 4K block
        {
            PRINTF("Erase 4k, start addr = 0x%x\n", u32AdjAddr);
            u8Ret |= rom_hw_flash_erase(EN_FLASH_ERASE_SECTOR, u32AdjAddr);
            u32AdjAddr += GT_FLASH_SECTOR_SIZE_IN_BYTE;
            u32Len -= GT_FLASH_SECTOR_SIZE_IN_BYTE;
            continue;
        }

        if (gu8FlashType == EN_FLASH_TYPE_PY)
        {
            if ((u32Len >= PY_FLASH_PAGE_SIZE_IN_BYTE) & (0 == (u32AdjAddr % PY_FLASH_PAGE_SIZE_IN_BYTE))) // Whole Page block
            {
                u8Ret |= rom_hw_flash_erase(EN_FLASH_ERASE_PAGE, u32AdjAddr);
                u32AdjAddr += PY_FLASH_PAGE_SIZE_IN_BYTE;
                u32Len -= PY_FLASH_PAGE_SIZE_IN_BYTE;
            }
        }
        else
        {
            PRINTF("Erase Error\n");
            return EN_ERROR_STA_ERROR;
        }
    }

    return u8Ret;
}

/**
 * @brief Write data to Flash using DAC mode.
 *
 * @param u32Addr The address to which data needs to be written.
 * @param pu8Buffer A pointer to the Buffer to which data is to be written.
 * @param u16Len Write data length.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_write_data(uint32_t u32Addr, uint8_t *pu8Buffer, uint32_t u32Len)
{
    uint32_t u32WriteData = 0;
    // Write directly
    uint32_t u32WordLen = u32Len / 4;
    uint32_t i = 0;
    uint32_t u32Cnt = 0;
    for (; i < u32WordLen; i++)
    {
        u32WriteData = 0;
        u32WriteData = rom_utility_little_endian_read_32(pu8Buffer, i * 4);

        *(volatile uint32_t *)(u32Addr + i * 4) = u32WriteData;
    }
    u32Cnt = u32Len - u32WordLen * 4; // left data

    if (u32Cnt)
    {
        u32WriteData = 0;
        i = 0;
        for (; i < u32Cnt; i++)
        {
            u32WriteData |= pu8Buffer[u32WordLen * 4 + i] << (i * 8);
        }
        *(volatile uint32_t *)(u32Addr + u32WordLen * 4) = u32WriteData;
    }
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_write_data_u32(uint32_t u32Addr, uint32_t *pu32Buffer, uint32_t u32Len)
{
    // Write directly
    uint32_t i = 0;
    uint32_t u32Cnt = 0;
    for (; i < u32Len; i++)
    {
        *(volatile uint32_t *)(u32Addr + i * 4) = pu32Buffer[i];
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief Read data from the flash.
 *
 * @param u32Addr The absolute address at which the data is read.
 * @param pu8Buffer A pointer to the buffer from which the data was read.
 * @param u16Len Read data length.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_read_data(uint32_t u32Addr, uint8_t *pu8Buffer, uint32_t u32Len)
{
    uint32_t u32ReadData = 0;
    uint32_t u32ReadLen = u32Len / 4;
    uint32_t i = 0;

    for (; i < u32ReadLen; i++)
    {
        u32ReadData = *(volatile uint32_t *)(u32Addr + i * 4);
        rom_utility_little_endian_store_32(pu8Buffer, i * 4, u32ReadData);
    }
    u32ReadLen = u32Len - u32ReadLen * 4;
    if (u32ReadLen)
    {
        u32ReadData = *(volatile uint32_t *)(u32Addr + i * 4);
        for (i = 0; i < u32ReadLen; i++)
        {
            pu8Buffer[u32Len - u32ReadLen + i] = u32ReadData >> (i * 8);
        }
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_read_data_u32(uint32_t u32Addr, uint32_t *pu32Buffer, uint32_t u32Len)
{
    uint32_t i = 0;

    for (; i < u32Len; i++)
    {
        pu32Buffer[i] = *(volatile uint32_t *)(u32Addr + i * 4);
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief Sends low-power commands to the flash.
 *
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_deep_power_down(void)
{
    uint32_t u32Cmd = FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_DEEP_POWER_DOWN);

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if(EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    rom_utility_delay_us(GT_FLASH_TIMING_DEEP_POWER_DOWN_US * 2);
    return EN_ERROR_STA_OK;
}

/**
 * @brief The command to disable low power consumption is sent to the flash.
 *
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_release_deep_power_down(void)
{
    uint32_t u32Cmd = FLASH_CTRL_CMD_NOADDR_NODATA_INIT(GT_FLASH_CMD_RELEASE_DEEP_POWER_DOWN);
    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if(EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    rom_utility_delay_us(GT_FLASH_TIMING_STANDBY_WITHOUT_ID_READ_US * 2);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_write_status_reg1(uint16_t u16Sta)
{
    uint8_t u8Ret = rom_hw_flash_write_enable_cmd();
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    uint8_t u8Opcode = GT_FLASH_CMD_WRITE_STATUS1;

    uint32_t u32Cmd = FLASH_CTRL_FLASH_CMD_WRITE_NO_ADDR_INIT(GT_FLASH_CMD_WRITE_STATUS1, 1, 0);

    FLASH_CTRL->FLASH_WRITE_DATA_LOW = u16Sta;

    u8Ret |= rom_hw_flash_ctrl_send_cmd(u32Cmd);

    rom_utility_delay_us(PY_FLASH_TIMER_PROGRAM_TYPICAL_US);

    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG_T enReg, uint8_t u8Sta)
{
    uint8_t u8Ret = rom_hw_flash_write_enable_cmd();
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    uint8_t u8Opcode = GT_FLASH_CMD_WRITE_STATUS1;

    switch (enReg)
    {
    case EN_FLASH_READ_STA_REG2:
    {
        u8Opcode = GT_FLASH_CMD_WRITE_STATUS2;
    }
    break;

    case EN_FLASH_READ_STA_REG3:
    {
        u8Opcode = GT_FLASH_CMD_WRITE_STATUS3;
    }
    break;

    default:
        break;
    }

    uint32_t u32Cmd = FLASH_CTRL_WRITE_FLASH_CTRL_REG_CMD(u8Opcode);

    FLASH_CTRL->FLASH_WRITE_DATA_LOW = u8Sta;

    u8Ret |= rom_hw_flash_ctrl_send_cmd(u32Cmd);

    rom_utility_delay_us(PY_FLASH_TIMING_WRITE_STATUS_REG_US);

    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief Read the flash status register 1~3.
 *
 * @param enReg @ref EN_FLASH_READ_STA_REG_T.
 * @param pu8Sta Pointer to buffer.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG_T enReg, uint8_t *pu8Sta)
{
    if (NULL == pu8Sta)
    {
        return EN_ERROR_STA_INVALID;
    }

    uint8_t u8Opcode = GT_FLASH_CMD_READ_STATUS1;

    if (1)
    {
        u8Opcode = GT_FLASH_CMD_READ_STATUS1;
        switch (enReg)
        {
        case EN_FLASH_READ_STA_REG2:
        {
            u8Opcode = GT_FLASH_CMD_READ_STATUS2;
        }
        break;

        case EN_FLASH_READ_STA_REG3:
        {
            u8Opcode = GT_FLASH_CMD_READ_STATUS3;
        }
        break;

        default:
            break;
        }
    }
    else
    {
        u8Opcode = PY_FLASH_CMD_RDSR;
        if (enReg != EN_FLASH_READ_STA_REG1)
        {
            u8Opcode = PY_FLASH_CMD_RDSR1;
        }
    }

    uint32_t u32Cmd = FLASH_CTRL_FLASH_CMD_READ_NO_ADDR_INIT(u8Opcode, 0, 0);

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    rom_hw_flash_ctrl_get_read_data(pu8Sta, 1);

    return EN_ERROR_STA_OK;
}

/**
 * @brief Enable/disable flash qspi mode.
 *
 * @param bEnable true: enable, false: disable.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_enable_qspi(bool bEnable)
{
    uint8_t u8Sta = 0;
    uint16_t u16Flag = 0;
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &u8Sta);
    u16Flag = (u8Sta << 8);

    if (bEnable)
    {
        if (u8Sta & EN_FLASH_STA2_QE)
        {
            return EN_ERROR_STA_OK;
        }
    }
    else
    {
        if (0 == (u8Sta & EN_FLASH_STA2_QE))
        {
            return EN_ERROR_STA_OK;
        }
    }

    uint8_t u8Ret = EN_ERROR_STA_OK;

    if (bEnable)
    {
        u16Flag = 0x200;
    }
    else
    {
        u16Flag = 0;
    }
    u8Ret |= rom_hw_flash_write_status_reg1(u16Flag);

    rom_utility_delay_us(PY_FLASH_TIMER_PROGRAM_TYPICAL_US);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_puya_release_read_enhanced(void)
{
    uint32_t u32Cmd = FLASH_CTRL_RELEASE_READ_ENHANCED;
    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    rom_utility_delay_us(GT_FLASH_TIMING_STANDBY_WITHOUT_ID_READ_US);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_exit_abnormal_state(void)
{
    // 1. Release deep power down
    uint8_t u8Ret = rom_hw_flash_release_deep_power_down();
    RETURN_IF_ERR(u8Ret);

    // 2. puya flash exit read enhanced
    u8Ret = rom_hw_flash_puya_release_read_enhanced();
    RETURN_IF_ERR(u8Ret);

    // 3. Enable flash reset
    u8Ret = rom_hw_flash_ctrl_send_cmd(FLASH_CTRL_ENABLE_FLASH_RESET);
    RETURN_IF_ERR(u8Ret);

    u8Ret = rom_hw_flash_ctrl_send_cmd(FLASH_CTRL_FLASH_RESET);
    RETURN_IF_ERR(u8Ret);

    rom_utility_delay_us(PY_FLASH_TIMING_RESET_US);
    return u8Ret;
}

/**
 * @brief flash initialization, flash frequency division is not performed by default,
 *        and dac mode is enabled.
 * @return EN_ERR_STA_T
 */
EN_ERR_STA_T rom_hw_flash_init(void)
{
    unFlashCrtlInit_t unFlashCtrlCfg;
    unFlashCtrlCfg.u32Cfg = FLASH_CTRL_INIT_DEFAULT(0);
    rom_hw_flash_ctrl_init(unFlashCtrlCfg);

    rom_hw_flash_exit_abnormal_state();

    rom_hw_flash_enable_qspi(true);

    unFlashReadInst_t unFlashReadInst;
    unFlashReadInst.u32Cfg = FLASH_CTRL_READ_INST_QSPI_INIT2(GT_FLASH_CMD_QUAD_OUTPUT_READ);
    rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);

    unFlashWriteInst_t unFlashWriteInst;
    unFlashWriteInst.u32Cfg = FLASH_CTRL_WRITE_INST_QSPI_INIT(GT_FLASH_CMD_QUAD_PAGE_WRITE);
    rom_hw_flash_ctrl_set_write_instruction(unFlashWriteInst);

    rom_hw_flash_ctrl_set_mode_bits(0);

    unFlashQspiDelay_t unDelay;
    unDelay.u32Cfg = 0x01010101;
    rom_hw_flash_ctrl_set_qspi_delay(unDelay);

    rom_hw_flash_ctrl_set_read_data_capture(1, 1);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_read_uid(uint8_t *pu8Buffer, uint8_t u8Len)
{
    uint32_t u32Cmd = FLASH_CTRL_FLASH_CMD_READ_UID;

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    rom_hw_flash_ctrl_get_read_data(pu8Buffer, u8Len);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_read_dev_id(uint8_t *pu8Buffer, uint8_t u8Len)
{
    uint32_t u32Cmd = FLASH_CTRL_FLASH_CMD_READ_NO_ADDR_INIT(GT_FLASH_CMD_READ_ID, 3, 0);

    uint8_t u8Ret = rom_hw_flash_ctrl_send_cmd(u32Cmd);
    if (EN_ERROR_STA_OK != u8Ret)
    {
        return u8Ret;
    }

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    rom_hw_flash_ctrl_get_read_data(pu8Buffer, u8Len);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM_T enType, uint16_t u16Offset, uint8_t *pu8Buffer, uint16_t u16Len)
{
    if ((NULL == pu8Buffer) | ((u16Offset + u16Len) > 512))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32Addr = ((enType & 0x3) << 8) | u16Offset;

    if (gu8FlashType == EN_FLASH_TYPE_PY)
    {
        if (enType == EN_FLASH_SEC_MEM3) {
            return EN_ERROR_STA_INVALID;
        } else {
            enType += 1;
        }
        u32Addr = ((enType & 0x3) << 12) | u16Offset;
    }

    uint32_t u32Cmd = 0;
    uint8_t u8Idx = 0;
    uint8_t u8ReadLen = 0;

    while (u16Len)
    {
        if (u16Len <= 8)
        {
            u8ReadLen = (u16Len) & 0xFF;
        }
        else
        {
            u8ReadLen = 8;
        }

        u32Cmd = FLASH_CTRL_READ_SECURITY_REG_CMD((u8ReadLen - 1));
        rom_hw_flash_ctrl_set_cmd_addr(u32Addr);
        rom_hw_flash_ctrl_send_cmd(u32Cmd);
        if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
        {
            return EN_ERROR_STA_TIMEOUT;
        }
        rom_utility_delay_ms(1);
        rom_hw_flash_ctrl_get_read_data(&pu8Buffer[u8Idx], u8ReadLen);
        u32Addr += u8ReadLen;
        u8Idx += u8ReadLen;
        u16Len -= u8ReadLen;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM_T enType, uint16_t u16Offset, uint8_t *pu8Buffer, uint16_t u16Len)
{
    if ((NULL == pu8Buffer) | ((u16Offset + u16Len) > 256))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32Addr = ((enType & 0x3) << 8) | u16Offset;
    uint32_t u32Cmd = 0;
    uint8_t u8Idx = 0;
    uint8_t u8ReadLen = 0;

    if (gu8FlashType == EN_FLASH_TYPE_PY)
    {
        u32Addr = ((enType & 0x3) << 12) | u16Offset;
    }

    while (u16Len)
    {
        if (u16Len < 8)
        {
            u8ReadLen = (u16Len) & 0xFF;
        }
        else
        {
            u8ReadLen = 8;
        }

        u32Cmd = FLASH_CTRL_WRITE_SECURITY_REG_CMD(u8ReadLen - 1);
        rom_hw_flash_write_enable_cmd();
        rom_hw_flash_ctrl_set_cmd_addr(u32Addr);
        rom_hw_flash_ctrl_set_write_data(&pu8Buffer[u8Idx], u8ReadLen);
        rom_hw_flash_ctrl_send_cmd(u32Cmd);
        if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
        {
            return EN_ERROR_STA_TIMEOUT;
        }
        rom_utility_delay_ms(3);
        u32Addr += u8ReadLen;
        u8Idx += u8ReadLen;
        u16Len -= u8ReadLen;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM_T enType, uint32_t u32Key)
{
    if (u32Key != FLASH_ERASE_SECURITY_MEM_KEY)
    {
        return EN_ERROR_STA_INVALID;
    }

    uint32_t u32Addr;

    if (enType <= EN_FLASH_SEC_MEM1)
    {
        u32Addr = 0;
    }
    else
    {
        u32Addr = 0x1 << 9;
    }

    if (gu8FlashType == EN_FLASH_TYPE_PY)
    {
        u32Addr = ((enType & 0x3) << 12);
    }

    uint32_t u32Cmd = FLASH_CTRL_ERASE_SECURITY_REG_CMD;
    rom_hw_flash_write_enable_cmd();
    rom_hw_flash_ctrl_set_cmd_addr(u32Addr);
    rom_hw_flash_ctrl_send_cmd(u32Cmd);

    if (!rom_hw_flash_ctrl_wait_flash_cmd_done())
    {
        return EN_ERROR_STA_TIMEOUT;
    }

    rom_utility_delay_us(GT_FLASH_TIMING_SECTOR_ERASE_US);
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_T enMode)
{
    unFlashReadInst_t unFlashReadInst;
    uint8_t u8Ret = EN_ERROR_STA_OK;

    switch (enMode)
    {
    case EN_FLASH_READ_MODE_READ:
    {
        // 03H
        u8Ret |= rom_hw_flash_enable_qspi(false);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_BY_SPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
    }
    break;

    case EN_FLASH_READ_MODE_FAST_READ:
    {
        // 03H
        u8Ret |= rom_hw_flash_enable_qspi(false);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_BY_FAST_SPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
    }
    break;

    case EN_FLASH_READ_MODE_DUAL_READ:
    {
        // 3BH
        u8Ret |= rom_hw_flash_enable_qspi(false);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_BY_DPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
    }
    break;

    case EN_FLASH_READ_MODE_QUAD_READ:
    {
        // 6BH
        u8Ret |= rom_hw_flash_enable_qspi(true);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_INST_QSPI_INIT2(GT_FLASH_CMD_QUAD_OUTPUT_READ);
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
    }
    break;

    case EN_FLASH_READ_MODE_DUAL_FAST_READ:
    {

        u8Ret |= rom_hw_flash_enable_qspi(false);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_BY_FAST_DPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
        u8Ret |= rom_hw_flash_ctrl_set_mode_bits(0x0); // No continue
    }
    break;

    case EN_FLASH_READ_MODE_QUAD_FAST_READ:
    {

        u8Ret |= rom_hw_flash_enable_qspi(true);
        unFlashReadInst.u32Cfg = FLASH_CTRL_READ_BY_FAST_QPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_read_instruction(unFlashReadInst);
        u8Ret |= rom_hw_flash_ctrl_set_mode_bits(0x0); // No continue
    }
    break;

    default:
        break;
    }
    return u8Ret;
}

EN_ERR_STA_T rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_T enMode)
{
    unFlashWriteInst_t unFlashWriteInst;

    uint8_t u8Ret = EN_ERROR_STA_OK;

    switch (enMode)
    {
    case EN_FLASH_WRITE_MODE_PP:
    {
        //            u8Ret |=rom_hw_flash_enable_qspi(false);
        unFlashWriteInst.u32Cfg = FLASH_CTRL_WRITE_BY_SPI_CFG;
        u8Ret |= rom_hw_flash_ctrl_set_write_instruction(unFlashWriteInst);
    }
    break;

    case EN_FLASH_WRITE_MODE_QPP:
    {
        //            u8Ret |=rom_hw_flash_enable_qspi(true);
        unFlashWriteInst.u32Cfg = FLASH_CTRL_WRITE_INST_QSPI_INIT(GT_FLASH_CMD_QUAD_PAGE_WRITE);
        u8Ret |= rom_hw_flash_ctrl_set_write_instruction(unFlashWriteInst);
    }
    break;

    default:
        break;
    }
    return u8Ret;
}

EN_ERR_STA_T rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_T enMode)
{
    uint8_t u8Ret = EN_ERROR_STA_OK;

    switch (enMode)
    {
    case EN_FLASH_RW_MODE_SPI:
    {
        rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_READ);

        rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_PP);
    }
    break;

    case EN_FLASH_RW_MODE_DPI:
    {
        rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_DUAL_READ);

        rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_PP);

        break;
    }

    case EN_FLASH_RW_MODE_QSPI:
    {
        rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_QUAD_READ);

        rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_QPP);

        break;
    }

    case EN_FLASH_RW_MODE_FQSPI:
    {
        rom_hw_flash_set_read_mode(EN_FLASH_READ_MODE_QUAD_FAST_READ);

        rom_hw_flash_set_write_mode(EN_FLASH_WRITE_MODE_QPP);

        break;
    }

    default:
        break;
    }

    return u8Ret;
}
