/**
 * ****************************************************************************
 * @file qmx_hal_crc.h
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __QMX_HAL_CRC_H__
#define __QMX_HAL_CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    HAL_CRC16,
    HAL_CRC24,
    HAL_CRC32,

    HAL_CRC_TYPE_MAX,
} hal_crc_type_e;

typedef enum
{
    HAL_CRC_OUTPUT_NORMAL        = 0x0,  // 按照正常顺序 [15:0] / [23:0] / [31:0] 进行输出
    HAL_CRC_OUTPUT_REVERSE       = 0x1,  // 按照反转顺序 [0:15] / [0:23] / [0:31] 进行输出
    HAL_CRC_OUTPUT_BYTE_REVERSE  = 0x2,  // 按照字节反转顺序 [7:0] , [15:8] , [23:16] , [31:24] 进行输出
    HAL_CRC_OUTPUT_BIT_REVERSE   = 0x3,  // 按照位反转顺序 [24:31] , [16:23] , [8:15] , [0:7] 进行输出
} hal_crc_out_e;

typedef struct
{
    uint32_t init_val;  /*  初始值 */
    uint32_t xor_val;   /*  输出异或值 */
    hal_crc_type_e crc_type;
    hal_crc_out_e output_mode;
    bool input_reverse; /*  输入数据是否反转：
                            true: 反转; 数据为 [0:15] / [0:23] / [0:31]
                            false: 不反转；数据为 [15:0] / [23:0] / [31:0] */
    bool output_xor;
} hal_crc_init_t;

/**
 * @brief CRC初始化
 * 
 * @param init CRC初始化参数，详见hal_crc_init_t结构体
 */
void qmx_hal_crc_init(hal_crc_init_t *init);

/**
 * @brief CRC去初始化
 * 
 * @param crc_type CRC类型，详见hal_crc_type_e枚举
 */
void qmx_hal_crc_deinit(hal_crc_type_e crc_type);

/**
 * @brief CRC计算
 * 
 * @param crc_type CRC类型，详见hal_crc_type_e枚举
 * @param data CRC计算的数据指针
 * @param len 数据长度
 * @return uint32_t 返回CRC计算结果，CRC16返回16位结果，CRC24返回24位结果，CRC32返回32位结果
 */
uint32_t qmx_hal_crc_calculate(hal_crc_type_e crc_type, uint8_t *data, uint32_t len);

/**
 * @brief Get CRC16 value of buffer.)
 *        CRC16-CCITT (POLY 0x1021), 0x0000 initial value, input data reverse, output xor 0x0000, output reverse
 * @note  该接口仅供快速计算CRC16使用，无需调用qmx_hal_crc_init，其他CRC类型请使用qmx_hal_crc_calculate接口
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC16 value.
 */
uint16_t qmx_hal_crc_calc16(uint8_t *data, uint32_t len);

/**
 * @brief Get CRC24 value of buffer.
 *        CRC24 (POLY 0x800063), 0xFFFFFF initial value, input data reverse, output xor 0xFFFFFF, output reverse
 * @note  该接口仅供快速计算CRC24使用，无需调用qmx_hal_crc_init，其他CRC类型请使用qmx_hal_crc_calculate接口
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC24 value.
 */
uint32_t qmx_hal_crc_calc24(uint8_t *data, uint32_t len);

/**
 * @brief Get CRC32 value of buffer.
 *        CRC32 (POLY 0x04C11DB7), 0xFFFFFFFF initial value, input data reverse, output xor 0xFFFFFFFF, output reverse
 * @note  该接口仅供快速计算CRC32使用，无需调用qmx_hal_crc_init，其他CRC类型请使用qmx_hal_crc_calculate接口
 * @param pu8Buffer Pointer to buffer.
 * @param u16Len Length of buffer.
 * @return uint32_t CRC32 value.
 */
uint32_t qmx_hal_crc_calc32(uint8_t *data, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __QMX_HAL_CRC_H__ */
