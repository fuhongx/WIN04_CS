/**
 * ****************************************************************************
 * @file boot2.c
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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "string.h"

#include "error_def.h"
#include "utility.h"

#include "boot_upgrade.h"
#include "reg_Dev.h"
#include "hw_flash.h"
#include "hw_sysctrl.h"
#include "hw_uart.h"
#include "hw_crc.h"
#include "hw_rng.h"
#include "boot2.h"
#include "app_cfg.h"

#include "version.h"

//Must Keep This Declare
void boot2_main(void) __attribute__ ((section(".bootmain.code")));

extern void debug_printf_init(void);

void peripheral_init(void)
{
#if APP_DEBUG_ENABLED
    debug_printf_init();
    PRINTF("[BL2]------Boot2 Start.Build Time:[%s T %s].------\n", __DATE__, __TIME__);
#endif

    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_CRC32, true);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_CRC32);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_RAND, true);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_RAND);
    rom_hw_rng_init(0, true);
    rom_hw_update_rng_seed(0x12345678);
}

void show_boot2_header_info(boot2_header_t *pstBoot2)
{
#if APP_DEBUG_ENABLED
    PRINTF("[BL2] Boot2Flag= 0x%X\n", pstBoot2->u32Boot2Flag);
    PRINTF("[BL2] Boot2Ver= 0x%X\n", pstBoot2->u32Boot2Ver);
    PRINTF("[BL2] Boot2Len= 0x%X\n", pstBoot2->u32Boot2Len);
    PRINTF("[BL2] Boot2LoadAddr= 0x%X\n", pstBoot2->u32Boot2LoadAddr);
    PRINTF("[BL2] Boot2Crc= 0x%X\n", pstBoot2->u32Boot2Crc);
#endif
}

void show_fw_header_info(fw_header_t *pstFW)
{
#if APP_DEBUG_ENABLED
    PRINTF("[BL2] FW_Flag= 0x%X\n", pstFW->u32FirmwareFlag);
    PRINTF("[BL2] FW_Ver= 0x%X\n", pstFW->u32FirmwareVer);
    PRINTF("[BL2] FW_Len= 0x%X\n", pstFW->u32FirmwareLen);
    PRINTF("[BL2] FW_LoadAddr= 0x%X\n", pstFW->u32FirmwareAddr);
    PRINTF("[BL2] FW_Crc= 0x%X\n", pstFW->u32FirmwareCrc);
#endif
}

void boot2_peripheral_deinit(void)
{
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_CRC32, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_CRC32);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_RAND, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_RAND);
#if APP_DEBUG_ENABLED
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART0, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART0);
#endif
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_UART1, false);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_UART1);

    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);
}
// #include "hw_timer.h"
void boot2_main(void)
{
    // uint32_t time = 0;
    // rom_hw_timer_get_counter(TIM0, &time);

    peripheral_init();
    // PRINTF("[BL2] Boot2 Start, timer = %d us\n", (0xFFFFFFFF - time) / 50);
    uint32_t u32Boot2Buffer[BOOT2HEADER_LEN / 4] = {0};

    rom_hw_flash_read_data_u32(FLASH_BOOT2_HEADER_ADDR, u32Boot2Buffer, BOOT2HEADER_LEN);

    boot2_header_t* pstBoot2 = (boot2_header_t*)u32Boot2Buffer;

    boot_upgrade_update_boot2_code(pstBoot2);

    memset(u32Boot2Buffer, 0, sizeof(u32Boot2Buffer));
    rom_hw_flash_read_data_u32(FLASH_BOOT2_FIRMWARE_HEADER_ADDR, u32Boot2Buffer, FIRMWARE_HEADER_LEN);

    fw_header_t *pstFirm = (fw_header_t *) u32Boot2Buffer;

    bool ret = boot_upgrade_update_app_code(pstFirm);

    if(ret) {
        // After upgrade, relaod header.
        memset(u32Boot2Buffer, 0, sizeof(u32Boot2Buffer));
        rom_hw_flash_read_data_u32(FLASH_BOOT2_FIRMWARE_HEADER_ADDR, u32Boot2Buffer, sizeof(u32Boot2Buffer));
    }

    show_boot2_header_info((boot2_header_t *)(FLASH_BOOT2_HEADER_ADDR));
    show_fw_header_info((fw_header_t *)(FLASH_BOOT2_FIRMWARE_HEADER_ADDR));
    uint32_t u32LoadAddr = *(volatile uint32_t*)(pstFirm->u32FirmwareAddr);
    PRINTF("[BL2]------BOOT2 Done, jump to 0x%X------\n\n", u32LoadAddr);

    boot2_peripheral_deinit();
    rom_utility_run_callback_noparam(u32LoadAddr);

    while (1);
}



