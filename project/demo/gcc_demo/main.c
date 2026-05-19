#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#include "reg_Dev.h"
#include "error_def.h"
#include "utility.h"
#include "compiler.h"
#include "debug.h"
#include "app_cfg.h"
#include "slc_hal_uart.h"
#include "slc_hal_sysctrl.h"
#include "slc_hal_intc.h"
#include "slc_cali.h"
#include "slc_lpwr_ctrl.h"
#include "slc_pmu.h"

#ifdef SLC_PHYTEST
#include "slc_phy_test.h"
#endif

#ifdef SLC_SOCTEST
#include "slc_soc_test.h"
#endif

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
    slc_hal_sysctrl_peripheral_mod_reset(HAL_CLK_GPIO);   // 复位boot内的PIN8管脚配置

#ifndef SLC_FPGA
    // 校准时切换到TCXO25M，可正常使用外设等，否则时钟波动可能导致打印等异常
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_TCXO25M, HAL_SYSCLK_DIV_NONE);

    slc_hal_pmu_phy_power_enable(true);

#if APP_DEBUG_ENABLED
    debug_printf_init();
#endif
    slc_cali_init(SLC_CALI_FFT_IQ);
#endif

    // 校准结束切换回RC50M，需重新进行UART初始化
    slc_hal_sysctrl_system_clock_init(HAL_SYSCLK_RC50M, HAL_SYSCLK_DIV_NONE);
    // while(1);

#if APP_DEBUG_ENABLED
    debug_printf_init();

#ifdef NT_SHELL
    slc_hal_register_irq_handler(DEBUG_UART_IRQ, slc_debug_uart_irq_handler);
    slc_hal_uart_enable_irq(DEBUG_UART_HANDLE, HAL_UART_INT_EN_RX_NOT_EMPTY);
    SLC_HAL_ENABLE_PERIPHERAL_IRQ(DEBUG_UART_IRQ, 0x3);

    ntshell_top_init();
#endif
#endif
}

int main(void)
{
    slc_check_boot_status();

#ifdef SLC_AUTOTEST
    slc_uart_cmd_process_main();
#endif // SLC_AUTOTEST

    slc_platform_init();

    PRINTF("FW start to work.Build Time:[%s T %s].\n", __DATE__, __TIME__);

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
