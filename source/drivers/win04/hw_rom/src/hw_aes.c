/**
 * ****************************************************************************
 * @file hw_aes.c
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

#include "utility.h"
#include "hw_sysctrl.h"
#include "hw_aes.h"

static uint32_t rom_hw_data_to_be32(const uint8_t *data)
{
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8)  |
           ((uint32_t)data[3]);
}

static void rom_hw_data_to_be32_to_le(uint32_t data, uint8_t *out)
{
    out[0] = (uint8_t)(data >> 24);
    out[1] = (uint8_t)(data >> 16);
    out[2] = (uint8_t)(data >> 8);
    out[3] = (uint8_t)(data);
}

EN_ERR_STA_T rom_hw_aes128_set_key(uint8_t *key, EN_AES128_KEY_MODE_T enMode)
{
    if ((NULL == key) | (EN_AES128_KEY128 < enMode))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint8_t i = 0;
    uint32_t temp[AES128_BLOCK_SIZE/4] = {0};

    for (i = 0; i < AES128_KEY_SIZE; i += 4) {
        temp[AES128_BLOCK_SIZE/4 - 1 - i/4] = rom_hw_data_to_be32(key + i);
    }

    // Clear Keys
    AES128->AES_KEY0 = temp[0];
    AES128->AES_KEY1 = 0;
    AES128->AES_KEY2 = 0;
    AES128->AES_KEY3 = 0;

    switch (enMode)
    {
        case EN_AES128_KEY32:
        {
            break;
        }

        case EN_AES128_KEY64:
        {
            AES128->AES_KEY1 = temp[1];
            break;
        }

        case EN_AES128_KEY96:
        {
            AES128->AES_KEY1 = temp[1];
            AES128->AES_KEY2 = temp[2];
            break;
        }

        case EN_AES128_KEY128:
        {
            AES128->AES_KEY1 = temp[1];
            AES128->AES_KEY2 = temp[2];
            AES128->AES_KEY3 = temp[3];
            break;
        }

        default:
            break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_set_vector(uint8_t *iv, EN_AES128_IV_MODE_T enMode)
{
    if ((NULL == iv) | (EN_AES128_IV128 < enMode))
    {
        return EN_ERROR_STA_INVALID;
    }

    uint8_t i = 0;
    uint32_t temp[AES128_BLOCK_SIZE/4] = {0};

    for (i = 0; i < AES128_IV_SIZE; i += 4) {
        temp[AES128_BLOCK_SIZE/4 - 1 - i/4] = rom_hw_data_to_be32(iv + i);
    }

    // Clear IV
    AES128->AES_INI0 = temp[0];
    AES128->AES_INI1 = 0;
    AES128->AES_INI2 = 0;
    AES128->AES_INI3 = 0;

    switch (enMode)
    {
        case EN_AES128_IV32:
        {
            break;
        }

        case EN_AES128_IV64:
        {
            AES128->AES_INI1 = temp[1];
            break;
        }

        case EN_AES128_IV96:
        {
            AES128->AES_INI1 = temp[1];
            AES128->AES_INI2 = temp[2];
            break;
        }

        case EN_AES128_IV128:
        {
            AES128->AES_INI1 = temp[1];
            AES128->AES_INI2 = temp[2];
            AES128->AES_INI3 = temp[3];
            break;
        }

        default:
            break;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_set_data(uint32_t *pu32Data)
{
    if (NULL == pu32Data)
    {
        return EN_ERROR_STA_INVALID;
    }

    AES128->AES_DAT0 = pu32Data[0];
    AES128->AES_DAT1 = pu32Data[1];
    AES128->AES_DAT2 = pu32Data[2];
    AES128->AES_DAT3 = pu32Data[3];

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_get_data(uint32_t *pu32Buffer)
{
    if ((NULL == pu32Buffer))
    {
        return EN_ERROR_STA_INVALID;
    }

    pu32Buffer[0] = AES128->AES_OUT0;
    pu32Buffer[1] = AES128->AES_OUT1;
    pu32Buffer[2] = AES128->AES_OUT2;
    pu32Buffer[3] = AES128->AES_OUT3;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_set_interrupt_mask(uint8_t u8Mask, bool bEnable)
{
    if (bEnable)
    {
        AES128->AES_INT_IMR = u8Mask & AES_INT_IMR_REG_MASK;
    }
    else
    {
        AES128->AES_INT_IMR = ~(u8Mask)&AES_INT_IMR_REG_MASK;
    }

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_get_interrupt_mask(uint8_t *pu8Mask)
{
    *pu8Mask = AES128->AES_INT_IMR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_get_interrupt_flag(uint8_t *pu8Flag)
{
    if ((NULL == pu8Flag))
    {
        return EN_ERROR_STA_INVALID;
    }

    *pu8Flag = AES128->AES_INT_ISR;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_clear_interrupt_flag(uint8_t u8Flag)
{

    AES128->AES_INT_CLR = (u8Flag & AES_INT_CLR_REG_MASK);

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_enable_interrupt(uint8_t u8Flag)
{
    AES128->AES_INT_IER = u8Flag & AES_INT_IER_REG_MASK;

    return EN_ERROR_STA_OK;
}

static uint32_t g_aes_total_len = 0;

void rom_hw_aes128_init(EN_AES128_MODE_T mode, uint8_t *key, uint8_t *iv)
{
    if (key == NULL || iv == NULL) {
        return;
    }

    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_AES128);
    g_aes_total_len = 64;

    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE | EN_AES128_INT_BYTES_DONE | EN_AES128_INT_ROUND_DONE);

    rom_hw_aes128_set_key(key, EN_AES128_KEY128);
    rom_hw_aes128_set_vector(iv, EN_AES128_IV128);

    AES128->AES128_MODE = mode & AES128_MODE_REG_MASK;
}

EN_ERR_STA_T rom_hw_aes128_crypto(uint8_t *in, uint32_t inlen, uint8_t *out)
{
    uint8_t i = 0;
    uint32_t temp[AES128_BLOCK_SIZE/4] = {0};
    uint8_t sta = 0;
    uint32_t timeout = 0;
    uint32_t round = 0;

    g_aes_total_len += inlen;
    round = inlen / AES128_BLOCK_SIZE;

    AES128->AES_BYTE_NUM = g_aes_total_len;
    AES128->AES_ROUND_NUM = 1;

cal_block:
    timeout = 0;
    for (i = 0; i < AES128_BLOCK_SIZE; i += 4) {
        temp[AES128_BLOCK_SIZE/4 - 1 - i/4] = rom_hw_data_to_be32(in + i);
    }
    in += AES128_BLOCK_SIZE;

    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE);
    rom_hw_aes128_set_data(temp);
    AES128->AES_START = 1;

    do {
        rom_hw_aes128_get_interrupt_flag(&sta);
        if (sta & EN_AES128_INT_BLOCK_DONE)
            break;

        rom_utility_delay_cycles(1);
        timeout++;
    } while (timeout < AES128_WAIT_TIMEOUT);

    if (timeout >= AES128_WAIT_TIMEOUT)
        return EN_ERROR_STA_TIMEOUT;

    rom_hw_aes128_get_data(temp);
    for (i = 0; i < AES128_BLOCK_SIZE; i += 4) {
        rom_hw_data_to_be32_to_le(temp[AES128_BLOCK_SIZE/4 - 1 - i/4], out + i);
    }
    out += AES128_BLOCK_SIZE;

    round--;
    if (round > 0)
        goto cal_block;

    rom_hw_aes128_clear_interrupt_flag(EN_AES128_INT_BLOCK_DONE | EN_AES128_INT_BYTES_DONE | EN_AES128_INT_ROUND_DONE);
    AES128->AES_START = 0;

    return EN_ERROR_STA_OK;
}

EN_ERR_STA_T rom_hw_aes128_common(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t len, uint8_t *enc_data, EN_AES128_MODE_T mode)
{
    if (key == NULL || iv == NULL || data == NULL || enc_data == NULL || ((len % AES128_BLOCK_SIZE) != 0) || (len == 0))
        return EN_ERROR_STA_INVALID;

    rom_hw_aes128_init(mode, key, iv);

    return rom_hw_aes128_crypto(data, len, enc_data);
}

// AES default use Big-Endian mode. So, we need to convert input to Big-Endian, and convert output to Little-Endian.
EN_ERR_STA_T rom_hw_aes128_encrypt_once(uint8_t *key, uint8_t *iv, uint8_t *data, uint32_t len, uint8_t *enc_data)
{
    return rom_hw_aes128_common(key, iv, data, len, enc_data, EN_AES128_MODE_ENCRYPT);
}

EN_ERR_STA_T rom_hw_aes128_decrypt_once(uint8_t *key, uint8_t *iv, uint8_t *enc_data, uint32_t len, uint8_t *dec_data)
{
    return rom_hw_aes128_common(key, iv, enc_data, len, dec_data, EN_AES128_MODE_DECRYPT);
}

