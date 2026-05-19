/**
 * ****************************************************************************
 * @file slc_hal_aes.h
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

#ifndef __SLC_HAL_AES_H__
#define __SLC_HAL_AES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define HAL_AES128_BLOCK_SIZE   16

typedef enum
{
    HAL_AES128_CBC,

    HAL_AES_MAX,
} hal_aes_mode_e;

typedef enum
{
    HAL_AES_ENC,
    HAL_AES_DEC,
} hal_aes_enc_dec_e;

/**
 * @brief AES算法初始化，设置AES算法类型、密钥和初始化向量，准备进行AES算法操作
 * 
 * @param mode AES算法类型，参考 hal_aes_mode_e 枚举，目前仅支持CBC模式
 * @param enc_dec AES算法操作类型，参考 hal_aes_enc_dec_e 枚举，指定是加密还是解密
 * @param key  AES算法密钥，长度必须为16字节（128位）
 * @param iv   AES算法初始化向量，长度必须为16字节（128位）
 * 
 * @note 与slc_hal_aes_update和slc_hal_aes_final配合使用，用于分包运算
 */
void slc_hal_aes_init(hal_aes_mode_e mode, hal_aes_enc_dec_e enc_dec, uint8_t *key, uint8_t *iv);

/**
 * @brief AES算法去初始化，结束AES算法操作，释放资源
 * 
 * @param mode AES算法类型，参考 hal_aes_mode_e 枚举，目前仅支持CBC模式
 * @note 与slc_hal_aes_update和slc_hal_aes_init配合使用，用于分包运算
 */
void slc_hal_aes_final(hal_aes_mode_e mode);

/**
 * @brief AES算法加密/解密
 * 
 * @param in 输入数据，长度必须为16字节（128位）的整数倍
 * @param inlen 输入数据的长度，单位为字节，必须为16字节（128位）的整数倍
 * @param out 输出数据的指针，长度与输入数据相同，单位为字节，为16字节（128位）的整数倍
 * @return int 0表示成功，非0表示失败
 * 
 * @note 与slc_hal_aes_init和slc_hal_aes_final配合使用，用于分包运算
 */
int slc_hal_aes_update(uint8_t *in, uint32_t inlen, uint8_t *out);

/**
 * @brief AES算法加密，单次调用，无需调用slc_hal_aes_init
 * 
 * @param key   AES算法密钥，长度必须为16字节（128位）
 * @param iv    AES算法初始化向量，长度必须为16字节（128位）
 * @param plain AES算法输入数据，长度必须为16字节（128位）的整数倍
 * @param len   AES算法输入数据的长度，单位为字节，必须为16字节（128位）的整数倍
 * @param enc_data AES算法输出数据的指针，长度与输入数据相同，单位为字节，为16字节（128位）的整数倍
 * @return int 0表示成功，非0表示失败
 */
int slc_hal_aes128_enc_once(uint8_t *key, uint8_t *iv, uint8_t *plain, uint32_t len, uint8_t *enc_data);

/**
 * @brief AES算法解密，单次调用，无需调用slc_hal_aes_init
 * 
 * @param key   AES算法密钥，长度必须为16字节（128位）
 * @param iv    AES算法初始化向量，长度必须为16字节（128位）
 * @param enc_data AES算法输出数据的指针，长度与输入数据相同，单位为字节，为16字节（128位）的整数倍
 * @param len   AES算法输入数据的长度，单位为字节，必须为16字节（128位）的整数倍
 * @param plain AES算法输入数据，长度必须为16字节（128位）的整数倍
 * @return int 0表示成功，非0表示失败
 */
int slc_hal_aes128_dec_once(uint8_t *key, uint8_t *iv, uint8_t *enc_data, uint32_t len, uint8_t *plain);

#ifdef __cplusplus
}
#endif

#endif  /* __SLC_HAL_AES_H__ */
