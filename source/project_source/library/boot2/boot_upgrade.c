#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utility.h"

#include "debug.h"
#include "boot_upgrade.h"
#include "hw_crc.h"
#include "hw_flash.h"
#include "app_cfg.h"

//------------------------------------------------------------------------------
// Upgrade in flash boot.
//------------------------------------------------------------------------------
uint32_t u32Buffer[UNIT_SECTOR / 4] = {0};

uint32_t au32ReadBuffer[BOOT2HEADER_LEN / 4] = {0};
extern uint32_t rom_hw_crc32(uint8_t *pu8Buffer, uint16_t u16Len);
typedef enum
{
    EN_BOOT_UPGRADE_ERASE_BOOT2,
    EN_BOOT_UPGRADE_ERASE_FIRMWARE,
} EN_BOOT_UPGRADE_ERASE_T;

void boot_upgrade_erase_block(EN_BOOT_UPGRADE_ERASE_T enMode)
{
    switch (enMode) {
        case EN_BOOT_UPGRADE_ERASE_BOOT2:
        {
            rom_hw_flash_erase_by_length(FLASH_BOOT2_HEADER_ADDR, FLASH_BOOT2_SIZE);
            break;
        }

        case EN_BOOT_UPGRADE_ERASE_FIRMWARE:
        {
            rom_hw_flash_erase_by_length(FLASH_BOOT2_FIRMWARE_HEADER_ADDR, FLASH_CODE_BLOCK_A_SIZE - FLASH_BOOT2_SIZE);
            break;
        }

        default:
            break;
    }
}

/**
 * @brief Must aligned 4.
 *
 * @param u32DstAddr
 * @param u32SrcAddr
 * @param u32Len
 * @return true
 * @return false
 */
bool boot_upgrade_flash_to_flash(uint32_t u32DstAddr, uint32_t u32SrcAddr, uint32_t u32Len)
{
    uint32_t u32WriteLen = u32Len;

    if (u32Len % 4)
    {
        u32WriteLen = u32Len / 4 + 1;
    }

    rom_hw_flash_erase_by_length(u32DstAddr, u32WriteLen);

    while (u32WriteLen)
    {
        if (u32WriteLen < UNIT_SECTOR)
        {
            rom_hw_flash_read_data_u32(u32SrcAddr, u32Buffer, u32WriteLen / 4);
            rom_hw_flash_write_data_u32(u32DstAddr, u32Buffer, u32WriteLen / 4);
            u32SrcAddr += u32WriteLen;
            u32DstAddr += u32WriteLen;
            break;
        }
        else
        {
            // Copy by sector
            rom_hw_flash_read_data_u32(u32SrcAddr, u32Buffer, UNIT_SECTOR / 4);
            rom_hw_flash_write_data_u32(u32DstAddr, u32Buffer, UNIT_SECTOR / 4);
            u32WriteLen -= UNIT_SECTOR;
            u32SrcAddr += UNIT_SECTOR;
            u32DstAddr += UNIT_SECTOR;
        }
    }

    return true;
}

extern uint8_t rom_get_safe_level(void);
extern int rom_check_firmware(uint32_t u32Addr, uint32_t u32Len, uint8_t *pu8Hash, uint8_t *pu8Sign);
bool boot_upgrade_update_boot2_code(boot2_header_t *pstBoot2)
{
    if (NULL == pstBoot2)
        return false;

    boot2_header_t *pstUpgradeVer;
    bool bUpgradeFlag = false;
    uint8_t hash[HASH_MAX_LEN] = {0};
    int ret = 0;

    // 1 -> Read upgrade header
    rom_hw_flash_read_data_u32(FLASH_FIRM_UP_BOOT2_HEADER_ADDR, au32ReadBuffer, BOOT2HEADER_LEN / 4);
    pstUpgradeVer = (boot2_header_t *)au32ReadBuffer;

    // 2 -> Check header crc.
    uint32_t u32HeaderCrc = rom_hw_crc32((uint8_t *)au32ReadBuffer, BOOT2HEADER_LEN - BOOT2HEADER_CRC_LEN);
    if(pstUpgradeVer->u32Boot2HeaderCrc != u32HeaderCrc) {
        PRINTF("[BL2] warning check Boot2upgrade HeaderCrc fail, Read = 0x%x, Caculate Crc= 0x%x\n",
                pstUpgradeVer->u32Boot2HeaderCrc, u32HeaderCrc);
        return false;
    }

    // 3 -> Switch flag.
    switch (pstUpgradeVer->u32Boot2Flag)
    {
        case EN_FIRMWARE_UPGRADE:
        {
            if (pstBoot2->u32Boot2Ver < pstUpgradeVer->u32Boot2Ver) {
                bUpgradeFlag = true;
            }

            break;
        }

        default:
            break;
    }

    if (bUpgradeFlag) {
        PRINTF("[BL2] start to upgrade\n");

        ret = rom_check_firmware(FLASH_FIRM_UP_BOOT2_CODE_ADDR, pstUpgradeVer->u32Boot2Len, pstUpgradeVer->hash, pstUpgradeVer->signiture);
        if (ret != 0) {
            PRINTF("[BL2] check Boot2 upgrade fail\n");
            return false;
        }

        boot_upgrade_erase_block(EN_BOOT_UPGRADE_ERASE_BOOT2);
        boot_upgrade_flash_to_flash(FLASH_BOOT2_HEADER_ADDR, FLASH_FIRM_UP_BOOT2_HEADER_ADDR, pstUpgradeVer->u32Boot2Len + BOOT2HEADER_LEN);
        PRINTF("[BL2] boot2 upgrade end\n");
        return true;
    }

    PRINTF("[BL2] Boot2 donot upgrade\n");

    return true;
}

bool boot_upgrade_update_app_code(fw_header_t *pstFirm)
{
    if (NULL == pstFirm)
        return false;

    bool bUpgradeFlag = false;
    uint8_t safe_level = rom_get_safe_level();
    int ret = 0;

    rom_hw_flash_read_data_u32(FLASH_FIRM_UP_FIRMWARE_HEADER_ADDR, au32ReadBuffer, sizeof(au32ReadBuffer));

    fw_header_t *pstUpgrade = (fw_header_t *)au32ReadBuffer;

    // 2 -> Check header crc.
    uint32_t u32HeaderCrc = rom_hw_crc32((uint8_t *)au32ReadBuffer, BOOT2HEADER_LEN - BOOT2HEADER_CRC_LEN);
    if(pstUpgrade->u32FirmwareHeaderCrc != u32HeaderCrc)
    {
        PRINTF("[BL2] warning check FW_upgrade HeaderCrc fail, Read = 0x%x, Caculate Crc= 0x%x\n",
                pstUpgrade->u32FirmwareHeaderCrc, u32HeaderCrc);
        goto check_fw_integrality;
    }

    switch (pstUpgrade->u32FirmwareFlag)
    {
        case EN_FIRMWARE_UPGRADE:
        {
            if (pstFirm->u32FirmwareVer < pstUpgrade->u32FirmwareVer)
                bUpgradeFlag = true;

            break;
        }

        default:
            break;
    }

    if (bUpgradeFlag)
    {
        PRINTF("[BL2] Firmware start to upgrade!\n");
        ret = rom_check_firmware(FLASH_FIRM_UP_FIRMWARE_HEADER_MAX_ADDR, pstUpgrade->u32FirmwareLen, pstUpgrade->hash, pstUpgrade->signiture);
        if (ret != 0) {
            PRINTF("[BL2] check Upgrade FW code fail\n");
            goto check_fw_integrality;
        }

        boot_upgrade_erase_block(EN_BOOT_UPGRADE_ERASE_FIRMWARE);
        boot_upgrade_flash_to_flash(FLASH_BOOT2_FIRMWARE_HEADER_ADDR, FLASH_FIRM_UP_FIRMWARE_HEADER_ADDR, pstUpgrade->u32FirmwareLen + FLASH_BOOT2_FIRMWARE_HEADER_SIZE);
    }

check_fw_integrality:
    memset(au32ReadBuffer, 0, sizeof(au32ReadBuffer));
    fw_header_t *cur_header = (fw_header_t *)au32ReadBuffer;

    rom_hw_flash_read_data_u32(FLASH_BOOT2_FIRMWARE_HEADER_ADDR, au32ReadBuffer, sizeof(au32ReadBuffer));

    if ((safe_level == 0) || (safe_level >= 5)) {
        PRINTF("[BL2] Firmware safe level is 0x%X, bypass check FW integrality\n", safe_level);
        return true;
    }

    PRINTF("[BL2] start to check current Firmware integrality, safe_level = %u\n", safe_level);
    uint32_t cur_HeaderCrc = rom_hw_crc32((uint8_t *)au32ReadBuffer, FIRMWARE_HEADER_LEN - BOOT2HEADER_CRC_LEN);
    if (cur_header->u32FirmwareHeaderCrc != cur_HeaderCrc) {
        PRINTF("[BL2] check Current Firmware HeaderCRC fail! Read = 0x%x, Caculate Crc= 0x%x\n",
                cur_header->u32FirmwareHeaderCrc, cur_HeaderCrc);
        goto err;
    }

    // 此处校验APP的HASH时，地址需要减去4字节的SP，否则会导致校验失败
    ret = rom_check_firmware(cur_header->u32FirmwareAddr - 4, cur_header->u32FirmwareLen, cur_header->hash, cur_header->signiture);
    if (ret != 0) {
        goto err;
    }

    return true;

err:
    PRINTF("[BL2] Current Firmware is broken!\n");
    while (1)
    {
        rom_utility_delay_ms(1000);
    }

    return false;
}