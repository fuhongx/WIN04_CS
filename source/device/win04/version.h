/**
 * ****************************************************************************
 * @file version.h
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

#ifndef __VERSION_H__
#define __VERSION_H__

typedef enum
{
    EN_FIRMWARE_DEBUG           =   0xAA,  // Debug mode, Firmware will not be checked.

    EN_FIRMWARE_UPGRADE         =   0xC1,  // Code will be covered when version upgrade.
}EN_FIRMWARE_T;

#define HASH_MAX_LEN            (32U)
#define SIGNATURE_MAX_LEN       (64U)

#define FIRMWARE_FLAG           (EN_FIRMWARE_DEBUG)
#define FIRMWARE_VERSION        (0x0004)        // V0004
#define FIRMWARE_HEADER_LEN     (FLASH_BOOT2_FIRMWARE_HEADER_SIZE)

typedef struct __attribute__((packed))
{
    uint32_t u32FirmwareFlag;
    uint32_t u32FirmwareVer;
    uint32_t u32FirmwareLen;
    uint32_t u32FirmwareCrc;
    uint32_t u32FirmwareAddr;
    uint8_t hash[HASH_MAX_LEN];
    uint8_t signiture[SIGNATURE_MAX_LEN];
    uint32_t u32FirmwareHeaderCrc;
} fw_header_t;

/**
 * @brief It takes effect after recompiling boot2 project.
 * 
 */
#define BOOT2CCODE_FLAG         (EN_FIRMWARE_UPGRADE)
#define BOOT2CODE_VERSION       (0x0001)
#define BOOT2HEADER_LEN         (FLASH_BOOT2_HEADER_SIZE)
#define BOOT2HEADER_CRC_LEN     (4)

typedef struct __attribute__((packed))
{
    uint32_t u32Boot2Flag;
    uint32_t u32Boot2Ver;
    uint32_t u32Boot2Len;
    uint32_t u32Boot2Crc;
    uint32_t u32Boot2LoadAddr;
    uint8_t hash[HASH_MAX_LEN];
    uint8_t signiture[SIGNATURE_MAX_LEN];
    uint32_t u32Boot2HeaderCrc;
} boot2_header_t;


#endif


