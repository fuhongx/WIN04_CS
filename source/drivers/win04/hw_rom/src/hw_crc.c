/**
 * ****************************************************************************
 * @file hw_crc.c
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
#include "hw_crc.h"

#define CRC_CALC_TIMEOUT     (0xFFFF)

EN_ERR_STA_T rom_hw_crc_init(stCrcHandle_t *pstHandle, stCrcInit_t *pstInit)
{
    pstInit->bInputReverse = pstInit->bInputReverse ? false : true;
    pstHandle->CRC_SCR = ((pstInit->bDmaEn & CRC_SCR_DMA_EN_MASK) << CRC_SCR_DMA_EN_SHIFT) |
                         ((pstInit->bInputReverse & CRC_SCR_INPUT_DATA_REVERSE_EN_MASK) << CRC_SCR_INPUT_DATA_REVERSE_EN_SHIFT) |
                         ((pstInit->bOutputXor & CRC_SCR_OUTPUT_XOR_EN_MASK) << CRC_SCR_OUTPUT_XOR_EN_SHIFT) |
                         ((pstInit->enOutputMode & CRC_SCR_OUTPUT_SEQ_MASK) << CRC_SCR_OUTPUT_SEQ_SHIFT);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_crc_set_init_value(stCrcHandle_t *pstHandle, uint32_t u32Value)
{
    pstHandle->CRC_DEFAULT = u32Value;
    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_crc_set_xor_bytes(stCrcHandle_t *pstHandle, uint32_t u32Xor)
{
    pstHandle->CRC_OLOGIC = u32Xor;
    return EN_ERROR_STA_OK;
}

uint32_t rom_hw_crc_calculate_data(stCrcHandle_t *pstHandle, uint8_t *pu8Data, uint16_t u16Len)
{
    uint16_t i = 0;
    uint16_t timeout = 0;

    if (u16Len == 0)
        return 0;

    pstHandle->CRC_LEN = u16Len;

    // Start Calculate
    pstHandle->CRC_SCR |= (CRS_SCR_CRC_EN_MASK << CRC_SCR_CRC_EN_SHIFT);

    for (i = 0; i < u16Len; i++)
    {
        pstHandle->CRC_DATA = pu8Data[i];
    }

    do {
        if (pstHandle->CRC_ISR == 0x1)
            break;

        rom_utility_delay_cycles(1);
    } while (timeout++ < CRC_CALC_TIMEOUT);

    pstHandle->CRC_ISR = 0x1; // Clear interrupt flag

    // Stop calculate
    pstHandle->CRC_SCR &= ~(CRS_SCR_CRC_EN_MASK << CRC_SCR_CRC_EN_SHIFT);

    if (timeout >= CRC_CALC_TIMEOUT)
        return 0;

    return pstHandle->CRC_CRR;
}

uint16_t rom_hw_crc_get_crc16_value(uint8_t *pu8Buffer, uint16_t u16Len)
{
    stCrcInit_t stInit;

    stInit.bDmaEn = false;
    stInit.bInputReverse = true;
    stInit.bOutputXor = true;
    stInit.enOutputMode = EN_CRC_OUTPUT_REVERSE;

    rom_hw_crc_init(CRC16, &stInit);

    rom_hw_crc_set_init_value(CRC16, 0x0);
    rom_hw_crc_set_xor_bytes(CRC16, 0x0);

    return (rom_hw_crc_calculate_data(CRC16, pu8Buffer, u16Len) & 0xFFFF);
}

uint32_t rom_hw_crc_get_crc24_value(uint8_t *pu8Buffer, uint16_t u16Len)
{
    stCrcInit_t stInit;

    stInit.bDmaEn = false;
    stInit.bInputReverse = true;
    stInit.bOutputXor = true;
    stInit.enOutputMode = EN_CRC_OUTPUT_REVERSE;

    rom_hw_crc_init(CRC24, &stInit);

    rom_hw_crc_set_init_value(CRC24, 0xFFFFFF);
    rom_hw_crc_set_xor_bytes(CRC24, 0xFFFFFF);

    return (rom_hw_crc_calculate_data(CRC24, pu8Buffer, u16Len) & 0xFFFFFF);
}

uint32_t rom_hw_crc_get_crc32_value(uint8_t *pu8Buffer, uint16_t u16Len)
{
    stCrcInit_t stInit;

    stInit.bDmaEn = false;
    stInit.bInputReverse =true;
    stInit.bOutputXor = true;
    stInit.enOutputMode = EN_CRC_OUTPUT_REVERSE;

    rom_hw_crc_init(CRC32, &stInit);

    rom_hw_crc_set_init_value(CRC32, 0xFFFFFFFF);
    rom_hw_crc_set_xor_bytes(CRC32, 0xFFFFFFFF);

    return rom_hw_crc_calculate_data(CRC32, pu8Buffer, u16Len);
}
