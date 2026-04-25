/**
 * @file     flash_prog.c
 * @author   Maxscend SoC Software Team
 * @version  V1.0
 * @date     2023/5/26
 * @history
 * @note
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION REGARDING THEIR
 * PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, MAXSCEND SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE
 * USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * Copyright (c) 2016~2023, Maxscend Microelectronics Company Limited.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"

#include "hw_sysctrl.h"
#include "hw_flash.h"
#include "utility.h"
#include "app_cfg.h"

#include "FlashOS.H"
#include "flash_prog.h"

#define BLOCK_ADJUST_SIZE       (0) //(0) //(1024 * 124)

void flash_test_read_status_reg(void)
{
    PRINTF("------------------Flash Read Status Reg-----------------\n");
    uint8_t u8Status = 0;
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG1, &u8Status);
    PRINTF("Status Reg1 = 0x%x\n", u8Status);
    u8Status = 0;
    rom_hw_flash_read_status_reg(EN_FLASH_READ_STA_REG2, &u8Status);
    PRINTF("Status Reg2 = 0x%x\n", u8Status);
}

/**
 * @brief  Initialize Flash Programming Functions.
 * @param  u32Addr: Device Base Address.
 * @param  u32Clk: Clock Frequency (Hz).
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
static void flash_init(void)
{
    rom_hw_sysctrl_system_clock_init(EN_SYS_CLK_TCXO25M, EN_SYS_CLK_MCLK_DIV_NONE);
    rom_hw_flash_init();
    rom_hw_flash_set_type(EN_FLASH_TYPE_PY);
    rom_hw_flash_config_read_write_mode(EN_FLASH_RW_MODE_QSPI);

    rom_hw_sysctrl_set_cache_mode(EN_CACHE_DISABLE);
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);

    flash_test_read_status_reg();
    rom_hw_flash_write_status_reg(EN_FLASH_READ_STA_REG1, 0x0);
}

/**
 * @brief  Initialize Flash Programming Functions.
 * @param  u32Addr: Device Base Address.
 * @param  u32Clk: Clock Frequency (Hz).
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
int Init(unsigned long u32Addr, unsigned long u32Clk, unsigned long enCode)
{
    int      retCode = 0;
    uint32_t u32Val  = 0;

    extern void debug_printf_init(void);
    debug_printf_init();

    flash_init();

    switch (enCode)
    {
        case FUNC_CODE_ERASE:
        {
            PRINTF("Init case 1. Start to erase.\n");
            break;
        }
        case FUNC_CODE_PROGM:
        {
            PRINTF("Init case 2. Start to program.\n");
            break;
        }
        case FUNC_CODE_VERFIY:
        {
            PRINTF("Init case 3. Start to verify.\r\n");
            break;
        }
        default:
        {
            PRINTF("Init case default.\r\n");
            break;
        }
    }
    
    return retCode; // Finished without Errors
}

/**
 * @brief  Deinitialize Flash Programming Functions.
 * @param  enCode: Function Code(1: Erase, 2: Program, 3: Verify).
 * @return 0: OK, 1: Failed.
 */
int UnInit(unsigned long enCode)
{
    switch (enCode)
    {
        case FUNC_CODE_ERASE:
        {
            PRINTF("UnInit case 1. Erase done.\r\n\n");
            break;
        }
        case FUNC_CODE_PROGM:
        {
            PRINTF("UnInit case 2. Program done.\r\n\n");
            break;
        }
        case FUNC_CODE_VERFIY:
        {
            PRINTF("UnInit case 3. Verify done.\r\n\n");
            break;
        }
        default:
        {
            PRINTF("UnInit 4. Default.\r\n");
            break;
        }
    }

    return EN_ERROR_STA_OK;
}

/**
 * @brief  Erase complete Flash Memory.
 * @return 0: OK, 1: Failed.
 */


/**
 * @brief  Erase Sector in Flash Memory.
 * @param  u32Addr: Sector Address.
 * @return 0: OK, 1: Failed.
 */
int EraseSector(unsigned long u32Addr)
{
    uint32_t u32AddrTmp   = u32Addr + BLOCK_ADJUST_SIZE;
    uint32_t u32AdrOffset = 0;
    uint8_t u8Ret = EN_ERROR_STA_OK;

    PRINTF("Erase sector. Addr:0x%08X, ", u32AddrTmp);
    u8Ret |= rom_hw_flash_erase_by_length(u32AddrTmp, 120 * KBYTE);
    PRINTF("Erase Ret = 0x%x\n",u8Ret);
    return 0;
}

int EraseChip(void)
{
    EN_ERR_STA_T retCode = 0;
    retCode = rom_hw_flash_erase(EN_FLASH_ERASE_CHIP, 0x0);
    return (int)retCode;
}

/**
 * @brief  Program Page in Flash Memory.
 * @param  u32Addr: Page Start Address.
 * @param  u32Size: Page Size.
 * @param  pBuf: Page Data.
 * @return 0: OK, 1: Failed.
 */
int ProgramPage(unsigned long u32Addr, unsigned long u32Size, unsigned char *pBuf)
{
    uint32_t u32SizeTmp = u32Size;
    uint32_t u32AddrTmp = u32Addr + BLOCK_ADJUST_SIZE;

    uint16_t u16Len    = 0;
    uint32_t u32Offset = 0;

    u32AddrTmp |= FLASH_BASE_ADDR;
    rom_hw_flash_write_data(u32AddrTmp, pBuf, u32Size);
    PRINTF("Program page. Addr:0x%08X, Size:0x%08X, ", u32AddrTmp, u32SizeTmp);

    return EN_ERROR_STA_OK;
}

/**
 * @brief  Verify function.
 * @param  u32Addr: Start Address.
 * @param  u32Size: Size in bytes.
 * @param  pBuf: Page Data.
 * @return OK: u32Addr + u32Size, Failed: any other number.
 */
unsigned long Verify(unsigned long u32Addr, unsigned long u32Size, unsigned char *pBuf)
{
    uint8_t  pu8Buffer[300];
    uint32_t u32SizeTmp = u32Size;
    uint32_t u32AddrTmp = u32Addr + BLOCK_ADJUST_SIZE;

    uint32_t u32Offset;
    uint16_t u16Len;
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_DISABLE);
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
    PRINTF("Verify page. Addr:0x%08X, Size:0x%08X, ", u32AddrTmp, u32SizeTmp);
    // App Code. Form .bin 0x80000000 ~ 0x80040000 write to chip 0x00000000 ~ 0x00040000
    if ((u32AddrTmp >= FLASH_BASE_ADDR) && ((u32AddrTmp + u32SizeTmp) <= (FLASH_BASE_ADDR + MAX_FLASH_SIZE)))
    {
        u16Len    = (uint16_t)(u32SizeTmp & 0xFFFF);
        u32Offset = u32AddrTmp;// - FLASH_BASE_ADDR;

        PRINTF("READ ADDR = 0x%x\n", u32Offset);
        PRINTF("READ u16Len = 0x%x\n", u16Len);
        if (EN_ERROR_STA_OK != rom_hw_flash_read_data(u32Offset, pu8Buffer, u16Len))
        {
            return u32Addr;
        }

        if (memcmp(pBuf, pu8Buffer, u16Len))
        {
            PRINTF("Offset addr:0x%08X.\r\n", u32Offset);
            PRINTF("Verify page failure.\r\n");
            return u32Addr;
        }
    }
    else
    {
        PRINTF("Verify. Addr is illegal.\n");
        return u32Addr;
    }

    PRINTF("Offset addr:0x%08X.\r\n", u32Offset);

    return (u32Addr + u32Size);
 
    return EN_ERROR_STA_OK;
}
