#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "debug.h"
#include "slc_hal_uart.h"
#include "slc_hal_sysctrl.h"
#include "app_cfg.h"
#include "slc_hal_intc.h"
#include "slc_hal_delay.h"
#include "slc_cali.h"
#include "slc_lpwr_ctrl.h"
#include "slc_hal_pmu.h"
#include "slc_phy_test.h"
#include "slc_soc_test.h"
#include "compiler.h"
#include "slc_test_slave.h"

#ifdef SLC_AUTOTEST
#include "slc_uart_cmd_process.h"
#endif

#ifdef NT_SHELL
#include "nttop.h"

void slc_debug_uart_irq_handler(void)
{
    uint32_t sta __MAYBE_UNUSED = 0;
    sta = slc_hal_uart_get_irq_status(DEBUG_UART_HANDLE);

    ntshell_top_run_once();
}
#endif

void slc_platform_init(void)
{
    slc_hal_sysctrl_cache_mode_set(HAL_CACHE_ENABLE);

#ifndef SLC_FPGA
    // TODO: MPW规避standby起来后RF未复位的异常，待CS优化后删除
    slc_hal_sysctrl_reset_phy();

    slc_hal_pmu_phy_power_enable(true);
    // vtcxo bypass，使用vBAT供电，切25M之前配置，防止TCXO未工作，切换后CPU卡死
    slc_rf_tcxo_bypass(true);

    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif

    PRINTF("FW start to CALI\n");
    if (slc_cali_init(SLC_CALI_DC_IQ) != 0) {
        PRINTF("FW CALI fail\n");
    }
#endif
    // 校准结束切换回RC50M，需重新进行UART初始化
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_FDB50M, HAL_SYSCLK_DIV_NONE);

#if APP_DEBUG_ENABLED
    debug_printf_init();

#ifdef NT_SHELL
    slc_hal_register_irq_handler(DEBUG_UART_IRQ, slc_debug_uart_irq_handler);
    slc_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();
#endif
#endif

    PRINTF("FW start to work(freq: %dMHz). Build Time:[%s T %s].\n",
            slc_hal_sysctrl_get_system_clock() / 1000000, __DATE__, __TIME__);
}
#include "hw_flash.h"
void flash_read_id_test(void)
{
    uint8_t dev_id[3] = {0};
    uint8_t uid[8] = {0};
    uint8_t buffer[256] = {0};

    rom_hw_flash_read_dev_id(dev_id, 3);
    rom_hw_flash_read_uid(uid, 8);
    dump_u8buf("flash dev id", dev_id, sizeof(dev_id));
    dump_u8buf("flash uid", uid, sizeof(uid));
    rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, buffer, 256);
    dump_u8buf("flash sec mem0", buffer, 256);

    // buffer[36] = 0x2;
    // rom_hw_flash_erase_security_mem(EN_FLASH_SEC_MEM0, FLASH_ERASE_SECURITY_MEM_KEY);
    // rom_hw_flash_write_security_mem(EN_FLASH_SEC_MEM0, 0, buffer, 256);

    // rom_hw_flash_read_security_mem(EN_FLASH_SEC_MEM0, 0, buffer, 256);
    // dump_u8buf("flash sec mem0", buffer, 256);
}

int main(void)
{
    slc_check_boot_status();

#ifdef SLC_AUTOTEST
    slc_uart_cmd_process_main();
#endif // SLC_AUTOTEST

    slc_platform_init();
    flash_read_id_test();

#ifdef SLC_PHYTEST
    slc_phy_test();
#endif // SLC_PHYTEST

#ifdef SLC_SOCTEST
#ifndef SLC_TEST_SLAVE
    soctest_main();
#else
    slc_test_slave_main();
#endif // SLC_TEST_SLAVE
#endif // SLC_SOCTEST

    while (1);
#ifdef __CC_ARM
#pragma diag_suppress 111
#endif
    return 0;
}

void NMI_Handler_Proc(void)
{
    while (1);
}
