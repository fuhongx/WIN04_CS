/**
 * ****************************************************************************
 * @file hw_aes.h
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

#ifndef __HW_AES_H__
#define __HW_AES_H__

#include "reg_Dev.h"
#include "error_def.h"

#define AES128_KEY_SIZE     16
#define AES128_IV_SIZE      16
#define AES128_BLOCK_SIZE   16

#define AES128_WAIT_TIMEOUT (0x500000)

typedef enum
{
    EN_AES128_KEY32  = 0,
    EN_AES128_KEY64  = 1,
    EN_AES128_KEY96  = 2,
    EN_AES128_KEY128 = 3,
} EN_AES128_KEY_MODE_T;

typedef enum
{
    EN_AES128_IV32  = 0,
    EN_AES128_IV64  = 1,
    EN_AES128_IV96  = 2,
    EN_AES128_IV128 = 3,
} EN_AES128_IV_MODE_T;

typedef enum
{
    EN_AES128_INT_BLOCK_DONE = AES_INT_IMR_BLOCK_DONE_INT_MASK << AES_INT_IMR_BLOCK_DONE_INT_SHIFT,
    EN_AES128_INT_BYTES_DONE = AES_INT_IMR_BYTES_DONE_INT_MASK << AES_INT_IMR_BYTES_DONE_INT_SHIFT,
    EN_AES128_INT_ROUND_DONE = AES_INT_IMR_ROUND_DONE_INT_MASK << AES_INT_IMR_ROUND_DONE_INT_SHIFT,
} EN_AES128_INT_T;

typedef enum
{
    EN_AES128_MODE_DECRYPT = AES128_MODE_DECRYPT,
    EN_AES128_MODE_ENCRYPT = AES128_MODE_ENCRYPT,
} EN_AES128_MODE_T;

void rom_hw_aes128_init(EN_AES128_MODE_T mode, uint8_t *key, uint8_t *iv);
EN_ERR_STA_T rom_hw_aes128_crypto(uint8_t *in, uint32_t inlen, uint8_t *out);
EN_ERR_STA_T rom_hw_aes128_encrypt_once(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t len, uint8_t *enc_data);
EN_ERR_STA_T rom_hw_aes128_decrypt_once(uint8_t *key, uint8_t *iv, uint8_t *enc_data, uint32_t len, uint8_t *dec_data);

#endif
