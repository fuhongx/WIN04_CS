/**
 * ****************************************************************************
 * @file slc_hal_rng.h
 * @author SLC software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE APPLICATION DEVELOPMENT AND INCREASE DEVELOPMENT SPEED.
 * AS A RESULT, SLC IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 SLC Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */

#ifndef __SLC_HAL_RNG_H__
#define __SLC_HAL_RNG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint32_t seed;  /* seed for random number generator */
} hal_rng_init_t;

/**
 * @brief 初始化RNG模块，配置RNG的种子值
 * 
 * @param init 初始化参数，具体见hal_rng_init_t结构体
 */
void slc_hal_rng_init(hal_rng_init_t *init);

/**
 * @brief 关闭RNG模块，停止随机数生成
 * 
 */
void slc_hal_rng_deinit(void);

/**
 * @brief 更新RNG模块的种子值
 * 
 * @param seed rng种子值
 */
void slc_hal_update_rng_seed(uint32_t seed);

/**
 * @brief 获取u32格式指定长度的随机数
 * 
 * @param data u32格式的随机数数据缓冲区指针
 * @param len  随机数长度
 * @param timeout 获取随机数的超时时间，单位为微秒
 * @return int 获取随机数结果，0表示成功，-1表示失败
 */
int slc_hal_get_random_u32(uint32_t *data, uint32_t len, uint32_t timeout);

/**
 * @brief 获取u8格式指定长度的随机数
 * 
 * @param data u8格式的随机数数据缓冲区指针
 * @param len  随机数长度
 * @param timeout 获取随机数的超时时间，单位为微秒
 * @return int 获取随机数结果，0表示成功，-1表示失败
 */
int slc_hal_get_random_u8(uint8_t *data, uint32_t len, uint32_t timeout);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_RNG_H__ */
