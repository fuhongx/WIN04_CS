/**
 * ****************************************************************************
 * @file firmware_upgrade.h
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

#ifndef __FIRMWARE_UPGRADE_H__
#define __FIRMWARE_UPGRADE_H__

#define FIRM_UPGRADE_CMD_LEN                                (268)
#define FIRM_UPGRADE_RESP_FORMAT                            (0x80)

#define FIRM_UPGRADE_HEADER_LEN                             (4)
#define FIRM_UPGRADE_CRC_LEN                                (4)

#define FIRM_UPGRADE_CRC_INIT_VAL                           (0xFFFFFFFF)


#define FIRM_UPGRADE_CMD_HEADER                             (0xAA)
#define FIRM_UPGRADE_RESP_HEADER                            (0x55)

typedef enum
{
    EN_FIRM_STA_OK          =   0,
    EN_FIRM_STA_CMD_ERROR   =   0x1 << 0,
    EN_FIRM_STA_FRAM_ERROR  =   0x1 << 1,
    EN_FIRM_STA_CRC_ERROR   =   0x1 << 2,
    EN_FIRM_STA_MCU_ERROR   =   0x1 << 3,
}EN_FIRM_STA_T;

typedef enum
{
    EN_FIRM_CMD_GET_CHIP_INFO       =   0x01,
    EN_FIRM_CMD_SET_BAUD            =   0x02,
    EN_FIRM_CMD_START_DOWNBIN       =   0x03,
    EN_FIRM_CMD_FLASH_ERASE         =   0x04,
    EN_FIRM_CMD_FLASH_WRITE         =   0x05,
    EN_FIRM_CMD_FLASH_CHECK         =   0x06,
    EN_FIRM_CMD_RESET               =   0x07,
    EN_FIRM_CMD_GET_FLASH_ID        =   0x08,
    EN_FIRM_CMD_DOWNBIN_END         =   0x0A,
    EN_FIRM_CMD_READ_FLASH_STA      =   0x0B,
    EN_FIRM_CMD_WRITE_FLASH_STA     =   0x0C,
    EN_FIRM_CMD_READ_SEC_REG        =   0x0D,
    EN_FIRM_CMD_WRITE_SEC_REG       =   0x0E,
    EN_FIRM_CMD_ERASE_SEC_REG       =   0x0F,
    EN_FIRM_CMD_GET_FLASH_DEVID     =   0x10,
    EN_FIRM_CMD_FLASH_ERASE_CHIP    =   0x11,
    EN_FIRM_CMD_ENC                 =   0x12,
    EN_FIRM_CMD_DEC                 =   0x13,
}EN_FIRM_UPGRADE_CMD_T;

typedef enum
{
    EN_FIRM_RESP_UNKNOWN            =   FIRM_UPGRADE_RESP_FORMAT,
    EN_FIRM_RESP_GET_CHIP_INFO      =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_GET_CHIP_INFO,
    EN_FIRM_RESP_SET_BAUD           =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_SET_BAUD,
    EN_FIRM_RESP_START_DOWNBIN      =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_START_DOWNBIN,
    EN_FIRM_RESP_FLASH_ERASE        =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_FLASH_ERASE,
    EN_FIRM_RESP_FLASH_WRITE        =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_FLASH_WRITE,
    EN_FIRM_RESP_FLASH_CHECK        =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_FLASH_CHECK,
    EN_FIRM_RESP_RESET              =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_RESET,
    EN_FIRM_RESP_FLASH_ID           =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_GET_FLASH_ID,
    EN_FIRM_RESP_DOWNBIN_END        =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_DOWNBIN_END,
    EN_FIRM_RESP_FLASH_DEVID        =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_GET_FLASH_DEVID,
    EN_FIRM_RESP_FLASH_ERASE_CHIP   =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_FLASH_ERASE_CHIP,
    EN_FIRM_RESP_ENC                =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_ENC,
    EN_FIRM_RESP_DEC                =   FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_DEC,
}EN_FIRM_UPGRADE_RESP_T;

typedef struct __attribute__((packed))
{
    uint8_t au8ChipId[8];
    uint32_t u32SocVer;
    uint32_t u32BootVer;
    uint8_t u8BootSel;
}stFirmUpgradeInfo_t;

extern void firmware_upgrade_main(void);

#endif
