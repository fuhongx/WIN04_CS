/**
 * ****************************************************************************
 * @file firmware_upgrade.c
 * @author SLC(SC) software team. 
 * @brief This file is responsible for firmware updates and upgrades at boot time. 
 *        It is currently limited to Boot2 mode and needs to be modified in direct boot mode.
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"

#include "hw_crc.h"
#include "hw_flash.h"
#include "hw_uart.h"
#include "hw_rng.h"
#include "hw_sysctrl.h"
#include "firmware_upgrade.h"
#include "app_cfg.h"
#include "version.h"
#include "boot_interface.h"
#include "sha256.h"

bool bDownbinFlag = false;
uint32_t u32FlashReadBuffer[UNIT_PAGE/2] = {0};

// uint8_t au8FirmUartRxBuffer[BOOT_TRX_BUFFER_SIZE] = {0};
uint8_t au8FirmRespBuffer[BOOT_TRX_BUFFER_SIZE];
uint8_t g_boot_flash_devid[3] = {0};

//------------------------------------------------------------------------------
//Upgrade by cmd.
//------------------------------------------------------------------------------
void firmware_upgrade_uart_tx(uint8_t *pu8Buffer, uint16_t u16Len)
{
    boot_interface_send_data(pu8Buffer, u16Len);
}

void firmware_upgrade_send_error(uint8_t u8Resp, uint8_t u8ErrorType)
{
    memset(au8FirmRespBuffer, 0, sizeof(au8FirmRespBuffer));
    au8FirmRespBuffer[0] = FIRM_UPGRADE_RESP_HEADER;
    au8FirmRespBuffer[1] = u8Resp;
    au8FirmRespBuffer[2] = 0x1 & 0xFF;
    au8FirmRespBuffer[3] = (0x1 >> 8) & 0xFF;
    au8FirmRespBuffer[4] = u8ErrorType;
    uint32_t u32Crc = rom_hw_crc_get_crc32_value(au8FirmRespBuffer, 4 + 1);
    rom_utility_little_endian_store_32(au8FirmRespBuffer, 4 + 1, u32Crc);
    firmware_upgrade_uart_tx(au8FirmRespBuffer, 1 + 8);
}

void firmware_send_resp(uint8_t u8Resp, uint8_t* pu8Buffer, uint16_t u16Len)
{
    memset(au8FirmRespBuffer, 0, sizeof(au8FirmRespBuffer));
    au8FirmRespBuffer[0] = FIRM_UPGRADE_RESP_HEADER;
    au8FirmRespBuffer[1] = u8Resp;
    au8FirmRespBuffer[2] = u16Len & 0xFF;
    au8FirmRespBuffer[3] = (u16Len >> 8) & 0xFF;
    memcpy(&au8FirmRespBuffer[4], pu8Buffer, u16Len);
    
    uint32_t u32Crc = rom_hw_crc_get_crc32_value(au8FirmRespBuffer, 4 + u16Len);

    rom_utility_little_endian_store_32(au8FirmRespBuffer, 4 + u16Len, u32Crc);
    firmware_upgrade_uart_tx(au8FirmRespBuffer, u16Len + 8);
}

void firmware_change_baud(uint32_t u32Baud)
{
    boot_interface_change_baudrate(u32Baud);
}

bool firmware_upgrade_is_cmd_valid(uint8_t* pu8Buffer, uint16_t u16Len)
{
    // Check header
    if(pu8Buffer[0] != FIRM_UPGRADE_CMD_HEADER)
    {
        firmware_upgrade_send_error(EN_FIRM_RESP_UNKNOWN, EN_FIRM_STA_FRAM_ERROR);
        PRINTF("Firm upgrade error in cmd header\n");
        return false;
    }

    // Check data len
    if(rom_utility_little_endian_read_16(pu8Buffer, 2) != (u16Len - FIRM_UPGRADE_HEADER_LEN - FIRM_UPGRADE_CRC_LEN))
    {
        firmware_upgrade_send_error(EN_FIRM_RESP_UNKNOWN, EN_FIRM_STA_FRAM_ERROR);
        PRINTF("Firm upgrade error in cmd data len\n");
        return false;
    }

    // Check Crc
    uint32_t u32Crc = rom_hw_crc_get_crc32_value(pu8Buffer, (u16Len - FIRM_UPGRADE_CRC_LEN));
    uint32_t u32ReadCrc = rom_utility_little_endian_read_32(pu8Buffer, u16Len - FIRM_UPGRADE_CRC_LEN);

    if(u32Crc != u32ReadCrc)
    {
        firmware_upgrade_send_error(EN_FIRM_RESP_UNKNOWN, EN_FIRM_STA_CRC_ERROR);
        PRINTF("Firm upgrade error in cmd crc, Read Crc = 0x%x, Own Crc = 0x%x\n", u32ReadCrc, u32Crc);
        return false;
    }

    return true;
}

void firmware_upgrade_cmd_handler(uint8_t u8Cmd, uint8_t* pu8Data, uint16_t u16Len)
{
    uint8_t u8Sta = EN_FIRM_STA_OK;
    // uint32_t flash_size = 1 << g_boot_flash_devid[2];
    uint32_t flash_size = FLASH_SIZE_MAX;   // 当前芯片限制仅可支持256K，超过256K不可读写

    switch (u8Cmd)
    {
    case EN_FIRM_CMD_GET_CHIP_INFO:
        {
            uint32_t u32ChipId = SYS_CTRL->CHIP_ID;

            firmware_send_resp(EN_FIRM_RESP_GET_CHIP_INFO, (uint8_t *)&u32ChipId, 4);
            PRINTF("[BL CMD] EN_FIRM_CMD_GET_CHIP_INFO\n");
            break;
        }

    case EN_FIRM_CMD_SET_BAUD:
        {
            uint32_t u32Baud = rom_utility_little_endian_read_32(pu8Data, 0);

            firmware_send_resp(EN_FIRM_RESP_SET_BAUD, pu8Data, 4);

            rom_utility_delay_ms(1);
            firmware_change_baud(u32Baud);
            PRINTF("[BL CMD] EN_FIRM_CMD_SET_BAUD, NewBaud = %d\n", u32Baud);
            break;
        }

    case EN_FIRM_CMD_START_DOWNBIN:
        {
            bDownbinFlag = true;
            u8Sta = EN_FIRM_STA_OK;
            firmware_send_resp(EN_FIRM_RESP_START_DOWNBIN, &u8Sta, 1);
            PRINTF("[BL CMD] EN_FIRM_CMD_START_DOWNBIN\n");
            break;
        }

    case EN_FIRM_CMD_FLASH_ERASE:
        {
            if(!bDownbinFlag)
            {
                u8Sta = EN_FIRM_STA_MCU_ERROR;
                firmware_send_resp(EN_FIRM_RESP_FLASH_ERASE, &u8Sta, 1);
            }
            uint32_t u32Addr = rom_utility_little_endian_read_32(pu8Data, 0);
            uint32_t u32Len = rom_utility_little_endian_read_32(pu8Data, 4);
            uint8_t u8Ret = rom_hw_flash_erase_by_length(u32Addr, u32Len);

            if(EN_ERROR_STA_OK != u8Ret)
            {
                u8Sta = EN_FIRM_STA_MCU_ERROR;
            }
            else
            {
                u8Sta = EN_FIRM_STA_OK;
            }

            firmware_send_resp(EN_FIRM_RESP_FLASH_ERASE, &u8Sta, 1);
            PRINTF("[BL CMD] EN_FIRM_RESP_FLASH_ERASE\n");
            break;
        }
    
    case EN_FIRM_CMD_FLASH_WRITE:
        {
            if(!bDownbinFlag)
            {
                u8Sta = EN_FIRM_STA_MCU_ERROR;
                firmware_send_resp(EN_FIRM_RESP_FLASH_WRITE, &u8Sta, 1);
            }

            uint32_t u32Addr = rom_utility_little_endian_read_32(pu8Data, 0);
            uint32_t u32Len = u16Len - 4;
            uint8_t u8Ret = rom_hw_flash_write_data(u32Addr, &pu8Data[4], u32Len);
            if(EN_ERROR_STA_OK != u8Ret)
            {
                u8Sta = EN_FIRM_STA_MCU_ERROR;
            }
            else
            {
                u8Sta = EN_FIRM_STA_OK;
            }
            firmware_send_resp(EN_FIRM_RESP_FLASH_WRITE, &u8Sta, 1);
            PRINTF("[BL CMD] EN_FIRM_RESP_FLASH_WRITE\n");
            break;
        }
    
    case EN_FIRM_CMD_FLASH_CHECK:
        {
            uint32_t u32Addr = rom_utility_little_endian_read_32(pu8Data, 0);
            uint32_t u32Len = rom_utility_little_endian_read_32(pu8Data, 4);
            uint32_t u32CrcInit = rom_utility_little_endian_read_32(pu8Data, 8);
            uint8_t au8Buffer[32] = {0};
            sha256((uint8_t *)u32Addr, u32Len, au8Buffer);
            firmware_send_resp(EN_FIRM_RESP_FLASH_CHECK, au8Buffer, 32);
            break;
        }

    case EN_FIRM_CMD_RESET:
        {
            u8Sta = EN_FIRM_STA_OK;
            firmware_send_resp(EN_FIRM_RESP_RESET, &u8Sta, 1);
            PRINTF("[BL CMD] EN_FIRM_CMD_RESET\n");
            //Wait add
            __NVIC_SystemReset();
            break;
        }

    case EN_FIRM_CMD_GET_FLASH_ID:
        {
            uint8_t au8Buffer[8] = {0};
            rom_hw_flash_read_uid(&au8Buffer[0], 8);
            firmware_send_resp(EN_FIRM_RESP_FLASH_ID, au8Buffer, 8);

            break;
        }

    case EN_FIRM_CMD_GET_FLASH_DEVID:
        {
            uint8_t au8Buffer[4] = {0};
            uint8_t u8Ret = rom_hw_flash_read_dev_id(au8Buffer, 3);
            firmware_send_resp(EN_FIRM_RESP_FLASH_DEVID, au8Buffer, 3);

            break;
        }

    case EN_FIRM_CMD_DOWNBIN_END:
        {
            bDownbinFlag = false;
            u8Sta = EN_FIRM_STA_OK;
            firmware_send_resp(EN_FIRM_RESP_DOWNBIN_END, &u8Sta, 1);
            PRINTF("[BL CMD] EN_FIRM_CMD_DOWNBIN_END\n");
            break;
        }

    case EN_FIRM_CMD_READ_FLASH_STA:
    {
        uint8_t au8Buffer;
        rom_hw_flash_read_status_reg(pu8Data[0], &au8Buffer);
        firmware_send_resp(EN_FIRM_CMD_READ_FLASH_STA | FIRM_UPGRADE_RESP_FORMAT, &au8Buffer, 1);
        PRINTF("[BL CMD] EN_FIRM_CMD_READ_FLASH_STA\n");
         break;
    }

    case EN_FIRM_CMD_WRITE_FLASH_STA:
    {
        u8Sta = rom_hw_flash_write_status_reg(pu8Data[0], pu8Data[1]);
        firmware_send_resp(FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_WRITE_FLASH_STA, &u8Sta, 1);
        PRINTF("[BL CMD] EN_FIRM_CMD_WRITE_FLASH_STA\n");
        break;
    }

    case EN_FIRM_CMD_READ_SEC_REG:
    {
        // 0 - MemIdx , 1-2 offset, 3-4 Len
        uint8_t *pu8Resp = (uint8_t*) u32FlashReadBuffer;
        uint16_t u16addrOffset = rom_utility_little_endian_read_16(pu8Data, 1);
        uint16_t u16Len = rom_utility_little_endian_read_16(pu8Data, 3);
        rom_hw_flash_read_security_mem(pu8Data[0], u16addrOffset, pu8Resp, u16Len);
        firmware_send_resp(FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_READ_SEC_REG, pu8Resp, u16Len);
        break;
    }

    case EN_FIRM_CMD_WRITE_SEC_REG:
    {
        // 0 - Idx , 1-2 0ffset, 3-4 Len
        uint8_t *pu8Resp = (uint8_t*) u32FlashReadBuffer;
        uint16_t u16addrOffset = rom_utility_little_endian_read_16(pu8Data, 1);
        uint16_t u16Len = rom_utility_little_endian_read_16(pu8Data, 3);

        // 写之前需确保写入区域已被擦除为0xFF，即避免写入失败，同时防止恶意破坏密钥存储区导致启动异常
        rom_hw_flash_read_security_mem(pu8Data[0], u16addrOffset, pu8Resp, u16Len);
        for (uint32_t i = 0; i < u16Len; i++) {
            if (pu8Resp[i] != 0xFF) {
                u8Sta = EN_ERROR_STA_ERROR;
                break;
            }
        }
        if (u8Sta == EN_FIRM_STA_OK) {
            rom_hw_flash_write_security_mem(pu8Data[0], u16addrOffset, &pu8Data[5], u16Len);
            u8Sta = EN_ERROR_STA_OK;
        }

        firmware_send_resp(FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_WRITE_SEC_REG, &u8Sta, 1);
        PRINTF("[BL CMD] EN_FIRM_CMD_WRITE_SEC_REG\n");
        break;
    }

    case EN_FIRM_CMD_ERASE_SEC_REG:
    {
        u8Sta = EN_FIRM_STA_OK;
        rom_hw_flash_erase_security_mem(pu8Data[0], FLASH_ERASE_SECURITY_MEM_KEY);

        firmware_send_resp(FIRM_UPGRADE_RESP_FORMAT | EN_FIRM_CMD_ERASE_SEC_REG, &u8Sta, 1);
        PRINTF("[BL CMD] EN_FIRM_CMD_ERASE_SEC_REG\n");
        break;
    }

    case EN_FIRM_CMD_FLASH_ERASE_CHIP:
    {
        u8Sta = EN_FIRM_STA_OK;
        u8Sta = rom_hw_flash_erase(EN_FLASH_ERASE_CHIP, 0);
        firmware_send_resp(EN_FIRM_RESP_FLASH_ERASE_CHIP, &u8Sta, 1);
        PRINTF("[BL CMD] EN_FIRM_CMD_FLASH_ERASE_CHIP\n");
        break;
    }

    case EN_FIRM_CMD_ENC:
    {
        u8Sta = EN_FIRM_STA_OK;
        uint32_t fw_key = 0;
        uint8_t *pu8Resp = (uint8_t*) u32FlashReadBuffer;
        rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, pu8Resp, 256);
        fw_key = rom_hw_crc_get_crc32_value(&pu8Resp[64], 64);
        // 写入flash加密密钥
        SYS_CTRL->Res3 = fw_key;
        // 关闭SWD并打开flash加密
        SYS_CTRL->Res2 = 0x3;

        // flash加密依赖cache的使能
        rom_hw_sysctrl_set_cache_mode(EN_CACHE_ENABLE);
        rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);

        rom_hw_update_rng_seed(fw_key);
        firmware_send_resp(EN_FIRM_RESP_ENC, &u8Sta, 1);
        break;
    }

    case EN_FIRM_CMD_DEC:
    {
        u8Sta = EN_FIRM_STA_OK;
        uint8_t *pu8Resp = (uint8_t *)u32FlashReadBuffer;

        rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, pu8Resp, 256);
        if ((pu8Resp[32] == 0) && (pu8Resp[36] == 0)) {
            SYS_CTRL->Res2 = 0x0;   // disable security
            rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
            rom_hw_sysctrl_set_cache_mode(EN_CACHE_DISABLE);
        } else {
            u8Sta = EN_ERROR_STA_ERROR;
        }

        firmware_send_resp(EN_FIRM_RESP_DEC, &u8Sta, 1);
        break;
    }

    default:
        firmware_upgrade_send_error(u8Cmd, EN_FIRM_STA_CMD_ERROR);
        break;
    }
}

void firmware_upgrade_cmd_process(uint8_t* pu8Buffer, uint16_t u16Len)
{
    if(!firmware_upgrade_is_cmd_valid(pu8Buffer, u16Len))
    {
        return;
    }

    uint8_t u8Cmd = pu8Buffer[1];
    uint8_t *pu8Data = &pu8Buffer[FIRM_UPGRADE_HEADER_LEN];
    uint16_t u16DataLen = rom_utility_little_endian_read_16(pu8Buffer, 2);

    firmware_upgrade_cmd_handler(u8Cmd, pu8Data, u16DataLen);
}

void firmware_upgrade_main(void)
{
    volatile uint16_t u16RxLen = 0;
    uint8_t *au8FirmUartRxBuffer = malloc(FIRM_UPGRADE_UART_RX_LEN);
    memset(au8FirmUartRxBuffer, 0, FIRM_UPGRADE_UART_RX_LEN);

    while(1) {
        u16RxLen = boot_interface_get_rx_data(au8FirmUartRxBuffer);
        if(u16RxLen) {
            firmware_upgrade_cmd_process(au8FirmUartRxBuffer, u16RxLen);
            u16RxLen = 0;
            memset(au8FirmUartRxBuffer, 0, FIRM_UPGRADE_UART_RX_LEN);
        }
    }

}
