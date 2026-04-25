#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "reg_Dev.h"
#include "error_def.h"

#include "hw_flash.h"
#include "hw_sysctrl.h"
#include "hw_pmu.h"
#include "hw_rng.h"
#include "utility.h"
#include "bootloader.h"
#include "app_cfg.h"

int main(void)
{
    uint8_t dev_id[3] = {0};

    boot_identify_from_lowpower();

    rom_hw_sysctrl_system_clock_init(EN_SYS_CLK_RC50M, EN_SYS_CLK_MCLK_DIV_NONE);

    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_CRC32, true);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_CRC32);
    rom_hw_sysctrl_enable_clock_gate(EN_SYSCTRL_RAND, true);
    rom_hw_sysctrl_reset_peripheral(EN_SYSCTRL_RAND);
    rom_hw_rng_init(0, true);
    rom_hw_update_rng_seed(0x12345678);

#if APP_DEBUG_ENABLED
    extern void debug_printf_init(void);
    debug_printf_init();
#endif
    //  1 Close Cache
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_DISABLE);
    rom_hw_sysctrl_set_cache_mode(EN_CACHE_FLUSH);

    // 2 flash common init
    rom_hw_flash_init();

    // 3 Get device id, set flash type
    rom_hw_flash_read_dev_id(dev_id, 3);
    if (dev_id[0] == EN_FLASH_TYPE_PY) {
        rom_hw_flash_set_type(EN_FLASH_TYPE_PY);
    } else {
        rom_hw_flash_set_type(EN_FLASH_TYPE_GT);
    }

    // 4 read security mem, set flash security
    boot_set_flash_enc_by_security_info();

    boot_selection();

    while(1)
    {
        rom_utility_delay_us(10);
    }

    return 0;
}
