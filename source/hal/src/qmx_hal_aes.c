/**
 * ****************************************************************************
 * @file qmx_hal_aes.c
 * @author QMX software team.
 * @brief
 * @version  1.0
 * ****************************************************************************
 *
 * THIS CODE IS LIMITED TO GUIDE THE APPLICATION AND IMPROVE THE EFFICIENCY OF RESEARCH AND DEVELOPMENT.
 * AS A RESULT, QMX IS NOT RESPONSIBLE FOR ANY CLAIMS ARISING DIRECTLY OR INDIRECTLY FROM SUCH FIRMWARE AND/OR CONTENT.
 *
 * Copyright (c) 2026 QMX Semiconductor all rights reserved.
 *
 * ****************************************************************************
 */
#include "qmx_hal_aes.h"
#include "qmx_hal_sysctrl.h"
#include "reg_Dev.h"
#include "utility.h"
#include "error_def.h"
#include "hw_aes.h"

void qmx_hal_aes_init(hal_aes_mode_e mode, hal_aes_enc_dec_e enc_dec, uint8_t *key, uint8_t *iv)
{
    EN_AES128_MODE_T aes_mode = (enc_dec == HAL_AES_ENC) ? EN_AES128_MODE_ENCRYPT : EN_AES128_MODE_DECRYPT;
    rom_hw_aes128_init(aes_mode, key, iv);
}

void qmx_hal_aes_final(hal_aes_mode_e mode)
{
    qmx_hal_sysctrl_peripheral_mod_reset(HAL_CLK_AES128);
    qmx_hal_sysctrl_peripheral_clk_enable(HAL_CLK_AES128, false);
}

int qmx_hal_aes_update(uint8_t *in, uint32_t inlen, uint8_t *out)
{
    if (in == NULL || out == NULL || (inlen == 0) || (inlen % AES128_BLOCK_SIZE != 0)) {
        return -1;
    }

    EN_ERR_STA_T ret;

    ret = rom_hw_aes128_crypto(in, inlen, out);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

int qmx_hal_aes128_enc_once(uint8_t *key, uint8_t *iv, uint8_t *plain, uint32_t len, uint8_t *enc_data)
{
    EN_ERR_STA_T ret;
    ret = rom_hw_aes128_encrypt_once(key, iv, plain, len, enc_data);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}

int qmx_hal_aes128_dec_once(uint8_t *key, uint8_t *iv, uint8_t *enc_data, uint32_t len, uint8_t *plain)
{
    EN_ERR_STA_T ret;
    ret = rom_hw_aes128_decrypt_once(key, iv, enc_data, len, plain);
    return (ret == EN_ERROR_STA_OK) ? 0 : -1;
}