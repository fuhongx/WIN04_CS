/**
 * ****************************************************************************
 * @file hw_crc.h
 * @author SLC(SC) software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SILICONDUCTOR IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2025 Siliconductor Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __HW_CRC_H__
#define __HW_CRC_H__

typedef enum
{
    EN_CRC_OUTPUT_NORMAL        = 0x0,  // 按照正常顺序 [15:0] / [23:0] / [31:0] 进行输出
    EN_CRC_OUTPUT_REVERSE       = 0x1,  // 按照反转顺序 [0:15] / [0:23] / [0:31] 进行输出
    EN_CRC_OUTPUT_BYTE_REVERSE  = 0x2,  // 按照字节反转顺序 [7:0] , [15:8] , [23:16] , [31:24] 进行输出
    EN_CRC_OUTPUT_BIT_REVERSE   = 0x3,  // 按照位反转顺序 [24:31] , [16:23] , [8:15] , [0:7] 进行输出
} EN_CRC_OUTPUT_T;

typedef struct
{
    bool bDmaEn;
    bool bInputReverse; // 输入数据是否反转, true: 反转[0:15] / [0:23] / [0:31], false: 不反转[15:0] / [23:0] / [31:0]
    bool bOutputXor;
    EN_CRC_OUTPUT_T enOutputMode;
} stCrcInit_t;

EN_ERR_STA_T rom_hw_crc_init(stCrcHandle_t *pstHandle, stCrcInit_t *pstInit);
EN_ERR_STA_T rom_hw_crc_set_init_value(stCrcHandle_t *pstHandle, uint32_t u32Value);
EN_ERR_STA_T rom_hw_crc_set_xor_bytes(stCrcHandle_t *pstHandle, uint32_t u32Xor);
uint32_t rom_hw_crc_calculate_data(stCrcHandle_t *pstHandle, uint8_t *pu8Data, uint16_t u16Len);

/**
 * @brief Get CRC16 value of buffer.
 *            CRC16-CCITT (POLY 0x1021), 0x0000 initial value, input data reverse, output xor 0x0000, output reverse
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC16 value.
 */
uint16_t rom_hw_crc_get_crc16_value(uint8_t *pu8Buffer, uint16_t u16Len);

/**
 * @brief Get CRC24 value of buffer.
 *            CRC24 (POLY 0x800063), 0xFFFFFF initial value, input data reverse, output xor 0xFFFFFF, output reverse
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC24 value.
 */
uint32_t rom_hw_crc_get_crc24_value(uint8_t *pu8Buffer, uint16_t u16Len);

/**
 * @brief Get CRC32 value of buffer.
 *            CRC32 (POLY 0x04C11DB7), 0xFFFFFFFF initial value, input data reverse, output xor 0xFFFFFFFF, output reverse
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC32 value.
 */
uint32_t rom_hw_crc_get_crc32_value(uint8_t *pu8Buffer, uint16_t u16Len);

#endif
