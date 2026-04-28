/**
 * ****************************************************************************
 * @file bootloader.h
 * @author SLC(SC) software team. 
 * @brief 
 * @version  1.0
 * ****************************************************************************
 * 
 * Copyright (c) 2024 SLC Semiconductor all rights reserved.
 * 
 * ****************************************************************************
 */


#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#include "compiler.h"

#define FLASH_SELF_CALI_CLK_DIV_MAX         (0xA)
#define FLASH_SELF_CALI_INIT_QSPI_DELAY     (0x01010101)

typedef struct
{
    uint8_t u8FlashClkDiv;
    unFlashQspiDelay_t u32FlashQspiDelay;
    uint8_t u8SampleDelay;
    uint8_t u8SampleEdge;
    uint8_t u8Index;
    EN_FLASH_RW_MODE_T enMode;
}stFlashSelfCali_t;

#define BOOT_SECURITY_INFO_BUF_SIZE         (256)

typedef enum
{
    FW_SAFE_LEVEL0 = 0,     // no security, for test only, flash no encrypt, support swd
    FW_SAFE_LEVEL1,         // flash encrypt, no swd, only normal startup check hash, low power startup not check
    FW_SAFE_LEVEL2,         // flash encrypt, no swd, only normal startup check hash and signature, low power startup not check
    FW_SAFE_LEVEL3,         // flash encrypt, no swd, normal startup check all, but low power startup only check hash
    FW_SAFE_LEVEL4,         // flash encrypt, no swd, normal startup check all, low power startup check all

    FW_SAFE_LEVEL_MAX,      // must be the last one
} fw_safe_level_t;

typedef struct 
{
    uint8_t fw_enc_capable;     // 0: not support, 1: support
    uint8_t otp_fuse_capable;   // 0: not support, 1: support
    uint8_t resv[30];           // reserved, padding to 32 bytes
} fw_security_cap_t;

typedef struct 
{
    uint8_t fw_enc_status;      // 0: not encrypted, 1: encrypted
    uint8_t otp_fuse_status;    // 0: not fuse, 1: fused
    uint8_t hash_type;          // 0: SHA256, 1: SM3
    uint8_t sign_type;          // 0: ECC256(k1), 1: SM2
    uint8_t safe_level;         // see fw_safe_level_t
    uint8_t resv[27];           // reserved, padding to 32 bytes
} fw_security_sta_t;

typedef struct 
{
    uint8_t pub_key[64];        // ECC256(k1) or SM2 public key
    uint8_t resv[128];          // reserved, add new public key or other key, padding to 192 bytes
} fw_security_key_t;

typedef struct 
{
    fw_security_cap_t cap;      // security capability, read only, 32 bytes
    fw_security_sta_t sta;      // security status, read write, 32 bytes
    fw_security_key_t key;      // security key, read write, 192 bytes
} fw_security_info_t;

void boot_identify_from_lowpower(void);
void boot_set_flash_enc_by_security_info(void);
void boot_selection(void);

#endif